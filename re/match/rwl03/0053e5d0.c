// FLAGS: /O2 /Ob1 /G5
// h3: 59.4% regs 67.2%. Local float10 temps for the three float diffs and two terms, unsigned width/height. Original folds the unsigned convert into fidiv dword + a zero hi store, not reproduced; class (b)/(a).
/* MATCH-RWL03 0x0053e5d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef struct { uint w; uint h; float u; float v; } P53e5d0;
// MATCH: 0x0053e5d0 FUN_0053e5d0
float10 FUN_0053e5d0(int param_1,P53e5d0 *param_2,int param_3)
{
  float *pfVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  float10 d, a, b, t1, t2;


  pfVar1 = (float *)(param_1 + 0x20 + param_3);
  d = (float10)*(float *)(param_1 + 0x14 + param_3) - (float10)*pfVar1;
  a = (float10)param_2->u - (float10)*pfVar1;
  b = (float10)*(float *)(param_1 + 0x14 + param_3) - (float10)param_2->v;
  iVar2 = -1;
  for (uVar4 = param_2->w; uVar4 != 0; uVar4 = uVar4 >> 1) {
    iVar2 = iVar2 + 1;
  }
  t1 = (float10)(iVar2 + 2) - (float10)(1 << (iVar2 + 1)) / (float10)param_2->w;
  iVar3 = -1;
  for (uVar4 = param_2->h; uVar4 != 0; uVar4 = uVar4 >> 1) {
    iVar3 = iVar3 + 1;
  }
  t2 = (float10)(iVar3 + 2) - (float10)(1 << (iVar3 + 1)) / (float10)param_2->h;
  return (t1 * a + t2 * b) / d;
}
