// FLAGS: /O1
/* MATCH-S17 0x004a2be9: CRT __security_check_cookie (secchk.c), __fastcall, cookie in ECX compared with
   __security_cookie DAT_00616038, failure tail-jumps to report_failure FUN_004a2bb8.
   Best 50% (regs 50%). Residual class (c): the VC7 CRT writes this function as __declspec(naked) inline __asm
   (cmp ecx,cookie / jne failure / ret / failure: jmp report_failure, so every register is preserved). From C the
   compiler always inverts the branch (je over a tail jmp, ret last). Tried: if/!=, early return, explicit returns,
   /O1 /O2 /Og /Os /Oy: all 50%. */
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
MGLOBAL(unsigned int, DAT_00616038, 0x00616038);
void FUN_004a2bb8(void);
// MATCH: 0x004a2be9 FUN_004a2be9
void __fastcall FUN_004a2be9(unsigned int cookie)
{
    if (cookie != DAT_00616038)
        FUN_004a2bb8();
}
