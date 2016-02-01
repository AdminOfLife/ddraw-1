#include "surface_impl.h"
#include "static.h"

//#include <Windows.h>
//#include <fstream>

//std::ofstream profile("ddraw_profile.txt");
//LARGE_INTEGER now, freq;
//LARGE_INTEGER now_all, now_all2;

surface_impl::surface_impl(int w, int h, HDC hdc, bool no_pre_rendering) : 
    w(w), h(h), hdc(hdc), buffersize(w * h), current_palette(NULL),
    skip_frame(false), no_pre_rendering(no_pre_rendering)
{
    this->setup_render();

    //QueryPerformanceFrequency(&freq);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // causes distortion on some settings without this
    //glPixelStorei(GL_PACK_ALIGNMENT, 1);

    this->shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    int len = (int)strlen(shaderprogram);
    glShaderSourceARB(this->shader_id, 1, (const GLcharARB**)&shaderprogram, &len);
    glCompileShaderARB(this->shader_id);
    this->program_id = glCreateProgram();
    glAttachShader(this->program_id, this->shader_id);
    glLinkProgram(this->program_id);
    glUseProgram(this->program_id);
    this->texture_sampler = glGetUniformLocation(this->program_id, "texture");
    this->colortable_sampler = glGetUniformLocation(this->program_id, "colortable");
    glUniform1i(this->texture_sampler, 0);
    glUniform1i(this->colortable_sampler, 1);

    glGenBuffersARB(1, this->pbo_id);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, this->pbo_id[this->index]);
    glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, this->buffersize, NULL, GL_STREAM_DRAW_ARB);
    if(!this->no_pre_rendering)
    {
        glGenBuffersARB(1, this->pbo_id + 1);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, this->pbo_id[this->next_index]);
        glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, this->buffersize, NULL, GL_STREAM_DRAW_ARB);
    }
    // initialize buffer data to 0
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, this->pbo_id[this->index]);
    unsigned char* buffer = (unsigned char*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
    memset(buffer, 0x00, this->buffersize);
    glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    // gen tex
    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    unsigned char* initial_buffer = new unsigned char[this->buffersize];
    memset(initial_buffer, 0xff, this->buffersize);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_LUMINANCE,
        w,
        h,
        0,
        GL_LUMINANCE,
        GL_UNSIGNED_BYTE,
        initial_buffer);
    delete [] initial_buffer;
}

void surface_impl::setup_render()
{
    if(this->no_pre_rendering)
    {
        this->pre_render = false;
        this->next_index = this->index = 0;
    }
    else
    {
        this->pre_render = true;
        this->index = 0;
        this->next_index = 1;
    }
}

void surface_impl::create_palette(unsigned char palette[256 * 4], palette_impl* p_impl)
{
    static int i = 0;
    if(i <= 1)
    {
        this->ui_palette = p_impl;
        i++;
    }

    glGenTextures(1, &p_impl->id);
    glBindTexture(GL_TEXTURE_2D, p_impl->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        256,
        1,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        palette);
}

void surface_impl::render()
{
    glBindTexture(GL_TEXTURE_2D, this->id);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, this->pbo_id[this->index]);
    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0,
        0,
        this->w,
        this->h,
        GL_LUMINANCE,
        GL_UNSIGNED_BYTE,
        0);
    
    //glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0.f, 0.f); glVertex2f(0.f, 0.f);
    glTexCoord2f(0.f, 1.f); glVertex2f(0.f, (GLfloat)this->h);
    glTexCoord2f(1.f, 1.f); glVertex2f((GLfloat)this->w, (GLfloat)this->h);
    glTexCoord2f(1.f, 0.f); glVertex2f((GLfloat)this->w, 0.f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

HRESULT WINAPI surface_impl::Lock(LPRECT a, LPDDSURFACEDESC b, DWORD c, HANDLE d)
{
    //QueryPerformanceCounter(&now);
    if(this->pre_render)
        this->render();

    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, this->pbo_id[this->next_index]);
    if(this->pre_render)
    {
        // map the buffer object into client's memory
        // Note that glMapBufferARB() causes sync issue.
        // If GPU is working with this buffer, glMapBufferARB() will wait(stall)
        // for GPU to finish its job. To avoid waiting (stall), you can call
        // first glBufferDataARB() with NULL pointer before glMapBufferARB().
        // If you do that, the previous data in PBO will be discarded and
        // glMapBufferARB() returns a new allocated pointer immediately
        // even if GPU is still working with the previous data.
        glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, this->buffersize, 0, GL_STREAM_DRAW_ARB);
        b->lpSurface = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
    }
    else
    {
        if(this->current_palette != this->ui_palette)
        {
            glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, this->buffersize, 0, GL_STREAM_DRAW_ARB);
            b->lpSurface = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
        }
        else
            b->lpSurface = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_WRITE_ARB);
    }
    b->lPitch = this->w;
    b->dwWidth = this->w;
    b->dwHeight = this->h;

    return S_OK;
}

HRESULT WINAPI surface_impl::Unlock(LPVOID a)
{
    //LARGE_INTEGER now2;
    //QueryPerformanceCounter(&now2);
    //profile << "lock: " << (now2.QuadPart - now.QuadPart) * 1000000 / freq.QuadPart << "us\n";
    //QueryPerformanceCounter(&now);

    glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);

    if(!this->pre_render)
        this->render();

    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    if(this->skip_frame)
        this->skip_frame = false;
    else
        SwapBuffers(this->hdc);

    if(this->pre_render)
    {
        this->index = (this->index + 1) % 2;
        this->next_index = (this->next_index + 1) % 2;
    }

    //QueryPerformanceCounter(&now2);
    //profile << "render: " << (now2.QuadPart - now.QuadPart) * 1000000 / freq.QuadPart << "us\n";

    //QueryPerformanceCounter(&now_all2);
    //profile << "frame: " << (now_all2.QuadPart - now_all.QuadPart) * 1000000 / freq.QuadPart << "us\n";
    //QueryPerformanceCounter(&now_all);

    return S_OK;
}

HRESULT WINAPI surface_impl::Flip(LPDIRECTDRAWSURFACE a, DWORD b)
{
    return S_OK;
}

HRESULT WINAPI surface_impl::SetPalette(LPDIRECTDRAWPALETTE a)
{
    if((palette_impl*)a == this->ui_palette)
    {
        this->index = this->next_index;
        this->pre_render = false;
    }
    else
    {
        this->setup_render();
        if(!this->no_pre_rendering)
            this->skip_frame = true;
    }

    this->current_palette = (palette_impl*)a;
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->current_palette->id);
    glActiveTexture(GL_TEXTURE0);
    return S_OK;
}

HRESULT WINAPI surface_impl::GetAttachedSurface(LPDDSCAPS a, LPDIRECTDRAWSURFACE* b)
{
    *b = (LPDIRECTDRAWSURFACE)this;
    return S_OK;
}

ULONG WINAPI surface_impl::Release()
{
    delete this;
    return 0;
}