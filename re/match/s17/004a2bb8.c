// FLAGS: /O1
/* MATCH-S17 0x004a2bb8: CRT secchk.c report_failure: __try { __security_error_handler(_SECERR_BUFFER_OVERRUN=1,
   NULL) FUN_004a583a } __except(EXCEPTION_EXECUTE_HANDLER) {} ExitProcess(3). Scope table 0x005d1158.
   Matched: whole obj symbol BYTE-IDENTICAL over 48 B (relocations masked). match.py prints 88.2% only because the
   Ghidra size (41 B) stops before push 3 / call ExitProcess. ExitProcess must be declared noreturn (no epilog).
   relocheck reports BAD on the scope table $T (it compares the table bytes raw, its label relocations unresolved:
   tool limit, same line on every __try file of this slice). */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
__declspec(dllimport) __declspec(noreturn) void __stdcall ExitProcess(unsigned int uExitCode);
void FUN_004a583a(int code, void *data);
// MATCH: 0x004a2bb8 FUN_004a2bb8
void FUN_004a2bb8(void)
{
    __try {
        FUN_004a583a(1, 0);
    }
    __except (1) {
    }
    ExitProcess(3);
}
