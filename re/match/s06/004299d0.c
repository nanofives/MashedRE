// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x004299d0 (h9, by hand): 100% (regs 100%).
   Raw converter 87.5%: param_1, 0x0067d9a0 and the 0x007f0e1c table are floats (fld/fstp copy). Tables are
   13 entries each (0x34 B apart). Residual: none. */
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
MGLOBAL(float, DAT_0067d9a0, 0x0067d9a0);
MGLOBAL(int, DAT_0067d998, 0x0067d998);
MGLOBAL(int, DAT_0067d990, 0x0067d990);
MGLOBALA(int, DAT_007f0db4, 13, 0x007f0db4);
MGLOBALA(int, DAT_007f0de8, 13, 0x007f0de8);
MGLOBALA(float, DAT_007f0e1c, 13, 0x007f0e1c);
int FUN_00430790(void);
// MATCH: 0x004299d0 FUN_004299d0
void FUN_004299d0(float param_1, int param_2, int param_3)
{
  DAT_0067d9a0 = param_1;
  DAT_0067d998 = param_2;
  DAT_0067d990 = param_3;
  DAT_007f0db4[FUN_00430790()] = DAT_0067d990;
  DAT_007f0de8[FUN_00430790()] = DAT_0067d998;
  DAT_007f0e1c[FUN_00430790()] = DAT_0067d9a0;
}
