// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00429a30 (h9, by hand): 100% (regs 100%).
   Raw converter 80%: 0x0067d99c and the 0x007f0e1c table are floats (fld/fstp copy). Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0067d98c, 0x0067d98c);
MGLOBAL(int, DAT_0067d994, 0x0067d994);
MGLOBAL(float, DAT_0067d99c, 0x0067d99c);
MGLOBALA(int, DAT_007f0db4, 13, 0x007f0db4);
MGLOBALA(int, DAT_007f0de8, 13, 0x007f0de8);
MGLOBALA(float, DAT_007f0e1c, 13, 0x007f0e1c);
int FUN_00430790(void);
// MATCH: 0x00429a30 FUN_00429a30
/* [C1 2026-05-15] No parameters. */
void FUN_00429a30(void)
{
  DAT_007f0db4[FUN_00430790()] = DAT_0067d98c;
  DAT_007f0de8[FUN_00430790()] = DAT_0067d994;
  DAT_007f0e1c[FUN_00430790()] = DAT_0067d99c;
}
