// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041d910: 100% (regs 100%) by hand from the raw converter (25%). What moved it: DAT_0063d588
   typed float (stored as 0.0f) and DAT_0063d584 int. With both as undefined4 the compiler shares one
   xor eax,eax for the two stores; the original stores two immediates. Residual: none. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef unsigned int undefined4;
MGLOBAL(int, DAT_0063d584, 0x0063d584);
MGLOBAL(float, DAT_0063d588, 0x0063d588);
// MATCH: 0x0041d910 FUN_0041d910
void FUN_0041d910(void)

{
  DAT_0063d584 = 0;
  DAT_0063d588 = 0.0f;
  return;
}
