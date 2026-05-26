#!/usr/bin/env bash
# Linux build host -> Windows PE. Builds on Linux; the result runs on Windows.
#   ./build.sh            debug link with lld-link (readable errors, Gate 3)
#   ./build.sh crinkler   final link with Crinkler under Wine (Gate 4)
# Native Windows build lives in build.bat and must stay in sync with this.
set -euo pipefail
cd "$(dirname "$0")"

SDK=sdk
INC="-isystem $SDK/sdk/include/um -isystem $SDK/sdk/include/shared -isystem $SDK/sdk/include/ucrt -isystem $SDK/crt/include"
LIBPATH="$SDK/sdk/lib/um/x86"
LIBS="kernel32.lib user32.lib gdi32.lib opengl32.lib winmm.lib"
mkdir -p build

echo "[*] compiling intro.c (clang, i686 msvc target, no CRT)"
clang --target=i686-pc-windows-msvc -Oz -fno-stack-protector \
      -fno-builtin-memset -fno-builtin-memcpy -msse2 -mfpmath=sse \
      $INC -c src/intro.c -o build/intro.obj

if [ "${1:-}" = "crinkler" ]; then
  echo "[*] linking with Crinkler (Wine)"
  # Drop any prior output first: a wedged Crinkler from an earlier crash can keep
  # build/intro.exe open and make the next run fail with "Cannot open ... for
  # writing". Unlinking the path lets this run create a fresh inode regardless.
  rm -f build/intro.exe
  # The shipped build uses a stub window proc and imports no DefWindowProcA, whose
  # Wine forwarded RVA used to crash Crinkler. With it gone, builds are
  # deterministic and /ORDERTRIES is usable under Wine (it re-links many times, so
  # it used to amplify that crash); it shaves a few bytes. /UNSAFEIMPORT is left off
  # because it gives no size benefit here.
  wine tools/crinkler30a/Win32/Crinkler.exe \
      /SUBSYSTEM:WINDOWS /ENTRY:entry /NODEFAULTLIB \
      /RANGE:opengl32 /COMPMODE:SLOW /TINYHEADER /TINYIMPORT /ORDERTRIES:4000 \
      /OUT:build/intro.exe build/intro.obj $LIBS /LIBPATH:$LIBPATH
else
  echo "[*] linking with lld-link (debug)"
  lld-link /SUBSYSTEM:WINDOWS /ENTRY:entry /NODEFAULTLIB \
      /OUT:build/intro.exe build/intro.obj $LIBS /LIBPATH:$LIBPATH
fi

ls -l build/intro.exe
