// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h1 0x0056ac40: 4.1% (regs 7.4%), class c. SSE xmm body (cmpltps/movmskps/andnps), rewritten as scalar 4-lane loops with extended 15-param signature; bytes cannot match under DDK cl. Tried: scalar lane form only. cxx:ok. */
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
/* scalar lane-by-lane form of the SSE body (cmpltps/andps/orps/movmskps/andnps/addps become 4-lane loops);
   the original uses xmm registers, which the DDK cl cannot emit */
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBALA(uint, DAT_005e5a70, 4, 0x005e5a70);
MGLOBALA(float, DAT_005e5a40, 4, 0x005e5a40);
// MATCH: 0x0056ac40 FUN_0056ac40
float10 FUN_0056ac40(int *param_1,uint *param_2,float *param_3,int param_4,
                     int param_5,int param_6,int param_7,int param_8,int param_9,int param_10,
                     int param_11,uint *param_12,int param_13,int param_14,float param_15)
{
  uint *U;
  uint *V;
  uint *M;
  float lim[4];
  float acc[4];
  float sel[4];
  float fVar9;
  float best;
  float tmp;
  int k;
  int n;
  int iVar5;
  uint uVar7;
  uint uVar6;

  best = param_15;
  for (k = 0; k < 4; k++) {
    lim[k] = param_15;
    acc[k] = 0.0f;
    sel[k] = 0.0f;
  }
  if (param_4 != 0) {
    U = (uint *)param_3;
    V = (uint *)param_9;
    M = param_12;
    for (n = param_4; n != 0; n--) {
      uint c[4];
      int any = 0;
      for (k = 0; k < 4; k++) {
        uint u = U[k];
        uint t1 = DAT_005e5a70[k] ^ (u & M[k]);
        uint t2 = u & V[k];
        float f1 = *(float *)&t1;
        float f2 = *(float *)&t2;
        c[k] = ((lim[k] < f1) || (lim[k] < f2)) ? 0xffffffffu : 0u;
        if (c[k] != 0) any = 1;
      }
      if (any) {
        for (k = 0; k < 4; k++) {
          uint u = U[k];
          if (c[k] != 0) {
            sel[k] = acc[k];
            *(uint *)&lim[k] = ~DAT_005e5a70[k] & u;
          }
        }
      }
      for (k = 0; k < 4; k++) {
        acc[k] = acc[k] + DAT_005e5a40[k];
      }
      U += 4;
      V += 4;
      M += 4;
    }
  }
  iVar5 = -1;
  uVar7 = 0xffffffff;
  fVar9 = DAT_005d757c;
  for (n = param_4, tmp = 0.0f; n != 0; n--) {
    for (k = 0; k < 4; k++) {
      float x = param_3[k];
      fVar9 = x * x + fVar9;
    }
    param_3 += 4;
  }
  for (uVar6 = 0; uVar6 < 4; uVar6++) {
    if (best < lim[uVar6]) {
      best = lim[uVar6];
      uVar7 = uVar6;
    }
  }
  if (uVar7 != 0xffffffff) {
    iVar5 = (int)(sel[uVar7] + (sel[uVar7] < 0.0f ? -0.5f : 0.5f));
  }
  *param_1 = iVar5;
  *param_2 = uVar7;
  return fVar9;
}
