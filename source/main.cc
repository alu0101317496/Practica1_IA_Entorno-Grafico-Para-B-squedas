/* =============================================================================
 // Creator: Lucas Hernández Abreu
// Name: main.cc
// Purpose: Creating the graphical interface talking with the API of Windows.
//
       //============================================================================== */
#include <windows.h>

#define internal          static //Internal variables or functions specific to the brackets they're inside
#define local_persist     static
#define global_variable   static



// TODO(Seriuusly): THIS IS A GLOBAL FOR NOW
global_variable bool Running;


internal void
Win32ResizeDIBSection()
{
    HBITMAP CreateDIBSection(
                             HDC              hdc,
                             BITMAPINFO       *pbmi,
                             UINT             usage,
                             VOID             **ppvBits,
                             HANDLE           hSection,
                             DWORD            offset
                             );
}

internal void
Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width, int Height)
{
    StretchDIBits(
                  DeviceContext,
                  X, Y, Width, Height,
                  X, Y, Width, Height,
                  
                  // TODO(Seriuusly): Vas POR AQuí 37:06 BACK BUFFER ALLOCATE NEGRATA
                  
                  const VOID       *lpBits,
                  const BITMAPINFO *lpbmi,
                  UINT             iUsage,
                  DWORD            rop
                  );
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
            ResizeDIBSection(Width, Height);
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