#include <bits/stdc++.h>

#define COL 25
#define ROW 20

typedef std::pair<int, int> Pair;
typedef std::pair<double, std::pair<int, int>> pPair;

//Nuestro algoritmo requiere guardar ciertos datos en cada celda de la matriz,
//así que definimos la estructura celda (más cómodo que crear una clase)
struct cell {
    //Coordenadas x e y del padre
	int parent_i, parent_j;
	// f = g + h
	double f, g, h;
};

/*-------------A partir de aquí definimos las funciones implementadas--------*/

//Función que revisa si una celda dada es apta o no
bool IsValid(int row, int col) {
	//Retornamos true si estamos en rango (fila, columna)
	return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL);
}

//Función que revisa si la celda está bloqueada o no
bool IsUnblock(int matrix[][COL], int row, int col) {
	//Retornamos true si la celda no está bloqueada
	return matrix[row][col] == 1 ? (true) : (false);
}

//Función que revisa si la celda es la de destino destino
bool IsDestination(int row, int col, Pair dest) {
	return (row == dest.first && col == dest.second) ? true : false;
}

//Función que calcula el valor 'h' (heuristico).
double CalculateHValue(int row, int col, Pair dest) {
	//Formula para diagonales
	// return ((double)sqrt((row - dest.first) * (row - dest.first) + 
    //                      (col - dest.second) * (col - dest.second)));

    //Formula sin diagonales
    return (abs(dest.first - row) + abs(dest.second - col));
}

//Función que traza el camino desde el origen al destino
void TracePath(cell cell_details[][COL], Pair dest) {
	int row = dest.first;
	int col = dest.second;

	std::stack<Pair> path;

	while (!(cell_details[row][col].parent_i == row && 
             cell_details[row][col].parent_j == col)) {
		path.push(std::make_pair(row, col));
		int temp_row = cell_details[row][col].parent_i;
		int temp_col = cell_details[row][col].parent_j;
		row = temp_row;
		col = temp_col;
	}

    // //Extraemos el camino de vuelta
	// path.push(std::make_pair(row, col));
	// while (!path.empty()) {
	// 	Pair p = path.top();
	//     path.pop();
    //     printf("-> (%d,%d) ", p.first, p.second);
	// }

	return;
}

//Función encargada de encontrar el camino mínimo entre dos puntos de una 
//matriz dada, mediante la implmentación del algoritmo A*
void AStarSearch(int world[][COL], Pair src, Pair dest) {

    /*Empezamos consultando los casos excepcionales*/

	//Revisamos que el punto de inicio se encuentre en rango
	if (!IsValid(src.first, src.second)) {return;}

	//Revisamos que el punto de destino se encuentre en rango
	if (!IsValid(dest.first, dest.second)) {return;}

	//Revisamos que las casillas de origen y destino no estén bloqueadas
	if (!IsUnblock(world, src.first, src.second) || 
        !IsUnblock(world, dest.first, dest.second)) {return;}

	//Si la celda de origen es la de destino
	if (IsDestination(src.first, src.second, dest)) {return;}

	//Creamos una "matriz" de booleanos e inicializamos a false (ninguna celda 
    //ha sido incluida todavia)
	bool closed_list[ROW][COL];
	memset(closed_list, false, sizeof(closed_list));

	//Declaramos una "matriz" de celdas, para almacenar los detalles de las 
    //celdas de la matriz
	cell cell_details[ROW][COL];

    //Inicializamos los parámetros de la "matriz" de celdas
    //NOTA: FLT_MAX -> Maximum finite representable floating-point number.
	int i, j;
	for (i = 0; i < ROW; i++) {
		for (j = 0; j < COL; j++) {
			cell_details[i][j].f = FLT_MAX;
			cell_details[i][j].g = FLT_MAX;
			cell_details[i][j].h = FLT_MAX;
			cell_details[i][j].parent_i = -1;
			cell_details[i][j].parent_j = -1;
		}
	}

	//Inicializamos los parámetros del nodo de arranque
	i = src.first, j = src.second;
	cell_details[i][j].f = 0.0;
	cell_details[i][j].g = 0.0;
	cell_details[i][j].h = 0.0;
	cell_details[i][j].parent_i = i;
	cell_details[i][j].parent_j = j;

	// Creamos la open list, que tendrá información del tipo <f, <i, j>>, donde
    // f = g + h, e i, j son los índices de fila y columna de dicha celda
	std::set<pPair> open_list;

    // open_list	-> casillas sin analizar, que nos siguen siendo útiles
    // closed_list	-> casillas ya visitadas  

	//Ponemos la celda inicial en la open list, con su 'f' igual a 0
	open_list.insert(std::make_pair(0.0, std::make_pair(i, j)));

    //Declaramos e inicializamos el booleano que indica si hemos acabado	
    bool found_dest = false;

    //Hasta que no hayamos terminado de vaciar la lista (seguimos teniendo 
    //candidatos)
	while (!open_list.empty()) {
        //Analizamos el valor en el top de la lista para analizarlo
		pPair p = *open_list.begin();
		open_list.erase(open_list.begin());

		//Marcamos que lo estamos analizando
		i = p.second.first;
		j = p.second.second;
		closed_list[i][j] = true;

		/*
		Generamos los 4 sucesores de la celda

                 N
		         |
			W--Cell--E
				 |
                 S
		Cell-->Popped Cell  (i, j)
		N --> North         (i-1, j)
		S --> South         (i+1, j)
		E --> East          (i, j+1)
		W --> West          (i, j-1)
        */

		//Variables auxiliares para almacenar los valores 'g', 'h' y 'f' de 
        //los 4 sucesores
		double g_new, h_new, f_new;

		//----------- 1st Successor (North) ------------
		//Solo procesamos si es válida
		if (IsValid(i - 1, j)) {
			//Revisamos que si es la de destino
			if (IsDestination(i - 1, j, dest)) {
				//Indicamos el padre y finalizamoss el recorrido
				cell_details[i - 1][j].parent_i = i;
				cell_details[i - 1][j].parent_j = j;
				TracePath(cell_details, dest);
				found_dest = true;
				return;
			}
			//Si el sucesor ya está en la lista cerrada o si está bloqueado 
            //ignoramos, en caso opuesto seguimos el procesamiento
			else if (!closed_list[i - 1][j] && IsUnblock(world, i - 1, j)) {
				g_new = cell_details[i][j].g + 1.0;
				h_new = CalculateHValue(i - 1, j, dest);
				f_new = g_new + h_new;

				//Si no está en la open list, lo agregamos, indicamos que el 
                //padre es la celda actual y almacenamos los valores f, g, y h 
                //de la celda
                //
                //Por otro lado:
				//
                //Si ya está en la open list, revisamos si el coste para 
                //llegar a dicha celda en menor por el camino actual o por el 
                //ya almacenado (valor f)
				if (cell_details[i - 1][j].f == FLT_MAX || 
                    cell_details[i - 1][j].f > f_new) {
					open_list.insert(std::make_pair(f_new, 
                                                    std::make_pair(i - 1, j)));

					//Actualizamos los valores de la celda
					cell_details[i - 1][j].f = f_new;
					cell_details[i - 1][j].g = g_new;
					cell_details[i - 1][j].h = h_new;
					cell_details[i - 1][j].parent_i = i;
					cell_details[i - 1][j].parent_j = j;
				}
			}
		}

		//----------- 2nd Successor (South) ------------

		if (IsValid(i + 1, j)) {
			if (IsDestination(i + 1, j, dest)) {
				cell_details[i + 1][j].parent_i = i;
				cell_details[i + 1][j].parent_j = j;
				TracePath(cell_details, dest);
				found_dest = true;
				return;
			}
			else if (!closed_list[i + 1][j] && IsUnblock(world, i + 1, j)) {
				g_new = cell_details[i][j].g + 1.0;
				h_new = CalculateHValue(i + 1, j, dest);
				f_new = g_new + h_new;

                if (cell_details[i + 1][j].f == FLT_MAX || 
                    cell_details[i + 1][j].f > f_new) {
					open_list.insert(std::make_pair(f_new,  
                                                    std::make_pair(i + 1, j)));

					cell_details[i + 1][j].f = f_new;
					cell_details[i + 1][j].g = g_new;
					cell_details[i + 1][j].h = h_new;
					cell_details[i + 1][j].parent_i = i;
					cell_details[i + 1][j].parent_j = j;
				}
			}
		}

		//----------- 3rd Successor (East) ------------

		if (IsValid(i, j + 1)) {
			if (IsDestination(i, j + 1, dest)) {
				cell_details[i][j + 1].parent_i = i;
				cell_details[i][j + 1].parent_j = j;
				TracePath(cell_details, dest);
				found_dest = true;
				return;
			}

			else if (!closed_list[i][j + 1] && IsUnblock(world, i, j + 1)) {
				g_new = cell_details[i][j].g + 1.0;
				h_new = CalculateHValue(i, j + 1, dest);
				f_new = g_new + h_new;

				if (cell_details[i][j + 1].f == FLT_MAX || 
                    cell_details[i][j + 1].f > f_new) {
					open_list.insert(std::make_pair(f_new, 
                                                    std::make_pair(i, j + 1)));

					cell_details[i][j + 1].f = f_new;
					cell_details[i][j + 1].g = g_new;
					cell_details[i][j + 1].h = h_new;
					cell_details[i][j + 1].parent_i = i;
					cell_details[i][j + 1].parent_j = j;
				}
			}
		}

		//----------- 4th Successor (West) ------------

		if (IsValid(i, j - 1)) {
			if (IsDestination(i, j - 1, dest)) {
				cell_details[i][j - 1].parent_i = i;
				cell_details[i][j - 1].parent_j = j;
				TracePath(cell_details, dest);
				found_dest = true;
				return;
			}

			else if (!closed_list[i][j - 1] && IsUnblock(world, i, j - 1)) {
				g_new = cell_details[i][j].g + 1.0;
				h_new = CalculateHValue(i, j - 1, dest);
				f_new = g_new + h_new;

				if (cell_details[i][j - 1].f == FLT_MAX || 
                    cell_details[i][j - 1].f > f_new) {
					open_list.insert(std::make_pair(f_new, 
                                                    std::make_pair(i, j - 1)));

					cell_details[i][j - 1].f = f_new;
					cell_details[i][j - 1].g = g_new;
					cell_details[i][j - 1].h = h_new;
					cell_details[i][j - 1].parent_i = i;
					cell_details[i][j - 1].parent_j = j;
				}
			}
		}
    }
}


/*_______________________________________________________________________________*/

float percentage = 30;        //Proporción de obstaculos en el mundo

void Print_world(std::vector<std::vector<int>>& world) {
    for (size_t i = 0; i < COL; i++) {
        for (size_t j = 0; j < ROW; j++) {
            std::cout << world[i][j] << " ";
        }
        std::cout << "\n";
    }   
}

int main() {
    int obstacles;          //Variable auxiliar para insertar los obstáculos
    std::vector<std::vector<int>> world;
    world.resize(COL);
    for (size_t i = 0; i < COL; i++) {
        world[i].resize(ROW);
    }

    for (size_t i = 0; i < COL; i++) {
        for (size_t j = 0; j < ROW; j++) {
            world[i][j] = 0;
        }
    }
    
    int n_obstacles = (COL * ROW) * (percentage / (float)100);
    int i = 0;
    srand(time(NULL));
    while(i < n_obstacles) {

        int j = rand() % COL;
        int k = rand() % ROW;
        if (world[j][k] != 1) {
            world[j][k] = 1; 
            i++;
        }
    }
    Print_world(world);
}