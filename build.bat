@echo off
rem Native Windows build. Counterpart to build.sh (which builds on Linux/Wine);
rem keep the two in sync. Uses clang (i686 msvc target, no CRT), same as build.sh.
rem   build.bat            debug link with lld-link
rem   build.bat crinkler   final link with Crinkler (native)
rem
rem Native re-enables two flags that build.sh drops for Wine: /UNSAFEIMPORT (the
rem DefWindowProcA forwarded-RVA crash is Wine-only) and /ORDERTRIES (crashes
rem Crinkler under Wine, safe and ~10 B smaller on real Windows).
setlocal
cd /d "%~dp0"

set SDK=sdk
set INC=-isystem %SDK%\sdk\include\um -isystem %SDK%\sdk\include\shared -isystem %SDK%\sdk\include\ucrt -isystem %SDK%\crt\include
set LIBPATH=%SDK%\sdk\lib\um\x86
set LIBS=kernel32.lib user32.lib gdi32.lib opengl32.lib winmm.lib
if not exist build mkdir build

echo [*] compiling intro.c (clang, i686 msvc target, no CRT)
clang --target=i686-pc-windows-msvc -Oz -fno-stack-protector ^
      -fno-builtin-memset -fno-builtin-memcpy -msse2 -mfpmath=sse ^
      %INC% -c src\intro.c -o build\intro.obj
if errorlevel 1 exit /b 1

if "%~1"=="crinkler" (
  echo [*] linking with Crinkler ^(native^)
  tools\crinkler30a\Win32\Crinkler.exe ^
      /SUBSYSTEM:WINDOWS /ENTRY:entry /NODEFAULTLIB ^
      /RANGE:opengl32 /COMPMODE:SLOW /TINYHEADER /TINYIMPORT /UNSAFEIMPORT /ORDERTRIES:4000 ^
      /OUT:build\intro.exe build\intro.obj %LIBS% /LIBPATH:%LIBPATH%
) else (
  echo [*] linking with lld-link ^(debug^)
  lld-link /SUBSYSTEM:WINDOWS /ENTRY:entry /NODEFAULTLIB ^
      /OUT:build\intro.exe build\intro.obj %LIBS% /LIBPATH:%LIBPATH%
)
if errorlevel 1 exit /b 1
dir build\intro.exe
endlocal
