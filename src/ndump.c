/* ndump.c - native hardware-GL frame dump (not shipped). Renders the exact
   shared shader/render path at a chosen size+time on the real GPU and writes a
   PPM, so we can verify what the demo renders without emulation.
   Usage: ndump [W H [t]]   default 1920x1080 t=3.0 */
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include "render.h"

int main(int argc, char **argv) {
    int W = argc > 2 ? atoi(argv[1]) : 1920;
    int H = argc > 2 ? atoi(argv[2]) : 1080;
    float t = argc > 3 ? (float)atof(argv[3]) : 3.0f;

    glfwInit();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);      /* headless, no window shown */
    GLFWwindow *win = glfwCreateWindow(W, H, "ndump", 0, 0);
    glfwMakeContextCurrent(win);
    fprintf(stderr, "GL_VENDOR  : %s\n", (const char *)glGetString(0x1F00));
    fprintf(stderr, "GL_RENDERER: %s\n", (const char *)glGetString(0x1F01));
    LOAD_GL(glfwGetProcAddress);

    g_w = W; g_h = H;
    glViewport(0, 0, W, H);
    render_init();
    render_frame(t);
    glFinish();

    unsigned char *b = malloc((size_t)W * H * 3);
    glReadPixels(0, 0, W, H, GL_RGB, GL_UNSIGNED_BYTE, b);
    FILE *f = fopen("build/ndump.ppm", "wb");
    fprintf(f, "P6\n%d %d\n255\n", W, H);
    for (int y = H - 1; y >= 0; y--) fwrite(b + (size_t)y * W * 3, 1, W * 3, f);
    fclose(f);
    return 0;
}
