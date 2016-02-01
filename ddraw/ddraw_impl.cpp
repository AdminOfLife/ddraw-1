#include "ddraw_impl.h"
#include "ddraw_impl_dx.h"
#include "surface_impl.h"
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi")

PFNGLGENBUFFERSARBPROC glGenBuffersARB = 0;                     // VBO Name Generation Procedure
PFNGLBINDBUFFERARBPROC glBindBufferARB = 0;                     // VBO Bind Procedure
PFNGLBUFFERDATAARBPROC glBufferDataARB = 0;                     // VBO Data Loading Procedure
PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB = 0;               // VBO Sub Data Loading Procedure
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = 0;               // VBO Deletion Procedure
PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB = 0; // return various parameters of VBO
PFNGLMAPBUFFERARBPROC glMapBufferARB = 0;                       // map VBO procedure
PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB = 0;                   // unmap VBO procedure
PFNGLCREATESHADEROBJECTARBPROC glCreateShader = 0;
PFNGLSHADERSOURCEARBPROC glShaderSourceARB = 0;
PFNGLCOMPILESHADERARBPROC glCompileShaderARB = 0;
PFNGLCREATEPROGRAMPROC glCreateProgram = 0;
PFNGLATTACHSHADERPROC glAttachShader = 0;
PFNGLLINKPROGRAMPROC glLinkProgram = 0;
PFNGLUSEPROGRAMPROC glUseProgram = 0;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = 0;
PFNGLACTIVETEXTUREPROC glActiveTexture = 0;
PFNGLUNIFORM1IPROC glUniform1i = 0;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = 0;
WNDPROC old_proc = 0;

bool windowed = false, pre_render = false, change_resolution = false;

ddraw_impl::ddraw_impl() : hdc(NULL), hwnd(NULL), w(0), h(0)
{
}

HRESULT WINAPI ddraw_impl::CreateSurface(LPDDSURFACEDESC surfacedesc, LPDIRECTDRAWSURFACE* surface, IUnknown* unk)
{
    this->surface = new surface_impl(this->w, this->h, this->hdc, !pre_render);
    *surface = (LPDIRECTDRAWSURFACE)this->surface;
    return S_OK;
}

HRESULT WINAPI ddraw_impl::CreatePalette(DWORD flags, LPPALETTEENTRY colorarray, LPDIRECTDRAWPALETTE* palette, IUnknown* unk)
{
    palette_impl* p_impl = new palette_impl;
    *palette = (LPDIRECTDRAWPALETTE)p_impl;
    unsigned char palettes[256 * 4];
    for(int i = 0; i < (256 * 4); i += 4)
    {
        palettes[i] = colorarray[i / 4].peRed;
        palettes[i + 1] = colorarray[i / 4].peGreen;
        palettes[i + 2] = colorarray[i / 4].peBlue;
        palettes[i + 3] = 0xff;
    }
    this->surface->create_palette(palettes, p_impl);

    return S_OK;
}

HRESULT WINAPI ddraw_impl::SetCooperativeLevel(HWND hwnd, DWORD flags)
{
    this->hwnd = hwnd;
    this->hdc = GetDC(this->hwnd);
    return S_OK;
}

HRESULT WINAPI ddraw_impl::SetDisplayMode(DWORD width, DWORD height, DWORD bpp)
{
    this->w = width;
    this->h = height;

    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DEPTH_DONTCARE,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0,
        0,
	    0,
	    0,
	    0, 0, 0, 0,
        0,
        0,
        0,
        0,
        0,
	    0, 0, 0
    };

    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    if(pixelFormat == 0)
    {
        MessageBox(this->hwnd, "ChoosePixelFormat failed", NULL, 0);
        exit(1);
    }
    if(SetPixelFormat(hdc, pixelFormat, &pfd) != TRUE)
    {
        MessageBox(this->hwnd, "SetPixelFormat failed", NULL, 0);
        exit(1);
    }

    int w = this->w, h = this->h;
    const DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
    const HWND topmost = windowed ? HWND_NOTOPMOST : HWND_TOPMOST;
    SetClassLongPtr(this->hwnd, GCL_STYLE, CS_HREDRAW | CS_VREDRAW | CS_OWNDC);
    if(windowed)
        SetWindowLongPtr(this->hwnd, GWL_STYLE, style);
    else
        // for not activating window accidentally
        SetWindowLongPtr(this->hwnd, GWL_EXSTYLE, WS_EX_NOACTIVATE | WS_EX_APPWINDOW);
    if(windowed)
    {
        RECT rect;
        rect.left = 0;
        rect.right = this->w;
        rect.top = 0;
        rect.bottom = this->h;
        AdjustWindowRect(&rect, style, FALSE);
        w = rect.right - rect.left;
        h = rect.bottom - rect.top;
    }
    if(change_resolution)
    {
        if(!set_fullscreen(w, h, 32, false, false))
        {
            MessageBox(this->hwnd, "Incompatible resolution", NULL, 0);
            exit(1);
        }
    }
    SetWindowPos(this->hwnd, topmost, 0, 0, w, h, SWP_SHOWWINDOW | SWP_NOMOVE);
    if(windowed)
        ShowCursor(TRUE);

    HGLRC hgl = wglCreateContext(this->hdc);
    wglMakeCurrent(this->hdc, hgl);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (GLdouble)this->w, (GLdouble)this->h, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
    glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
    glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
    glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");
    glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
    glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB");
    glMapBufferARB = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
    glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");
    glCreateShader = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShader");
    glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB");
    glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
    glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
    glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
    glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
    if(glGenBuffersARB == NULL || glCreateShader == NULL || glShaderSourceARB == NULL || glCreateProgram == NULL)
    {
        MessageBox(this->hwnd, "Video card doesn't support extension", NULL, 0);
        exit(1);
    }
    if(wglSwapIntervalEXT != NULL)
        wglSwapIntervalEXT(0);

    old_proc = (WNDPROC)GetWindowLongPtr(this->hwnd, GWLP_WNDPROC);
    SetWindowLongPtr(this->hwnd, GWLP_WNDPROC, (LPARAM)WindowProc);
    return S_OK;
}

ULONG WINAPI ddraw_impl::Release()
{
    delete this;
    return 0;
}

BOOL WINAPI verify_version_info(LPOSVERSIONINFOEXW, DWORD, DWORDLONG)
{
    return FALSE;
}

HRESULT WINAPI DirectDrawCreate(GUID* guid, LPDIRECTDRAW* obj, void*)
{
    windowed = (bool)PathFileExists("ddraw_windowed.txt");
    pre_render = (bool)PathFileExists("ddraw_pre_render.txt");
    change_resolution = (bool)PathFileExists("ddraw_change_resolution.txt") && !windowed;
    ddraw_impl* impl_ddraw = new ddraw_impl;
    *obj = (LPDIRECTDRAW)impl_ddraw;

    // patch IsWindows7OrGreater function to fix obs
    if(PathFileExists("ddraw_patch_obs.txt"))
    {
        void* func_address = (void*)GetProcAddress(GetModuleHandle("kernel32.dll"), "VerifyVersionInfoW");
        DWORD old_protect;
        VirtualProtect(func_address, 5, PAGE_EXECUTE_READWRITE, &old_protect);
        DWORD offset = (DWORD)verify_version_info - (DWORD)func_address - 5;
        const unsigned char jmp = 0xE9;
        unsigned char* code = (unsigned char*)func_address;
        *code = jmp;
        *((DWORD*)(code + 1)) = offset;
        VirtualProtect(func_address, 5, old_protect, &old_protect);
    }
    // patch level list size to fit array
    if(PathFileExists("ddraw_patch_elma.txt"))
    {
        void* ptr = (char*)GetModuleHandle(NULL) + 0xA14C0;
        DWORD old_protect;
        VirtualProtect(ptr, 4, PAGE_READWRITE, &old_protect);
        if((*(int*)ptr) > 27)
            *(int*)ptr = 27;
        VirtualProtect(ptr, 4, old_protect, &old_protect);
    }

    return S_OK;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            /*
        case WM_SETFOCUS:
            if(!windowed)
            {
                std::ofstream("ddraw_debug.txt", std::ios::app) << "a" << std::endl;
                if(change_resolution)
                    set_fullscreen(0, 0, 0, false, true);
                //ShowWindow(hWnd, SW_SHOWNOACTIVATE);
                return 0;//DefWindowProc(hWnd, message, wParam, lParam);
            }
            else
                return DefWindowProc(hWnd, message, wParam, lParam);
        case WM_KILLFOCUS:
            if(!windowed)
            {
                std::ofstream("ddraw_debug.txt", std::ios::app) << "b" << std::endl;
                if(change_resolution)
                    set_fullscreen(0, 0, 0, true, false);
                //ShowWindow(hWnd, SW_MINIMIZE);
                return 0;//DefWindowProc(hWnd, message, wParam, lParam);
            }
            else
                return DefWindowProc(hWnd, message, wParam, lParam);
            */
        case WM_ACTIVATEAPP:
            if(!windowed)
            {
                if(wParam == TRUE)
                {
                    if(change_resolution)
                        set_fullscreen(0, 0, 0, false, true);
                    ShowWindow(hWnd, SW_SHOWNORMAL);
                }
                else
                {
                    if(change_resolution)
                        set_fullscreen(0, 0, 0, true, false);
                    ShowWindow(hWnd, SW_MINIMIZE);
                }
            }
            return CallWindowProc(old_proc, hWnd, message, wParam, lParam);
            /*
        case WM_ACTIVATE:
            if(!windowed)
            {
                if(LOWORD(wParam) != 0)
                {
                    std::ofstream("ddraw_debug.txt", std::ios::app) << "a" << std::endl;
                    if(change_resolution)
                        set_fullscreen(0, 0, 0, false, true);
                    return 0;//DefWindowProc(hWnd, message, wParam, lParam);
                }
                else
                {
                    std::ofstream("ddraw_debug.txt", std::ios::app) << "b" << std::endl;
                    if(change_resolution)
                        set_fullscreen(0, 0, 0, true, false);
                    //ShowWindow(hWnd, SW_MINIMIZE);
                    return DefWindowProc(hWnd, message, wParam, lParam);
                }
            }
            else
                return DefWindowProc(hWnd, message, wParam, lParam);
                */
        case WM_CLOSE:
            return DefWindowProc(hWnd, message, wParam, lParam);
        
    }

    return CallWindowProc(old_proc, hWnd, message, wParam, lParam);
}

bool set_fullscreen(DWORD width, DWORD height, DWORD bpp, bool desktop, bool old)
{
    // query desktop video settings
    static DEVMODE desktop_mode, settings;

    if(desktop)
    {
        ChangeDisplaySettings(&desktop_mode, CDS_FULLSCREEN);
        return true;
    }
    if(old)
    {
        ChangeDisplaySettings(&settings, CDS_FULLSCREEN);
        return true;
    }

    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &desktop_mode);

    settings.dmSize = sizeof(DEVMODE);
    settings.dmBitsPerPel = bpp;
    settings.dmPelsWidth = width;
    settings.dmPelsHeight = height;
    settings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
    // make sure to use monitor frequency that works in fullscreen
    if(width <= (int)desktop_mode.dmPelsWidth && height <= (int)desktop_mode.dmPelsHeight) 
    {
        settings.dmDisplayFrequency = desktop_mode.dmDisplayFrequency;
        settings.dmFields |= DM_DISPLAYFREQUENCY;
    }
    bool changed = (ChangeDisplaySettings(&settings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL);
    if(!changed && (settings.dmFields & DM_DISPLAYFREQUENCY)) 
    {
       settings.dmFields &= ~DM_DISPLAYFREQUENCY;
       changed = (ChangeDisplaySettings(&settings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL);
    }

    return changed;
}