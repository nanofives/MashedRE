// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042d3a0 (h5, by hand): 100.0% (regs 100.0%). 13 x 0x40-byte records at 0x0067ed78 cleared field by field, the 12-dword middle via memset (intrinsic rep stosd). Indexed loop (a pointer loop gives a down-counter and a +8 anchor). Residual: none. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
typedef struct Rec0067ed78 {
  int f0;
  int f4;
  int f8;
  int f0c[12];
  int f3c;
} Rec0067ed78;
MGLOBALA(Rec0067ed78, DAT_0067ed78, 13, 0x0067ed78);
void *memset(void *p0, int p1, unsigned int p2);
#pragma intrinsic(memset)
// MATCH: 0x0042d3a0 FUN_0042d3a0
void FUN_0042d3a0(void)
{
  int i;

  for (i = 0; i < 13; i++) {
    DAT_0067ed78[i].f0 = 0;
    DAT_0067ed78[i].f4 = 0;
    DAT_0067ed78[i].f8 = 0;
    memset(DAT_0067ed78[i].f0c, 0, sizeof(DAT_0067ed78[i].f0c));
    DAT_0067ed78[i].f3c = 0;
  }
}
