// FLAGS: /O2 /Ob1 /G5
/* best 51.6% (regs 22.3% renamed, ops ~73%), 179 vs 186 insns: struct V typed from offsets (score at +0x58, list links +0/+4, sentinel
   &DAT_007dca18), FastSqrt-table path through a float/uint/ushort union, permuter swap/commute applied. Residual: x87 operand order and
   fcomp/fnstsw flag-test forms, register use; not tuned further for budget. Class (a)/(b): x87 compare+order residuals. */
/* PROV-FIX-9 2026-11-12 (verify/provfix9_20261112/RESULT_PROV-FIX-9.md, diagnosis verify/provcrash_20261112/RESULT_PROV-CRASH-12.md): three statement orders follow the original: exponent read after u.f = fVar11 (0x005a7655..0x005a7664); node->prev takes the old prev first (0x005a777b..0x005a7788); old tail linked before DAT_007dca1c moves (0x005a77e6..0x005a77ee). Best now 43.5%. */
/* MATCH-S25 0x005a75b0: hand-matched (h1). */
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

typedef struct V {
  struct V *next; struct V *prev; int p2; uint flags; int p4; int p5; float vol; int p7; int p8; int p9;
  byte *obj; int p11; float x0; float x1; float x2; int p15; int p16; int p17; float far_; float near_;
  float a; float b; float score;
} V;
typedef union { float f; uint u; ushort s[2]; } FU;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cc574, 0x005cc574);
MGLOBAL(float, DAT_007dca44, 0x007dca44);
MGLOBAL(float, DAT_007dca48, 0x007dca48);
MGLOBAL(float, DAT_007dca4c, 0x007dca4c);
MGLOBAL(float, DAT_00632f08, 0x00632f08);
MGLOBALA(short, DAT_00633b48, 256, 0x00633b48);
MGLOBAL(V *, DAT_007dca18, 0x007dca18);
MGLOBAL(V *, DAT_007dca1c, 0x007dca1c);
// MATCH: 0x005a75b0 FUN_005a75b0
void FUN_005a75b0(V *param_1)
{
  V *piVar12;
  float fVar3;
  V *piVar4;
  FU u;
  float fPrev;
  float fVar11;
  V *piVar5;
  V *piVar9;
  float fVar2;
  float fVar7;
  uint uVar10;
  piVar9 = param_1;
  param_1->score = 0;
  if (!(param_1->vol > DAT_005d757c)) goto LAB_005a7705;
  fVar7 = DAT_005cc320;
  if ((param_1->obj[0x54] & 2) != 0) {
    fVar3 = param_1->far_;
    fVar2 = param_1->near_;
    fVar11 = (DAT_007dca4c - param_1->x2) * (DAT_007dca4c - param_1->x2) +
             (DAT_007dca44 - param_1->x0) * (DAT_007dca44 - param_1->x0) +
             (DAT_007dca48 - param_1->x1) * (DAT_007dca48 - param_1->x1);
    if (fVar2 * fVar2 <= fVar11) goto LAB_005a7705;
    if (fVar11 != DAT_005d757c) {
      u.f = fVar11;
      uVar10 = (u.u >> 0x17) - 0x7f;
      u.u = u.u & 0x7fffff;
      if ((uVar10 & 1) != 0) {
        u.u = u.u | 0x800000;
      }
      u.u = ((int)DAT_00633b48[u.s[1]] << 0x10) | ((short)((short)uVar10 >> 1) + 0x7f) * 0x800000;
      fVar11 = u.f;
    }
    if ((fVar11 >= fVar3) &&
       (fVar7 = (((fVar2 - fVar3) - (fVar11 - fVar3)) /
                (DAT_00632f08 * (fVar11 - fVar3) + (fVar2 - fVar3))) * param_1->vol,
       fVar7 <= DAT_005d757c)) goto LAB_005a7705;
  }
  param_1->score = param_1->a * param_1->b + fVar7 * (DAT_005cc320 - param_1->b);
LAB_005a7705:
  if (0 != (param_1->flags & 1)) {
    piVar4 = param_1;
    do {
      piVar12 = piVar4;
      piVar4 = piVar12->next;
      piVar5 = piVar12->prev;
      fVar7 = DAT_005cc574;
      if (piVar4 != (V *)&DAT_007dca18) {
        fVar7 = piVar4->score;
      }
      if (piVar5 == (V *)&DAT_007dca18) {
        fPrev = -1.0f;
      }
      else {
        fPrev = piVar5->score;
      }
    } while ((fVar7 < param_1->score) || (piVar4 = piVar5, param_1->score < fPrev));
    if (piVar12 != piVar9) {
      *(V **)piVar9->prev = piVar9->next;
      piVar9->next->prev = piVar9->prev;
LAB_005a777b:
      piVar9->next = piVar12;
      piVar9->prev = piVar12->prev;
      *(V **)piVar12->prev = piVar9;
      piVar12->prev = piVar9;
    }
    piVar9->flags = piVar9->flags | 1;
    return;
  }
  *(V **)param_1->prev = param_1->next;
  param_1->next->prev = param_1->prev;
  {
    V *puVar8 = DAT_007dca1c;
    piVar12 = DAT_007dca18;
    if (piVar12 != (V *)&DAT_007dca18) {
      do {
        if (param_1->score < piVar12->score) {
          if (piVar12 != (V *)&DAT_007dca18) goto LAB_005a777b;
          break;
        }
        piVar12 = piVar12->next;
      } while (piVar12 != (V *)&DAT_007dca18);
    }
    param_1->prev = puVar8;
    param_1->next = (V *)&DAT_007dca18;
    *(V **)DAT_007dca1c = param_1;
    DAT_007dca1c = param_1;
    param_1->flags = param_1->flags | 1;
  }
}
