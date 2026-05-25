/* wgltest.c - headless verification of the raw-WGL render path. Not shipped.
   Creates a hidden window (never shown -> cannot take over the screen), makes
   a legacy WGL context, renders one frame at a fixed time through the shared
   render.h, reads it back with glReadPixels, and writes build/wgltest.ppm.
   This is the faithful test of the demo's actual WGL + glCreateShaderProgramv
   + glRecti path (GLFW exercises the same render.h but its own context). */
#include <windows.h>
#include <gl/gl.h>
#include "render.h"

#define W 640      /* 640*3 = 1920, divisible by 4 -> no glReadPixels row pad */
#define H 360
#ifndef GL_RGB
#define GL_RGB 0x1907
#endif

int _fltused = 0;
void *memset(void *d, int c, size_t n) {
    unsigned char *p = (unsigned char *)d; while (n--) *p++ = (unsigned char)c; return d;
}

void entry(void) {
    PIXELFORMATDESCRIPTOR pfd;
    HWND hwnd; HDC dc; unsigned char *buf; HANDLE f; DWORD wr; int y;
    static const char hdr[] = "P6\n640 360\n255\n";

    /* hidden: no WS_VISIBLE, never ShowWindow */
    hwnd = CreateWindowExA(0, "static", 0, WS_POPUP, 0, 0, W, H, 0, 0, 0, 0);
    dc   = GetDC(hwnd);
    memset(&pfd, 0, sizeof pfd);
    pfd.nSize = sizeof pfd; pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA; pfd.cColorBits = 32;
    SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd);
    wglMakeCurrent(dc, wglCreateContext(dc));

    g_w = W; g_h = H;
    glViewport(0, 0, W, H);
    LOAD_GL(wglGetProcAddress);
    render_init();
    render_frame(3.0f);          /* fixed timestamp for a deterministic frame */
    glFinish();

    buf = (unsigned char *)VirtualAlloc(0, W * H * 3, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    glReadPixels(0, 0, W, H, GL_RGB, GL_UNSIGNED_BYTE, buf);

    f = CreateFileA("build\\wgltest.ppm", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    WriteFile(f, hdr, sizeof hdr - 1, &wr, 0);
    for (y = H - 1; y >= 0; y--)         /* flip: GL origin is bottom-left */
        WriteFile(f, buf + y * W * 3, W * 3, &wr, 0);
    CloseHandle(f);
    ExitProcess(0);
}
