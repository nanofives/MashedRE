// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00442c80: by hand (h7). Raw 80.0%.
   MATCH-S08-FU: the .rdata float _DAT_005cc9b8 (7.0) is written as the literal 7.0f, because the exe's copy of the
   original .rdata reads 0. The compiler emits its own __real constant, so the code bytes and match.py are unchanged. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBALA(float, DAT_008989b0, 4, 0x008989b0);
int FUN_0040e350(void);
// MATCH: 0x00442c80 FUN_00442c80
int FUN_00442c80(int param_1)
{
  if (FUN_0040e350() == 6 && DAT_007f0fd0 != 4 && DAT_007f0fd0 != 9 && DAT_007f0fd0 != 8 &&
      DAT_008989b0[param_1] > 7.0f) {
    return 1;
  }
  return 0;
}
