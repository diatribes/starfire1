/* preview.c - throwaway native Linux preview. Not shipped, no size budget.
   Uses GLFW to get a compatibility GL context, then renders the exact same
   FRAG_SRC / render_frame() as the Windows demo so what you see here is what
   the packed PE produces. Build: see build_preview.sh. ESC quits. */
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "render.h"

int main(void) {
    if (!glfwInit()) return 1;
    /* No profile/version hints => legacy/compatibility context (needed for
       glRecti + gl_FragColor), matching the WGL wglCreateContext path. */
    GLFWwindow *win = glfwCreateWindow(1280, 720, "starfire1 preview", 0, 0);
    if (!win) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    LOAD_GL(glfwGetProcAddress);
    render_init();

    while (!glfwWindowShouldClose(win)) {
        glfwGetFramebufferSize(win, &g_w, &g_h);
        glViewport(0, 0, g_w, g_h);
        render_frame((float)glfwGetTime());
        glfwSwapBuffers(win);
        glfwPollEvents();
        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;
    }
    glfwTerminate();
    return 0;
}
