// values of the world
var worldWidth = 10;
var worldHeight = 10;
var percentage = 30;

// total size of the world (height + width)
var size;

// size of a tile in pixels
var tileWidth;
var tileHeight;

// canvas element
var canvas, ctx, spritesheet;

// true when the spritesheet has been downloaded
var spritesheetLoaded = false;

// the world grid (2d array of tiles)
var world = [[]];

// ----------------------------------------------------------------------------------------------------------------------------
var path = 0;

// start and end of path
var pathStart = [worldWidth, worldHeight];
var pathEnd = [0, 0];
var currentPath = [];


// ensure that concole.log doesn't cause errors
if (typeof console == "undefined") var console = {
	log: function() {}
};

// set the values of width, height and percentage
function SetWorld() {
	worldWidth = document.getElementById('Width').value;
	if (worldWidth == '' || worldWidth <= 0) {
		alert("NOT A VALID WIDTH INPUT! (Set 10 as default)");
		worldWidth = 10;
	}

	worldHeight = document.getElementById('Height').value;
	if (worldHeight == '' || worldWidth <= 0) {
		alert("NOT A VALID HEIGHT INPUT! (Set 10 as default");
		worldHeight = 10;
	}

	size = parseInt(worldWidth,10) + parseInt(worldHeight, 10);

	if (size <= 50) {
		tileWidth = 32;
		tileHeight = 32;
	} else if ((size > 50) && (size <= 400)) {
		tileWidth = 8;
		tileHeight = 8;
	} else {
		tileWidth = 2;
		tileHeight = 2;
	}

	percentage = document.getElementById('Percentage').value;

	if (percentage > 100 || percentage < 0 || percentage == '') {
		alert("NOT A VALID PERCENTAGE INPUT! (Setting 30 as default percentage)");
        percentage = 30;
    }

	onload();
}

// the html page is ready
function onload() {
	canvas = document.getElementById('gameCanvas');
	canvas.width = worldWidth * tileWidth;
	canvas.height = worldHeight * tileHeight;
	canvas.addEventListener("click", canvasClick, false);
	canvas.oncontextmenu = rightClick;
	ctx = canvas.getContext("2d");
	spritesheet = new Image();

	if (size <= 50) {
		spritesheet.src = './img/Tiles160x32.png'
	} else if ((size > 50) && (size <= 400)) {
		spritesheet.src = './img/Tiles40x8.png'
	} else {
		spritesheet.src = './img/Tiles10x2.png'
	}
	spritesheet.onload = loaded;
}

// the spritesheet is ready
function loaded() {
	spritesheetLoaded = true;
	clearWorld();
	createWorld();
}

// clear the world and path
function clearWorld() {
	// create emptiness
	for (var x = 0; x < worldWidth; x++) {
		world[x] = [];
		for (var y = 0; y < worldHeight; y++) {
			world[x][y] = 0;
		}
	}

	// remove old path
	pathStart = [0, 0];
	pathEnd = [0, 0];
	currentPath = findPath(world, pathStart, pathEnd);
	cleanCanvas();
}

// fill the world with walls
function createWorld() {
	// create emptiness
	for (var x = 0; x < worldWidth; x++) {
		world[x] = [];
		for (var y = 0; y < worldHeight; y++) {
			world[x][y] = 0;
		}
	}

	// calculate the number of obtacles and set the obstacles randomly
	var obstacles = (worldHeight * worldWidth) * (percentage / 100);
	var i = 0;
	var j = 0;
	var k = 0;
	while (i < obstacles) {
		j = Math.floor(Math.random() * worldWidth);
		k = Math.floor(Math.random() * worldHeight);
		if(world[j][k] != 1) {
			world[j][k] = 1;
			i = i + 1;
		}
	}
	redrawWalls();
}

// Original redraw (not recomended to use, not optimal)
function redraw() {
	if (!spritesheetLoaded) {return};
	var spriteNum = 0;

	// clear the screen
	ctx.fillStyle = '#000000';
	ctx.fillRect(0, 0, canvas.width, canvas.height);

	// draw world with walls
	for (var x = 0; x < worldWidth; x++) {
		for (var y = 0; y < worldHeight; y++) {
			switch (world[x][y]) {
				case 1:
					spriteNum = 1;
					break;
				default:
					spriteNum = 0;
					break;
			}
			ctx.drawImage(spritesheet, spriteNum * tileWidth, 0,
						  tileWidth, tileHeight, x * tileWidth,
						  y * tileHeight, tileWidth, tileHeight);
		}
	}

	// draw the path
	for (rp = 0; rp < currentPath.length; rp++) {
		switch (rp) {
			case 0:
				spriteNum = 2; // start
				break;
			case currentPath.length - 1:
				spriteNum = 3; // end
				break;
			default:
				spriteNum = 4; // path node
				break;
		}

		ctx.drawImage(spritesheet,
			spriteNum * tileWidth, 0,
			tileWidth, tileHeight,
			currentPath[rp][0] * tileWidth,
			currentPath[rp][1] * tileHeight,
			tileWidth, tileHeight);
	}
}

// redraw only the walls
function redrawWalls() {
	if (!spritesheetLoaded) {return};

	for (var x = 0; x < worldWidth; x++) {
		for (var y = 0; y < worldHeight; y++) {
			if (world[x][y] == 1) {
				ctx.drawImage(spritesheet, 1 * tileWidth, 0,
							  tileWidth, tileHeight, x * tileWidth,
							  y * tileHeight, tileWidth, tileHeight);
			}
		}
	}
}

// redraw only the path
function redrawPath() {
	if (!spritesheetLoaded) {return};
	var spriteNum = 0;

	// draw the path
	for (rp = 0; rp < currentPath.length; rp++) {
		switch (rp) {
			case 0:
				spriteNum = 2; // start
				break;
			case currentPath.length - 1:
				spriteNum = 3; // end
				break;
			default:
				spriteNum = 4; // path node
				break;
		}

		ctx.drawImage(spritesheet,
			spriteNum * tileWidth, 0,
			tileWidth, tileHeight,
			currentPath[rp][0] * tileWidth,
			currentPath[rp][1] * tileHeight,
			tileWidth, tileHeight);
	}	
}

// clear the old path
function clearPath() {
	if (!spritesheetLoaded) {return};
	var spriteNum = 0;

	// clear path
	for (rp = 0; rp < currentPath.length; rp++) {
		spriteNum = 0;
		ctx.drawImage(spritesheet,
			spriteNum * tileWidth, 0,
			tileWidth, tileHeight,
			currentPath[rp][0] * tileWidth,
			currentPath[rp][1] * tileHeight,
			tileWidth, tileHeight);
	}	
}

// clear the world
function cleanCanvas() {
	if (!spritesheetLoaded) {return};
	// clear the screen
	ctx.fillStyle = '#000000';
	ctx.fillRect(0, 0, canvas.width, canvas.height);
}

// handle right click events on the canvas
function rightClick(e) {
	// variables to save the page coords
	var x;
	var y;

	// grab html page coords
	if (e.pageX != undefined && e.pageY != undefined) {
		x = e.pageX;
		y = e.pageY;
	}
	else {
		x = e.clientX + document.body.scrollLeft +
			document.documentElement.scrollLeft;
		y = e.clientY + document.body.scrollTop +
			document.documentElement.scrollTop;
	}

	// make them relative to the canvas only
	x -= canvas.offsetLeft;
	y -= canvas.offsetTop;

	// return tile x,y that we clicked
	var cell = [
		Math.floor(x / tileWidth),
		Math.floor(y / tileHeight)
	];

	// toggle it into an obstacle
	world[cell[0]][cell[1]] = (world[cell[0]][cell[1]] == 1) ? 0 : 1;
	ctx.drawImage(spritesheet,
		world[cell[0]][cell[1]] * tileWidth, 0,
		tileWidth, tileHeight,
		cell[0] * tileWidth,
		cell[1] * tileHeight,
		tileWidth, tileHeight);

	// recalculate path
	clearPath();
	currentPath = findPath(world, pathStart, pathEnd);
	redrawPath();

	// Prevent context menu
	return false;
}

// handle click events on the canvas
function canvasClick(e) {
	var x;
	var y;

	// grab html page coords
	if (e.pageX != undefined && e.pageY != undefined) {
		x = e.pageX;
		y = e.pageY;
	}
	else {
		x = e.clientX + document.body.scrollLeft +
			document.documentElement.scrollLeft;
		y = e.clientY + document.body.scrollTop +
			document.documentElement.scrollTop;
	}

	// make them relative to the canvas only
	x -= canvas.offsetLeft;
	y -= canvas.offsetTop;

	// return tile x,y that we clicked
	var cell = [
		Math.floor(x / tileWidth),
		Math.floor(y / tileHeight)
	];

	// Prevent placing inside obstacle
	if (world[cell[0]][cell[1]] == 0) {
		if (!path) {
			// remove old path
			pathStart = cell;
			pathEnd = cell;

			clearPath();
			currentPath = findPath(world, pathStart, pathEnd);
		}

		// now we know while tile we clicked
		console.log('we clicked tile ' + cell[0] + ',' + cell[1]);

		path = !path;

		if (path) {
			pathStart = cell;
		}
		else {
			pathEnd = cell;
		}
		//Se han seteado bien los valores de inicio y de final
		// calculate path
		currentPath = findPath(world, pathStart, pathEnd);
	}
	redrawPath();
}

// world is a 2d array of integers
// pathStart and pathEnd are arrays like [5,10]
function findPath(world, pathStart, pathEnd) {
	// shortcuts for speed
	var abs = Math.abs;

	// keep track of the world dimensions
	var worldSize = worldWidth * worldHeight;

	// which heuristic should we use?
	// default: Manhattan (recommended)
	var distanceFunction = ManhattanDistance;

	// distanceFunction functions
	// these return how far away a point is to another (source to destination)
	function ManhattanDistance(Point, Goal) {
		return abs(Point.x - Goal.x) + abs(Point.y - Goal.y);
	}
	
	function EuclideanDistance(Point, Goal) {
		return Math.sqrt((abs(Goal.x - Point.x)) * (abs(Goal.x - Point.x)) + 
						 (abs(Goal.y - Point.y)) * (abs(Goal.y - Point.y)));
	}

	// Returns every available North, South, East or West
	// cell that is empty. No diagonals,
	function Neighbours(x, y) {
		// declare de directions (N, S, E, W) and match them only if 
		// the booleans respectively are true
		var N = y - 1,
			S = y + 1,
			E = x + 1,
			W = x - 1,
			myN = N > -1 && canWalkHere(x, N),
			myS = S < worldHeight && canWalkHere(x, S),
			myE = E < worldWidth && canWalkHere(E, y),
			myW = W > -1 && canWalkHere(W, y),
			result = [];
		if (myN)
			result.push(
			{
				x: x,
				y: N
			});
		if (myE)
			result.push(
			{
				x: E,
				y: y
			});
		if (myS)
			result.push(
			{
				x: x,
				y: S
			});
		if (myW)
			result.push(
			{
				x: W,
				y: y
			});
		return result;
	}

	// returns boolean value (world cell is available and open)
	function canWalkHere(x, y) {
		return ((world[x]    != null) &&
				(world[x][y] != null) &&
				(world[x][y] == 0));
	};

	// Node function, returns a new object with Node properties
	// Used in the calculatePath function to store route costs, etc.
	function Node(Parent, Point) {
		var newNode = {
			// pointer to another Node object
			Parent: Parent,
			// array index of this Node in the world linear array
			value: Point.x + (Point.y * worldWidth),
			// the location coordinates of this Node
			x: Point.x,
			y: Point.y,
			// the heuristic estimated cost of an entire path using this node
			f: 0,
			// the distanceFunction cost to get from the starting point to this node
			g: 0
		};
		return newNode;
	}

	// Path function, executes AStar algorithm operations
	function calculatePath() {
		// create Nodes from the Start and End x, y coordinates
		var mypathStart = Node(null, {
			x: pathStart[0],
			y: pathStart[1]
		});
		var mypathEnd = Node(null, {
			x: pathEnd[0],
			y: pathEnd[1]
		});
		var AStar = new Array(worldSize);		// create an array that will contain all world cells
		var Open = [mypathStart];				// list of currently open Nodes
		var Closed = [];						// list of closed Nodes
		var result = [];						// list of the final output array
		var myNeighbours;						// reference to a Node (that is nearby)
		var myNode;								// reference to a Node (that we are considering now)
		var myPath;								// reference to a Node (that starts a path in question)
		var length, max, min, i, j;				// temp integer variables used in the calculations

		// iterate through the open list until none are left
		while (length = Open.length) {			
			max = worldSize;
			min = -1;
			for (i = 0; i < length; i++) {
				if (Open[i].f < max) {
					max = Open[i].f;
					min = i;
				}
			}
			// grab the next node and remove it from Open array
			myNode = Open.splice(min, 1)[0];
			// is it the destination node?
			if (myNode.value === mypathEnd.value) {
				myPath = Closed[Closed.push(myNode) - 1];
				do {
					result.push([myPath.x, myPath.y]);
				}
				while (myPath = myPath.Parent);
				// clear the working arrays
				AStar = Closed = Open = [];
				// we want to return start to finish
				result.reverse();
			}
			else { // not the destination
				// find which nearby nodes are walkable
				myNeighbours = Neighbours(myNode.x, myNode.y);
				// test each one that hasn't been tried already
				for (i = 0, j = myNeighbours.length; i < j; i++) {
					myPath = Node(myNode, myNeighbours[i]);
					if (!AStar[myPath.value]) {
						// estimated cost of this particular route so far
						myPath.g = myNode.g + distanceFunction(myNeighbours[i], myNode);
						// estimated cost of entire guessed route to the destination
						myPath.f = myPath.g + distanceFunction(myNeighbours[i], mypathEnd);
						// remember this new path for testing above
						Open.push(myPath);
						// mark this node in the world graph as visited
						AStar[myPath.value] = true;
					}
				}
				// remember this route as having no more untested options
				Closed.push(myNode);
			}
		} // keep iterating until the Open list is empty
		return result;
	}
	return calculatePath();
}