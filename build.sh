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
  # Note: /UNSAFEIMPORT is dropped here. Under Wine, DefWindowProcA imports via a
  # forwarded RVA (-> ntdll), and /UNSAFEIMPORT + that forward crashes Crinkler
  # ("Oops! Crinkler has crashed", 0-byte output) for the current code layout.
  # The crash is layout-sensitive (it built with /UNSAFEIMPORT at 1322 B when more
  # code was present) and Wine-specific (no forwarded RVA on real Windows). Safe to
  # re-add on a native build.bat, or re-test here if the code layout changes.
  wine tools/crinkler30a/Win32/Crinkler.exe \
      /SUBSYSTEM:WINDOWS /ENTRY:entry /NODEFAULTLIB \
      /RANGE:opengl32 /COMPMODE:SLOW /TINYHEADER /TINYIMPORT \
      /OUT:build/intro.exe build/intro.obj $LIBS /LIBPATH:$LIBPATH
else
  echo "[*] linking with lld-link (debug)"
  lld-link /SUBSYSTEM:WINDOWS /ENTRY:entry /NODEFAULTLIB \
      /OUT:build/intro.exe build/intro.obj $LIBS /LIBPATH:$LIBPATH
fi

ls -l build/intro.exe
