// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004c7860 (h1): 100% (regs 100%). Call through RwGlobals+0x84 with (&param_2, param_1,
   (param_2 & 0xff) * 0x100 + param_3); returns param_2 when the call returns nonzero, else 0.
   The raw form `-(uint)(r != 0) & param_2` gave setne; the ternary gives neg/sbb/and. FLAGS /G5 (RW core). */
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
typedef int (*RwStdFn3)(void *, int, int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);

// MATCH: 0x004c7860 FUN_004c7860
unsigned int FUN_004c7860(int param_1, unsigned int param_2, int param_3)
{
  return (*(RwStdFn3 *)(DAT_007d3ff8 + 0x84))(&param_2, param_1, (param_2 & 0xff) * 0x100 + param_3) ? param_2 : 0;
}
