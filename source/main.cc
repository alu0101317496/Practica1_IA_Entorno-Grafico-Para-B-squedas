/* =============================================================================
 // @Creator: Lucas Hernández Abreu
// @Name: main.cc
// @Purpose: Creating the graphical interface talking with the API of Windows.
//
       //============================================================================== */
#include <windows.h>
#include <stdint.h>

#define internal          static //Internal variables or functions specific to the brackets they're inside
#define local_persist     static
#define global_variable   static

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;


typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;



// TODO(Seriuusly): THIS IS A GLOBAL FOR NOW
global_variable bool Running;
global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
global_variable int BytesPerPixel = 4;


internal void
RenderWeirdGradient(int XOffset, int YOffset)
{
    int Width = BitmapWidth;
    int Height = BitmapHeight;
    
    
    int Pitch = Width*BytesPerPixel;
    uint8 *Row = (uint8 *)BitmapMemory;
    
    
    for(int Y = 0; Y < BitmapHeight; ++Y)
    {
        uint8 *Pixel = (uint8 *)Row;
        for(int X = 0; X < BitmapWidth; ++X)
        {
            /*
--                  0  1  2  3
 --Pixel in memory: BB GG RR xx
*/
            
            *Pixel = (uint8)(X+XOffset); // BLUE
            ++Pixel;
            
            *Pixel = (uint8)(Y+YOffset); // GREEN
            ++Pixel;
            
            *Pixel = 0; // RED
            ++Pixel;
            
            *Pixel = 0; //xx
            ++Pixel;
            
        }
        
        Row+=Pitch;
    }
}


internal void
Win32ResizeDIBSection(int Width, int Height)
{
    /*
@INFO: BitmapinfoHeader Structure
typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
*/
    
    // TODO(Seriuusly): Bulletproof this
    //Free now or later or idk.
    
    if(BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }
    
    BitmapWidth = Width;
    BitmapHeight = Height;
    
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;
    
    int BytesPerPixel = 4;
    int BitmapMemorySize = (BitmapWidth*BitmapHeight)*BytesPerPixel;
    
    // Returns pages for the process inside memory.
    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE); 
    
    RenderWeirdGradient(0,0);
    
    
    
}

internal void
Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int X, int Y, int Width, int Height)
{
    int WindowWitdh = WindowRect->right - WindowRect->left;
    int WindowHeight = WindowRect->bottom - WindowRect->top;
    
    /*
--
--
--
--
*/
    
    StretchDIBits(DeviceContext,
                  /*
                  X, Y, Width, Height,
                  X, Y, Width, Height,*/
                  0, 0, BitmapWidth, BitmapHeight,
                  0, 0, WindowWitdh, WindowHeight,
                  BitmapMemory,
                  &BitmapInfo,
                  DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK 
MainWindowCallback(HWND Window,
                   UINT Message,
                   WPARAM Wparam,
                   LPARAM Lparam)
{
    LRESULT Result = 0;
    
    switch(Message)
    {
        case WM_SIZE:
        {
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            int Width = ClientRect.right - ClientRect.left;
            int Height = ClientRect.bottom - ClientRect.top;
            Win32ResizeDIBSection(Width, Height);
            OutputDebugStringA("WM_SIZE\n");
        }break;
        
        case WM_DESTROY:
        {
            // TODO(Seriuusly): Handle as an Error, Recreate window?
            Running = false;
            OutputDebugStringA("WM_DESTROY\n");
        }break;
        
        case WM_CLOSE:
        {
            // TODO(Seriuusly): Handle with a message for the user
            Running = false;
            PostQuitMessage(0);
            OutputDebugStringA("WM_CLOSE\n");
        }break;
        
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        }break;
        
        case WM_PAINT:
        {
            OutputDebugStringA("WM_PAINT\n");
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
            
            EndPaint(Window, &Paint);
            
        } break;
        
        default:
        {
            Result = DefWindowProc(Window, Message, Wparam, Lparam);
        }break;
        
    }
    
    return (Result);
}





// NOTE(Seriuusly): THE MAIN PROGRAM
int CALLBACK WinMain(
                     HINSTANCE Instance,
                     HINSTANCE PrevInstance,
                     LPSTR CommandLine,
                     int ShowCode)
{
    WNDCLASS WindowClass = {};
    
    
    WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW; // WINDOW STYLE
    WindowClass.lpfnWndProc = MainWindowCallback;
    WindowClass.hInstance = Instance;
    //WINDOW ICON
    //WindowClass.hIcon = ;          
    WindowClass.lpszClassName = "Graphic loader windows class";
    
    if(RegisterClass(&WindowClass))
    {
        HWND WindowHandle = CreateWindowEx(
                                           0,
                                           WindowClass.lpszClassName, 
                                           "GraphicLoader",
                                           WS_OVERLAPPEDWINDOW|WS_VISIBLE, 
                                           CW_USEDEFAULT, 
                                           CW_USEDEFAULT, 
                                           CW_USEDEFAULT, 
                                           CW_USEDEFAULT,  
                                           0, 
                                           0, 
                                           Instance, 
                                           0); 
        if(WindowHandle)
        {
            MSG Message;
            
            Running = true;
            while(Running)
            {
                BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
                
                if (MessageResult > 0)
                {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                else
                {
                    break;
                }
            }
        }
    }
    
    else
    {
        // TODO(Seriuusly): LOGGING
    }
    
    return (0);
}