// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d8b70 (h3): 100% (regs 100%), nosize.py: sym_len 90, ends 0x004d8bca on 90 padding.
   Raw form with RwGlobals pointer first and real arities (FUN_004d8560 takes 1 arg, slot 0x10c takes 1). */
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
typedef void (*Fn10c)(int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ad8, 0x00911ad8);
MGLOBAL(int, DAT_00911adc, 0x00911adc);
int FUN_004d8560(int param_1);
void FUN_004d8eb0(void);
// MATCH: 0x004d8b70 FUN_004d8b70
int FUN_004d8b70(int param_1)
{
  FUN_004d8eb0();
  if (*(int *)(DAT_007d3ff8 + DAT_00911ad8 + 0xc) != 0) {
    FUN_004d8560(*(int *)(DAT_007d3ff8 + DAT_00911ad8 + 0xc));
    (*(Fn10c *)(DAT_007d3ff8 + 0x10c))(*(int *)(DAT_007d3ff8 + DAT_00911ad8 + 0xc));
    *(int *)(DAT_007d3ff8 + DAT_00911ad8 + 0xc) = 0;
  }
  DAT_00911adc--;
  return param_1;
}
