#pragma once

#include <ddraw.h>
#include <gl/GL.h>
#include "glext.h"

#pragma comment(lib, "opengl32")

extern PFNGLGENBUFFERSARBPROC glGenBuffersARB;
extern PFNGLBINDBUFFERARBPROC glBindBufferARB;
extern PFNGLBUFFERDATAARBPROC glBufferDataARB;
extern PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB;
extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
extern PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB;
extern PFNGLMAPBUFFERARBPROC glMapBufferARB;
extern PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;
extern PFNGLCREATESHADEROBJECTARBPROC glCreateShader;
extern PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
extern PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
extern PFNGLUNIFORM1IPROC glUniform1i;

typedef BOOL (WINAPI* PFNWGLSWAPINTERVALEXTPROC)(int interval);
extern PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

extern WNDPROC old_proc;
extern bool windowed, pre_render, change_resolution;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool set_fullscreen(DWORD width, DWORD height, DWORD bpp, bool desktop, bool old);

class surface_impl;

class ddraw_impl
{
protected:
    surface_impl* surface;
    HWND hwnd;
    HDC hdc;
    int w, h;
public:
    ddraw_impl();

    virtual HRESULT WINAPI QueryInterface(const IID&, LPVOID*) {return E_NOTIMPL;}
    virtual ULONG WINAPI AddRef() {return 0;}
    virtual ULONG WINAPI Release();

    virtual HRESULT WINAPI Compact() {return E_NOTIMPL;}
    virtual HRESULT WINAPI CreateClipper(DWORD a, LPDIRECTDRAWCLIPPER* b, IUnknown* c) {return E_NOTIMPL;}
    virtual HRESULT WINAPI CreatePalette(DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE*, IUnknown*);
    virtual HRESULT WINAPI CreateSurface(LPDDSURFACEDESC, LPDIRECTDRAWSURFACE*, IUnknown*);
    virtual HRESULT WINAPI DuplicateSurface(LPDIRECTDRAWSURFACE a, LPDIRECTDRAWSURFACE* b) {return E_NOTIMPL;}
    virtual HRESULT WINAPI EnumDisplayModes(DWORD a, LPDDSURFACEDESC b, LPVOID c, LPDDENUMMODESCALLBACK d) {return E_NOTIMPL;}
    virtual HRESULT WINAPI EnumSurfaces(DWORD a, LPDDSURFACEDESC b, LPVOID c, LPDDENUMSURFACESCALLBACK d) {return E_NOTIMPL;}
    virtual HRESULT WINAPI FlipToGDISurface() {return E_NOTIMPL;}
    virtual HRESULT WINAPI GetCaps(LPDDCAPS a, LPDDCAPS b) {return E_NOTIMPL;}
    virtual HRESULT WINAPI GetDisplayMode(LPDDSURFACEDESC a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI GetFourCCCodes(LPDWORD a, LPDWORD b) {return E_NOTIMPL;}
    virtual HRESULT WINAPI GetGDISurface(LPDIRECTDRAWSURFACE* a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI GetMonitorFrequency(LPDWORD a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI GetScanLine(LPDWORD a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI GetVerticalBlankStatus(LPBOOL a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI Initialize(GUID* a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI RestoreDisplayMode() {return E_NOTIMPL;}
    virtual HRESULT WINAPI SetCooperativeLevel(HWND, DWORD);
    virtual HRESULT WINAPI SetDisplayMode(DWORD, DWORD, DWORD);
    virtual HRESULT WINAPI WaitForVerticalBlank(DWORD a, HANDLE b) {return E_NOTIMPL;}
};

class palette_impl
{
public:
    GLuint id;

    virtual HRESULT WINAPI QueryInterface(const IID&, LPVOID*) {return E_NOTIMPL;}
    virtual ULONG WINAPI AddRef() {return 0;}
    virtual ULONG WINAPI Release() {delete this; return 0;}

    virtual HRESULT WINAPI GetCaps(LPDWORD) {return E_NOTIMPL;}
    virtual HRESULT WINAPI GetEntries(DWORD, DWORD, DWORD, LPPALETTEENTRY) {return E_NOTIMPL;}
    virtual HRESULT WINAPI Initialize(LPDIRECTDRAW, DWORD, LPPALETTEENTRY) {return E_NOTIMPL;}
    virtual HRESULT WINAPI SetEntries(DWORD, DWORD, DWORD, LPPALETTEENTRY) {return E_NOTIMPL;}
};

HRESULT WINAPI DirectDrawCreate(GUID*, LPDIRECTDRAW*, void*);