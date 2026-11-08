// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h3 0x004e5fc0: hand-written (28-byte Key struct array, V4f copies) + permuter. 77.3%. Residual (a): x87 operand order/scheduling of the three products (original fld field; fmul st(1) vs ours fld st(0); fmul mem) and goto label epilogue order. */
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
typedef struct V4f { float x, y, z, w; } V4f;
typedef struct Key { int pad; V4f v; int pad2[2]; } Key;
// MATCH: 0x004e5fc0 FUN_004e5fc0
void FUN_004e5fc0(int param_1)
{
  int iVar1;
  short sVar3;
  short sVar4;
  int iVar5;
  Key *pKeys;
  Key *k0;
  Key *k1;
  float fVar7;
  float fVar8;
  float fVar9;
  int iVar10;

  iVar5 = *(int *)(param_1 + 0x18);
  if (iVar5 == 0) {
    return;
  }
  sVar3 = *(short *)(param_1 + 0x52);
  sVar4 = *(short *)(param_1 + 0x50);
  if (sVar4 != sVar3) {
    if (((int)sVar4 < *(int *)(iVar5 + 0x18)) &&
       (iVar10 = (int)sVar3, iVar10 < *(int *)(iVar5 + 0x18))) {
      pKeys = *(Key **)(iVar5 + 0x5c);
      fVar7 = *(float *)(param_1 + 0x5c) * *(float *)(param_1 + 0x58);
      k0 = pKeys + sVar4;
      k1 = pKeys + iVar10;
      *(float *)(param_1 + 0x28) = (k1->v.w - k0->v.w) * fVar7 + k0->v.w;
      *(float *)(param_1 + 0x1c) = k1->v.x - k0->v.x;
      *(float *)(param_1 + 0x20) = k1->v.y - k0->v.y;
      *(float *)(param_1 + 0x24) = k1->v.z - k0->v.z;
      fVar8 = fVar7 * *(float *)(param_1 + 0x1c);
      *(float *)(param_1 + 0x1c) = fVar8;
      fVar9 = fVar7 * *(float *)(param_1 + 0x20);
      *(float *)(param_1 + 0x20) = fVar9;
      fVar7 = fVar7 * *(float *)(param_1 + 0x24);
      *(float *)(param_1 + 0x24) = fVar7;
      *(float *)(param_1 + 0x1c) = k0->v.x + fVar8;
      *(float *)(param_1 + 0x20) = k0->v.y + fVar9;
      *(float *)(param_1 + 0x24) = k0->v.z + fVar7;
      goto LAB_004e60e6;
    }
  }
  if (((int)sVar4 < *(int *)(iVar5 + 0x18)) && ((int)sVar3 < *(int *)(iVar5 + 0x18))) {
    *(V4f *)(param_1 + 0x1c) = (*(Key **)(iVar5 + 0x5c) + sVar4)->v;
  }
  else {
    *(V4f *)(param_1 + 0x1c) = (*(Key **)(iVar5 + 0x5c))->v;
  }
LAB_004e60e6:
  *(unsigned int *)(param_1 + 0x4c) = *(unsigned int *)0xfffffffd & (param_1 + 0x4c);
  *(unsigned char *)(param_1 + 3) = *(unsigned char *)(param_1 + 3) | 1;
}


