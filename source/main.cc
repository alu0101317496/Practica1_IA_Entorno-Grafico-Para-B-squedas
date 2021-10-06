/* =============================================================================
 // @Creator: Lucas Hernández Abreu
// @Name: main.cc
// @Purpose: Creating the graphical interface talking with the API of Windows.
//
       //============================================================================== */
#include <windows.h>

#define internal          static //Internal variables or functions specific to the brackets they're inside
#define local_persist     static
#define global_variable   static



// TODO(Seriuusly): THIS IS A GLOBAL FOR NOW
global_variable bool Running;
global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable HBITMAP BitmapHandle;
global_variable HDC BitmapDeviceContext;

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
    
    if(BitmapHandle)
    {
        DeleteObject(BitmapHandle);
    }
    
    if(!BitmapDeviceContext)
    {
        BitmapDeviceContext = CreateCompatibleDC(0);
    }
    
    BITMAPINFO BitmapInfo = {};
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = Width;
    BitmapInfo.bmiHeader.biHeight = Height;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;
    BitmapInfo.bmiHeader.biSizeImage = 0;
    BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    BitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    BitmapInfo.bmiHeader.biClrUsed = 0;
    BitmapInfo.bmiHeader.biClrImportant = 0;
    
    // TODO(Seriuusly): Free after if the creation succeded, if it fails user de old DIB
    
    HBITMAP BitmapHandle = CreateDIBSection(BitmapDeviceContext,
                                            &BitmapInfo,
                                            DIB_RGB_COLORS,
                                            &BitmapMemory,
                                            0, 0);
}

internal void
Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width, int Height)
{
    StretchDIBits(DeviceContext,
                  X, Y, Width, Height,
                  X, Y, Width, Height,
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
            int Height = ClientRect.top - ClientRect.bottom;;
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
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Width = Paint.rcPaint.left - Paint.rcPaint.right;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            Win32UpdateWindow(DeviceContext, X, Y, Width, Height);
            
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
    WindowClass.lpszMenuName = "GraphicLoader";
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
                BOOL MessageResult = GetMessage(&Message, WindowHandle,0,0);
                
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