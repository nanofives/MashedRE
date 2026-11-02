// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00429aa0 (h10, by hand): 100% exact. Raw converter 60.7%: lost the (int) float
   conversions (FUN_004a2c48 = _ftol, emitted by the cast) and the 12-byte table at 0x00614718.
   Fixed: table typed as struct of 3 floats, (int) casts, float copies for 0x0067d9a0.
   Array counts (13) are the 0x34 spacing between 0x007f0db4/0x007f0de8/0x007f0e1c; the 0x00614718
   count is not read from the binary (does not affect bytes). */
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
typedef struct { float a; float b; float c; } S06_61_4718;
MGLOBAL(int, DAT_0067d990, 0x0067d990);
MGLOBAL(int, DAT_0067d998, 0x0067d998);
MGLOBAL(float, DAT_0067d9a0, 0x0067d9a0);
MGLOBALA(S06_61_4718, DAT_00614718, 13, 0x00614718);
MGLOBALA(int, DAT_007f0db4, 13, 0x007f0db4);
MGLOBALA(int, DAT_007f0de8, 13, 0x007f0de8);
MGLOBALA(float, DAT_007f0e1c, 13, 0x007f0e1c);
int FUN_00430790(void);
int FUN_00430820(void);
// MATCH: 0x00429aa0 FUN_00429aa0
void FUN_00429aa0(void)
{
  if (FUN_00430820() != 0) {
    DAT_0067d990 = (int)DAT_00614718[FUN_00430790()].a;
    DAT_0067d998 = (int)DAT_00614718[FUN_00430790()].b;
    DAT_0067d9a0 = DAT_00614718[FUN_00430790()].c;
    return;
  }
  DAT_0067d990 = DAT_007f0db4[FUN_00430790()];
  DAT_0067d998 = DAT_007f0de8[FUN_00430790()];
  DAT_0067d9a0 = DAT_007f0e1c[FUN_00430790()];
}
