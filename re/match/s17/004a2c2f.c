// FLAGS: /O1
/* MATCH-S17 0x004a2c2f: CRT fpinit.c _fpmath: _cfltcvt_init FUN_004a2bf7, _adjust_fdiv DAT_00773994 =
   _ms_p5_mp_test_fdiv FUN_004a5e35, _setdefaultprecision FUN_004a5de3, then fnclex.
   Best 50% (regs 50%). Residual class (c): the CRT source ends with an inline __asm fnclex. No C construct or cl 13
   intrinsic emits fnclex, so the C form tail-jumps to FUN_004a5de3 instead of call / fnclex / ret. */
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
MGLOBAL(int, DAT_00773994, 0x00773994);
void FUN_004a2bf7(void);
int FUN_004a5e35(void);
void FUN_004a5de3(void);
// MATCH: 0x004a2c2f FUN_004a2c2f
void FUN_004a2c2f(void)
{
    FUN_004a2bf7();
    DAT_00773994 = FUN_004a5e35();
    FUN_004a5de3();
}
