# starfire1

A 1.3 KB exe with raymarched sun and brown-noise audio.
Packed with Crinkler. No CRT. Color and size sync'd with noise.
Fades in and out (audio and gfx)

Mostly just tinkering. Seeing what I can get setup for cross-platform shader dev.
Tiny binaries, some basic audio, stuff like that.

![starfire1](starfire1.gif)

## Build & run

Needs **clang + lld-link** (LLVM) and **wine** (to run Crinkler from Linux).
Two dependencies aren't in the repo. Place them first:

- **Crinkler 3.0a** → `tools/crinkler30a/Win32/Crinkler.exe`
- **MSVC SDK** → `sdk/`, generated with [xwin](https://github.com/Jake-Shadle/xwin):
  `xwin --accept-license splat --output sdk`

```sh
./build.sh crinkler     # -> build/intro.exe  (~1304 bytes)
```

On Windows, `build.bat crinkler` is the native equivalent (same clang, native
Crinkler with `/ORDERTRIES` + `/UNSAFEIMPORT`). It hasn't been tested yet; see Gotchas.

Run `build/intro.exe` on Windows, or under Wine with a GL-capable prefix. For a
movable dev window, compile `src/intro.c` with `-DWINDOWED` and link with lld-link.

## Gotchas

- `build.sh` (Linux/Wine) is the validated build path. `build.bat` is the native
  Windows counterpart but hasn't been tested on real Windows yet.
- `build.sh` omits Crinkler's `/UNSAFEIMPORT`: under Wine it crashes the packer
  (the `DefWindowProcA` forwarded RVA). Safe to re-add on a native Windows build.
- Under Wine, an EGL-based build needs `__EGL_VENDOR_LIBRARY_FILENAMES` pointed at
  your GPU's OpenGL ICD, or the window renders black.

`starfire1.glsl` is the readable shader source; the demo embeds a minified copy
in `src/shader.h`.
