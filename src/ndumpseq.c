/* ndumpseq.c - headless frame-sequence dump for making a preview gif (not shipped).
   Renders the shared shader/render path at N evenly-spaced times over [0,T) and
   writes build/seq/fNNN.ppm. One GL context for the whole run (fast).
   Usage: ndumpseq W H N T */
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include "render.h"

int main(int argc, char **argv) {
    int W = atoi(argv[1]), H = atoi(argv[2]), N = atoi(argv[3]);
    float T = (float)atof(argv[4]);

    glfwInit();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow *win = glfwCreateWindow(W, H, "seq", 0, 0);
    glfwMakeContextCurrent(win);
    LOAD_GL(glfwGetProcAddress);

    g_w = W; g_h = H;
    glViewport(0, 0, W, H);
    render_init();

    unsigned char *b = malloc((size_t)W * H * 3);
    for (int k = 0; k < N; k++) {
        render_frame(T * k / N);
        glFinish();
        glReadPixels(0, 0, W, H, GL_RGB, GL_UNSIGNED_BYTE, b);
        char fn[64];
        sprintf(fn, "build/seq/f%03d.ppm", k);
        FILE *f = fopen(fn, "wb");
        fprintf(f, "P6\n%d %d\n255\n", W, H);
        for (int y = H - 1; y >= 0; y--) fwrite(b + (size_t)y * W * 3, 1, W * 3, f);
        fclose(f);
    }
    return 0;
}
