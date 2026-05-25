/* render.h - shared GL setup/draw for both the Windows demo and the Linux
   preview. The caller includes its platform GL headers first, then includes
   this, then calls LOAD_GL(<getproc>) once a context is current.

   The fullscreen draw is the sizecoding trick: a compatibility context lets
   glRecti(-1,-1,1,1) cover clip space with zero vertex data and no vertex
   shader, so only the fragment program runs. glCreateShaderProgramv compiles
   the whole thing in a single entry point. */
#include "shader.h"

#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif

/* GL entry points are __stdcall on 32-bit Windows, so these typedefs need
   APIENTRY. Without it the stack drifts a few bytes per call. */
#ifndef APIENTRY
#define APIENTRY
#endif

/* Only the two GL 2.0+ entry points need wglGetProcAddress now. Time and
   resolution are smuggled in through the legacy fixed-function primary colour
   (glColor4f, a free GL 1.1 import) and read back in the shader as gl_Color.
   That removes glGetUniformLocation/glUniform1f/glUniform3f (three long
   proc-name strings) and the uniform name strings entirely. */
typedef GLuint (APIENTRY *PFN_CSPV)(GLenum, GLsizei, const char *const *);
typedef void   (APIENTRY *PFN_USE)(GLuint);

static PFN_CSPV p_createprog;
static PFN_USE  p_use;

#define LOAD_GL(GET) do {                                          \
    p_createprog = (PFN_CSPV)GET("glCreateShaderProgramv");        \
    p_use        = (PFN_USE) GET("glUseProgram");                  \
} while (0)

static GLuint g_prog;
static int    g_w, g_h;            /* current framebuffer size */
static float  g_fade = 1;          /* 0..1 master brightness; demo ramps it, preview tools leave it 1 */

static void render_init(void) {
    g_prog = p_createprog(GL_FRAGMENT_SHADER, 1, &FRAG_SRC);
    p_use(g_prog);
}

static void render_frame(float t) {
    /* glColor clamps to [0,1], so pack pre-scaled (time/256, w/8192, h/8192) --
       powers of two so the shader's *256 / *8192 rescale is exact in float.
       Holds for tracks < 256 s and resolutions <= 8192 px. The 4th component is
       g_fade (master brightness), read in the shader as gl_Color.w. */
    glColor4f(t * (1.0f / 256), g_w * (1.0f / 8192), g_h * (1.0f / 8192), g_fade);
    glRecti(-1, -1, 1, 1);
}
