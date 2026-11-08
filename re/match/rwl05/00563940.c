// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00563940: hand-matched (h1). 56.4% (regs 65.4%, ops 83.3%), 151/156 ins. Edge/vertex pass with inline sqrt (#pragma intrinsic(sqrt), SQRT macro -> fsqrt), float globals typed float (were byte/int), '> 0' for jbe, compare inline. Remaining: ushort edge-index loads (original skips the xor because eax is known 16-bit), frame layout (sub esp 0xc), later vec-copy block. Class (b). */
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
#pragma intrinsic(sqrt)
double __cdecl sqrt(double);
#define SQRT(x) ((float)sqrt((double)(x)))
typedef unsigned char undefined1;
typedef unsigned int uint;
typedef unsigned short ushort;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
void FUN_00565c20(float * p0, float * p1, float * p2);
void FUN_005663a0(float * p0, float * p1);
// MATCH: 0x00563940 FUN_00563940
/* [C1 2026-05-19] Phase 1: normalizes edge directions. For each edge (count `*(short*)(param_1 +
   0x20)`): */

void FUN_00563940(int param_1)

{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  int iVar4;
  float *pfVar5;
  bool bVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  uint uVar10;
  ushort uVar11;
  
  uVar11 = 0;
  if (*(unsigned short *)(param_1 + 0x20) > 0) {
    do {
      iVar4 = *(int *)(param_1 + 4);
      pfVar1 = (float *)(*(int *)(param_1 + 0x14) + (uint)uVar11 * 0xc);
      uVar10 = (uint)*(ushort *)((int)pfVar1 + 6);
      pfVar2 = (float *)(iVar4 + (uint)*(ushort *)(pfVar1 + 1) * 0xc);
      fVar3 = *(float *)(iVar4 + 8 + (uint)*(ushort *)(pfVar1 + 1) * 0xc) -
              *(float *)(iVar4 + 8 + uVar10 * 0xc);
      fVar8 = pfVar2[1] - *(float *)(iVar4 + 4 + uVar10 * 0xc);
      fVar7 = *pfVar2 - *(float *)(iVar4 + uVar10 * 0xc);
      fVar3 = SQRT(fVar3 * fVar3 + fVar8 * fVar8 + fVar7 * fVar7);
      *pfVar1 = fVar3;
      if (fVar3 != DAT_005d757c) {
        *pfVar1 = DAT_005cc320 / fVar3;
      }
      uVar11 = uVar11 + 1;
    } while (uVar11 < *(ushort *)(param_1 + 0x20));
  }
  pfVar5 = *(float **)(param_1 + 4);
  pfVar1 = (float *)(param_1 + 0x34);
  pfVar2 = (float *)(param_1 + 0x24);
  *pfVar1 = *pfVar5;
  *(float *)(param_1 + 0x38) = pfVar5[1];
  uVar11 = 1;
  *(float *)(param_1 + 0x3c) = pfVar5[2];
  *pfVar2 = *pfVar5;
  *(float *)(param_1 + 0x28) = pfVar5[1];
  *(float *)(param_1 + 0x2c) = pfVar5[2];
  if (1 < *(ushort *)(param_1 + 0x1c)) {
    do {
      FUN_005663a0(pfVar2,(float *)(*(int *)(param_1 + 4) + (uint)uVar11 * 0xc));
      uVar11 = uVar11 + 1;
    } while (uVar11 < *(ushort *)(param_1 + 0x1c));
  }
  fVar9 = *pfVar2 - *pfVar1;
  fVar7 = *(float *)(param_1 + 0x28) - *(float *)(param_1 + 0x38);
  pfVar2 = (float *)(param_1 + 0x44);
  fVar8 = *(float *)(param_1 + 0x2c) - *(float *)(param_1 + 0x3c);
  *pfVar2 = DAT_005cc32c * fVar9 + *pfVar1;
  *(float *)(param_1 + 0x48) = fVar7 * DAT_005cc32c + *(float *)(param_1 + 0x38);
  *(float *)(param_1 + 0x4c) = fVar8 * DAT_005cc32c + *(float *)(param_1 + 0x3c);
  fVar3 = fVar8;
  if (fVar8 <= fVar7) {
    fVar3 = fVar7;
  }
  if ((fVar9 < fVar3) && (fVar9 = fVar8, fVar8 <= fVar7)) {
    fVar9 = fVar7;
  }
  uVar11 = 0;
  *(float *)(param_1 + 0x50) = fVar9 * DAT_005cc32c;
  if (*(short *)(param_1 + 0x1c) != 0) {
    do {
      FUN_00565c20(pfVar2,pfVar2,(float *)(*(int *)(param_1 + 4) + (uint)uVar11 * 0xc));
      uVar11 = uVar11 + 1;
    } while (uVar11 < *(ushort *)(param_1 + 0x1c));
  }
  *(uint *)(param_1 + 0x88) = *(uint *)(param_1 + 0x88) | 1;
  return;
}
