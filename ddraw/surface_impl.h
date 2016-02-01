#pragma once

#include "ddraw_impl.h"

class surface_impl
{
private:
    const int buffersize;
    const bool no_pre_rendering;
    palette_impl* ui_palette;
    palette_impl* current_palette;
    GLint texture_sampler, colortable_sampler;
    GLuint id, shader_id, program_id;
    int w, h;
    HDC hdc;
    GLuint pbo_id[2];
    int index, next_index;
    bool pre_render, skip_frame;
public:
    surface_impl(int w, int h, HDC hdc, bool no_pre_rendering = true);

    void setup_render();
    void render();
    void create_palette(unsigned char palette[256 * 4], palette_impl*);

    virtual HRESULT WINAPI QueryInterface(const IID& a, LPVOID* b) {return E_NOTIMPL;}
    virtual ULONG WINAPI AddRef() {return 0;}
    virtual ULONG WINAPI Release();

    virtual HRESULT WINAPI AddAttachedSurface(LPDIRECTDRAWSURFACE a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI AddOverlayDirtyRect(LPRECT a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI Blt(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDBLTFX e) {return E_NOTIMPL;}
    virtual HRESULT WINAPI BltBatch(LPDDBLTBATCH a, DWORD b, DWORD c) {return E_NOTIMPL;}
    virtual HRESULT WINAPI BltFast(DWORD a, DWORD b, LPDIRECTDRAWSURFACE c, LPRECT d, DWORD e) {return E_NOTIMPL;}
    virtual HRESULT WINAPI DeleteAttachedSurface(DWORD a, LPDIRECTDRAWSURFACE b) {return E_NOTIMPL;}
    virtual HRESULT WINAPI EnumAttachedSurfaces(LPVOID a, LPDDENUMSURFACESCALLBACK b) {return E_NOTIMPL;}
    virtual HRESULT WINAPI EnumOverlayZOrders(DWORD a, LPVOID b, LPDDENUMSURFACESCALLBACK c) {return E_NOTIMPL;}
    virtual HRESULT WINAPI Flip(LPDIRECTDRAWSURFACE a, DWORD b);
    virtual HRESULT WINAPI GetAttachedSurface(LPDDSCAPS a, LPDIRECTDRAWSURFACE* b);
    virtual HRESULT WINAPI GetBltStatus(DWORD a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI GetCaps(LPDDSCAPS a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI GetClipper(LPDIRECTDRAWCLIPPER* a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI GetColorKey(DWORD a, LPDDCOLORKEY b) {return E_NOTIMPL;}
    virtual HRESULT WINAPI GetDC(HDC* a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI GetFlipStatus(DWORD a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI GetOverlayPosition(LPLONG a, LPLONG b) {return E_NOTIMPL;}
    virtual HRESULT WINAPI GetPalette(LPDIRECTDRAWPALETTE* a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI GetPixelFormat(LPDDPIXELFORMAT a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI GetSurfaceDesc(LPDDSURFACEDESC a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI Initialize(LPDIRECTDRAW a, LPDDSURFACEDESC b) {return E_NOTIMPL;}
    virtual HRESULT WINAPI IsLost() {return E_NOTIMPL;}
    virtual HRESULT WINAPI Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d);
    virtual HRESULT WINAPI ReleaseDC(HDC a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI Restore() {return S_OK;}
    virtual HRESULT WINAPI SetClipper(LPDIRECTDRAWCLIPPER a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI SetColorKey(DWORD a, LPDDCOLORKEY b) {return E_NOTIMPL;}
    virtual HRESULT WINAPI SetOverlayPosition(LONG a, LONG b) {return E_NOTIMPL;}
    virtual HRESULT WINAPI SetPalette(LPDIRECTDRAWPALETTE a);
    virtual HRESULT WINAPI Unlock(LPVOID a);
    virtual HRESULT WINAPI UpdateOverlay(LPRECT a, LPDIRECTDRAWSURFACE b, LPRECT c, DWORD d, LPDDOVERLAYFX e) {return E_NOTIMPL;}
    virtual HRESULT WINAPI UpdateOverlayDisplay(DWORD a) {return E_NOTIMPL;}
    virtual HRESULT WINAPI UpdateOverlayZOrder(DWORD a, LPDIRECTDRAWSURFACE b) {return E_NOTIMPL;}
};