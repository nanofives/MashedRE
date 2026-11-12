// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0052e0e0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef long double float10;
typedef int code();
#ifndef __cplusplus
#define true 1
#endif
#ifndef __cplusplus
#define false 0
#endif
MGLOBAL(float, DAT_005d757c, 0x005d757c);
typedef struct H0052 { int w0; int n4; int w8; float len; int off10; int w14; } H0052;
typedef struct K0052 { int w0; float t; } K0052;
typedef struct A0052 A0052;
typedef int (*CB0052)(A0052 *, int);
typedef void (*FN0052)(int *, int, int, float, int);
struct A0052 {
  H0052 *hdr;
  float t;
  uint next;
  CB0052 cb3;
  int a4;
  float f5;
  CB0052 cb6;
  int a7;
  int w8;
  int stride;
  int step;
  int cnt;
  int pad[5];
  FN0052 fn11;
  int pad12;
  int data[1];
};
float FUN_0052de60(A0052 *p0, H0052 *p1);
// MATCH: 0x0052e0e0 FUN_0052e0e0
undefined4 FUN_0052e0e0(A0052 *param_1,float param_2)

{
  int *puVar1;
  float fVar2;
  float fVar3;
  int iVar4;
  int iVar5;
  int bVar6;
  int bVar7;
  int iVar8;
  H0052 *iVar9;
  int iVar10;
  uint uVar11;
  int iVar12;
  int *piVar13;
  int iVar9b;

  if (param_2 > DAT_005d757c) {
    fVar2 = param_1->f5;
    iVar9 = param_1->hdr;
    fVar3 = iVar9->len;
    iVar12 = param_1->cnt;
    bVar7 = 0;
    if ((fVar2 <= param_1->t) || (param_2 + param_1->t < fVar2)) {
      bVar6 = 0;
    }
    else {
      bVar6 = 1;
    }
    param_2 = param_2 + param_1->t;
    param_1->t = param_2;
    if (fVar3 < param_2) {
      bVar7 = 1;
      do {
        param_2 = param_2 - fVar3;
        bVar6 = bVar6 | (fVar2 <= param_2);
      } while (fVar3 < param_2);
      param_1->t = param_2;
      param_1->t = FUN_0052de60(param_1,iVar9);
    }
    if (param_1->next == 0) {
      param_1->t = FUN_0052de60(param_1,iVar9);
    }
    iVar4 = iVar9->n4;
    iVar9b = iVar9->off10;
    iVar5 = param_1->step;
    uVar11 = param_1->next;
    iVar10 = 0;
    while ((uVar11 < (uint)(iVar4 * iVar5 + iVar9b) &&
           (((K0052 *)param_1->next)->t <= param_1->t))) {
      do {
        iVar8 = param_1->stride * iVar10;
        iVar10 = iVar10 + 1;
        puVar1 = (int *)(iVar8 + 0x4c + (int)param_1);
        if (iVar12 <= iVar10) {
          iVar10 = 0;
        }
      } while (puVar1[1] != *(int *)param_1->next);
      *puVar1 = puVar1[1];
      puVar1[1] = param_1->next;
      uVar11 = param_1->next + param_1->step;
      param_1->next = uVar11;
    }
    piVar13 = param_1->data;
    if (0 < iVar12) {
      do {
        param_1->fn11(piVar13,*piVar13,piVar13[1],param_1->t,iVar9->w14);
        piVar13 = (int *)((int)piVar13 + param_1->stride);
        iVar12 = iVar12 + -1;
      } while (iVar12 != 0);
    }
    if (((bVar6) && (param_1->cb3 != 0)) &&
       (iVar4 = param_1->cb3(param_1,param_1->a4), iVar4 == 0)) {
      param_1->cb3 = 0;
    }
    if ((bVar7) && (param_1->cb6 != 0)) {
      iVar4 = param_1->cb6(param_1,param_1->a7);
      if (iVar4 == 0) {
        param_1->cb6 = 0;
        return 1;
      }
    }
  }
  return 1;
}
