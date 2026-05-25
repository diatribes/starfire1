# starfire1

A 1.3 KB Windows intro: a fullscreen raymarched sun over a deep brown-noise
rumble, packed with Crinkler. No CRT. Its color, size, and the rumble all pulse
together on a ~12 s cycle, with a fade in and out.

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

Run `build/intro.exe` on Windows, or under Wine with a GL-capable prefix. For a
movable dev window, compile `src/intro.c` with `-DWINDOWED` and link with lld-link.

## Gotchas

- Only the Linux/Wine build (`build.sh`) exists; there's no native Windows
  `build.bat` yet.
- `build.sh` omits Crinkler's `/UNSAFEIMPORT`: under Wine it crashes the packer
  (the `DefWindowProcA` forwarded RVA). Safe to re-add on a native Windows build.
- Under Wine, an EGL-based build needs `__EGL_VENDOR_LIBRARY_FILENAMES` pointed at
  your GPU's OpenGL ICD, or the window renders black.

`starfire1.glsl` is the readable shader source; the demo embeds a minified copy
in `src/shader.h`.
