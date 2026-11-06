// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00458880: 100% by hand (raw 94.1%: the raw pointer loop gives a different compare).
   Index loop over the 25 x 0x50-byte record array at 0x0068b198 (same records as 0x004587a0). */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct Icon00458880 {
  undefined4 clump;
  undefined4 f4;
  float f8;
  float fc;
  float f10;
  float f14;
  undefined4 rest[14];
} Icon00458880;
MGLOBALA(Icon00458880, DAT_0068b198, 25, 0x0068b198);
MGLOBALA(undefined1, DAT_0068b968, 8, 0x0068b968);
MGLOBAL(undefined4, DAT_0068b9ac, 0x0068b9ac);
int FUN_004e6e00(undefined4 p0);
void FUN_004768c0(void *p0);
void FUN_004c5930(undefined4 p0);
// MATCH: 0x00458880 FUN_00458880
void FUN_00458880(void)
{
  int i;

  for (i = 0; i < 25; i++) {
    FUN_004e6e00(DAT_0068b198[i].clump);
  }
  FUN_004768c0(DAT_0068b968);
  FUN_004c5930(DAT_0068b9ac);
}
