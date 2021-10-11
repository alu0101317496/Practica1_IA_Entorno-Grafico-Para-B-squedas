/* =============================================================================
// @Creator: Lucas Hernández Abreu
// @Name: main.cc
// @Purpose: Creating the graphical interface talking with the API of Windows.
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


struct win32_offscreen_buffer
{
    BITMAPINFO Info;
    void *BitmapMemory;
    int Width;
    int Height;
    int BytesPerPixel;
    int Pitch;
};

global_variable win32_offscreen_buffer GlobalBackBuffer;


// TODO(Seriuusly): THIS IS A GLOBAL FOR NOW
global_variable bool Running;



internal void
RenderWeirdGradient(win32_offscreen_buffer Buffer, int XOffset, int YOffset)
{
    int Pitch = Buffer.Width*Buffer.BytesPerPixel;
    uint8 *Row = (uint8 *)Buffer.BitmapMemory;
    
    for(int Y = 0; Y < Buffer.Height; ++Y)
    {
        uint32 *Pixel = (uint32 *)Row;
        
        for(int X = 0; X < Buffer.Width; ++X)
        {
            /*
--                  0  1  2  3
 --Pixel in memory: BB GG RR xx
--
--Memory:        BB GG RR xx
--Register:      xx RR GG BB
*/
            uint8 Blue = (X + XOffset);
            uint8 Green = (Y + YOffset);
            
            *Pixel++ = ((Green << 8) | Blue);
        }
        Row+=Pitch;
    }
}


internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
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
    
    if(Buffer->BitmapMemory)
    {
        VirtualFree(Buffer->BitmapMemory, 0, MEM_RELEASE);
    }
    
    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;
    Buffer->BytesPerPixel = 4;
    int BitmapMemorySize = (Buffer->Width*Buffer->Height)*Buffer->BytesPerPixel;
    
    // Returns pages for the process inside memory.
    Buffer->BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
    
    // TODO(Seriuusly): Probably clear this to black
    
    
}

internal void
Win32DisplayBufferInWindow(HDC DeviceContext, 
                           RECT WindowRect, 
                           win32_offscreen_buffer Buffer,
                           int X, 
                           int Y, 
                           int Width, 
                           int Height)
{
    int WindowWidth = WindowRect.right - WindowRect.left;
    int WindowHeight = WindowRect.bottom - WindowRect.top;
    
    StretchDIBits(DeviceContext,
                  /*
                  X, Y, Width, Height,
                  X, Y, Width, Height,*/
                  0, 0, Buffer.Width, Buffer.Height,
                  0, 0, WindowWidth, WindowHeight,
                  Buffer.BitmapMemory,
                  &Buffer.Info,
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
            Win32ResizeDIBSection(&GlobalBackBuffer, Width, Height);
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
            Win32DisplayBufferInWindow(DeviceContext, ClientRect, GlobalBackBuffer, X, Y, Width, Height);
            
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

int CALLBACK WinMain(HINSTANCE Instance,
                     HINSTANCE PrevInstance,
                     LPSTR CommandLine,
                     int ShowCode)
{
    WNDCLASS WindowClass = {};
    
    
    WindowClass.style = CS_HREDRAW|CS_VREDRAW; // WINDOW STYLE
    WindowClass.lpfnWndProc = MainWindowCallback;
    WindowClass.hInstance = Instance;
    //WINDOW ICON
    //WindowClass.hIcon = ;          
    WindowClass.lpszClassName = "Graphic loader windows class";
    
    if(RegisterClass(&WindowClass))
    {
        HWND Window = CreateWindowEx(
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
        if(Window)
        {
            int XOffset = 0;
            int YOffset = 0;
            Running = true;
            while(Running)
            {
                
                MSG Message;
                while(PeekMessage(&Message, 0 ,0 , 0, PM_REMOVE))
                {
                    if(Message.message == WM_QUIT) Running = false;
                    
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                RenderWeirdGradient(GlobalBackBuffer, XOffset, YOffset);
                HDC DeviceContext = GetDC(Window);
                
                RECT ClientRect;
                GetClientRect(Window, &ClientRect);
                int WindowWidth = ClientRect.right - ClientRect.left;
                int WindowHeight = ClientRect.bottom - ClientRect.top;
                Win32DisplayBufferInWindow(DeviceContext, ClientRect, GlobalBackBuffer, 0, 0, WindowWidth, WindowHeight);
                ReleaseDC(Window, DeviceContext);
                
                XOffset++;
            }
        }
        else
        {
            // TODO(Seriuusly): Logging
        }
    }
    else
    {
        // TODO(Seriuusly): LOGGING
    }
    
    return (0);
}