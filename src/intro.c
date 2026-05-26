#include <windows.h>
#include <mmsystem.h>
#include <gl/gl.h>
#include "render.h"

int _fltused = 0;

void *memset(void *d, int c, size_t n) {
    unsigned char *p = (unsigned char *)d;
    while (n--) *p++ = (unsigned char)c;
    return d;
}

#define SR   44100
#ifndef DUR
#define DUR  30
#endif
#define NS   (SR * DUR)
static short      *g_audio;
static HWAVEOUT    g_hwo;
static WAVEHDR     g_hdr;
static DWORD       g_t0;

static void render_audio(short *buf, int frames) {
    unsigned s = 14;
    int y1 = 0, y2 = 0, lp = 0, x, i, g, v, ph, tri;
    for (i = 0; i < frames; i++) {
        s = s * 1103515245u + 12345u;
        x = (int)(s >> 16) - 32768;
        y1 += (x  - y1) >> 7;
        y2 += (y1 - y2) >> 8;
        lp += (y2 - lp) >> 11;
        v = y2 - lp;
        ph = (i & 0x7FFFF) - 0x40000;
        tri = ph < 0 ? -ph : ph;
        v -= (v * tri) >> 20;
        g = i < frames - i ? i : frames - i;
        if (g > (1 << 17)) g = 1 << 17;
        buf[i] = (short)((v * g >> 17) << 3);
    }
}

static void audio_start(void) {
    WAVEFORMATEX wf;
    memset(&wf, 0, sizeof wf);
    wf.wFormatTag      = WAVE_FORMAT_PCM;
    wf.nChannels       = 1;
    wf.nSamplesPerSec  = SR;
    wf.wBitsPerSample  = 16;
    wf.nBlockAlign     = 2;
    wf.nAvgBytesPerSec = SR * 2;

    g_audio = (short *)VirtualAlloc(0, (SIZE_T)NS * 2, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    render_audio(g_audio, NS);

    waveOutOpen(&g_hwo, WAVE_MAPPER, &wf, 0, 0, CALLBACK_NULL);
    memset(&g_hdr, 0, sizeof g_hdr);
    g_hdr.lpData         = (LPSTR)g_audio;
    g_hdr.dwBufferLength = (DWORD)NS * 2;
    waveOutPrepareHeader(g_hwo, &g_hdr, sizeof g_hdr);
    waveOutWrite(g_hwo, &g_hdr, sizeof g_hdr);
    g_t0 = GetTickCount();
}

static float audio_time(void) {
    return (float)(GetTickCount() - g_t0) * 0.001f;
}

#ifdef WINDOWED
static LRESULT CALLBACK wndproc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_SIZE) {
        int cw = LOWORD(l), ch = HIWORD(l);
        if (cw > 0 && ch > 0) { g_w = cw; g_h = ch; }
        return 0;
    }
    if (m == WM_CLOSE || m == WM_DESTROY ||
        (m == WM_KEYDOWN && w == VK_ESCAPE))
        ExitProcess(0);
    return DefWindowProcA(h, m, w, l);
}
#else
/* Shipped build: a stub class proc that returns 1. This avoids importing
   DefWindowProcA, whose Wine forwarded RVA (-> ntdll) crashes Crinkler. The
   borderless fullscreen popup needs no real message handling: WM_NCCREATE only
   needs TRUE to let the window create, GL repaints the whole client each frame,
   and Esc is polled in the loop. See crinkler.md. */
static LRESULT CALLBACK stubproc(HWND h, UINT m, WPARAM w, LPARAM l) {
    return 1;
}
#endif

void entry(void) {
    PIXELFORMATDESCRIPTOR pfd;
    WNDCLASSA wc;
    HWND hwnd;
    HDC  dc;

    memset(&wc, 0, sizeof wc);
    wc.style         = CS_OWNDC;
#ifdef WINDOWED
    wc.lpfnWndProc   = wndproc;
#else
    wc.lpfnWndProc   = stubproc;
#endif
    wc.hInstance     = GetModuleHandleA(0);
    wc.lpszClassName = "S";
    RegisterClassA(&wc);

#ifdef WINDOWED
    hwnd = CreateWindowExA(WS_EX_TOPMOST, "S", 0, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                           CW_USEDEFAULT, CW_USEDEFAULT, 960, 540, 0, 0, wc.hInstance, 0);
    SetForegroundWindow(hwnd);
#else
    hwnd = CreateWindowExA(WS_EX_TOPMOST, "S", 0, WS_POPUP | WS_VISIBLE,
                           0, 0, GetSystemMetrics(SM_CXSCREEN),
                           GetSystemMetrics(SM_CYSCREEN), 0, 0, wc.hInstance, 0);
    ShowCursor(0);
#endif
    dc = GetDC(hwnd);

    memset(&pfd, 0, sizeof pfd);
    pfd.nSize      = sizeof pfd;
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.cColorBits = 32;
    SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd);

    wglMakeCurrent(dc, wglCreateContext(dc));
    if (g_w <= 0) {
#ifdef WINDOWED
        g_w = 960; g_h = 540;
#else
        g_w = GetSystemMetrics(SM_CXSCREEN); g_h = GetSystemMetrics(SM_CYSCREEN);
#endif
    }
    LOAD_GL(wglGetProcAddress);
    render_init();
    audio_start();

    for (;;) {
        MSG   msg;
        float t;
        if (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) DispatchMessageA(&msg);
        if (GetAsyncKeyState(VK_ESCAPE)) ExitProcess(0);
        glViewport(0, 0, g_w, g_h);
        t = audio_time();
        if (t >= (float)DUR) break;
        g_fade = t < (float)DUR - t ? t : (float)DUR - t;
        if (g_fade > 2) g_fade = 2;
        g_fade *= .5f;
        render_frame(t);
        SwapBuffers(dc);
    }
    ExitProcess(0);
}
