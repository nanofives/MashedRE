// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00426780 (h1, by hand): 100% / regs 100%, whole 133 B obj symbol BYTE-IDENTICAL.
   Moves: param_2 unsigned (fild + 2^32 fadd fixup), two indexed for loops (0x40 at +0x10110 gated by +0x10230,
   8 at +0x100f0 gated by +0x10210), callee FUN_00475010 takes (int, float). Residual: none. */
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
MGLOBAL(float, DAT_005cc94c, 0x005cc94c);
MGLOBAL(float, DAT_005cc948, 0x005cc948);
int FUN_00475010(int p0, float p1);
// MATCH: 0x00426780 FUN_00426780
void FUN_00426780(int param_1, unsigned int param_2)
{
  float f;
  int i;

  f = (float)param_2 * DAT_005cc948;
  for (i = 0; i < 0x40; i++) {
    if (*(int *)(param_1 + 0x10230 + i * 4) != 0 && *(int *)(param_1 + 0x10110 + i * 4) != 0) {
      FUN_00475010(*(int *)(param_1 + 0x10110 + i * 4), f);
    }
  }
  for (i = 0; i < 8; i++) {
    if (*(int *)(param_1 + 0x10210 + i * 4) != 0 && *(int *)(param_1 + 0x100f0 + i * 4) != 0) {
      FUN_00475010(*(int *)(param_1 + 0x100f0 + i * 4), f);
    }
  }
}
