/* Gate 1: minimal no-CRT 32-bit Win32 program.
   Declares its one import by hand to avoid needing SDK headers.
   Exits with code 42 so we can verify it actually ran under Wine. */
__declspec(dllimport) void __stdcall ExitProcess(unsigned int uExitCode);

void entry(void) {
    ExitProcess(42);
}
