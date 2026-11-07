// FLAGS: /O2 /GS /Ob1
// best 20.7 (regs 32.0, ops 46.0), raw Ghidra text unchanged (mixer inner loops over [ptd+0x..] fixed-point terms, 1063 B). Not reworked: over budget; needs loop-form recovery (unrolled by 4 loop + tail) from odis. Class (b).
/* MATCH-S28 0x005c98d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs28_20261107/results.tsv. */
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
typedef unsigned int uint;
// MATCH: 0x005c98d0 FUN_005c98d0
int FUN_005c98d0(int param_1)

{
  short sVar1;
  short sVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  int *piVar9;
  int iVar10;
  int *piVar11;
  short *psVar12;
  
  piVar3 = *(int **)(param_1 + 0x20);
  iVar8 = piVar3[0x1d];
  if (*piVar3 != 4) {
    return 0;
  }
  iVar4 = piVar3[0x11];
  piVar9 = *(int **)(iVar8 + 0x38);
  iVar5 = piVar3[0x12];
  piVar11 = *(int **)(iVar8 + 0x3c);
  uVar7 = *(uint *)(iVar8 + 0x28);
  iVar6 = piVar3[0x1f];
  psVar12 = (short *)piVar3[0x15];
  if (*(char *)(piVar3[0x1c] + 0x39) == '\x01') {
    for (; (iVar6 != 0 && (uVar7 != 0)); uVar7 = uVar7 - 1) {
      sVar1 = *psVar12;
      iVar6 = piVar3[0x21];
      iVar10 = piVar3[0x20] * (int)sVar1 >> 8;
      *piVar9 = *piVar9 + iVar10;
      iVar6 = iVar6 * sVar1 >> 8;
      psVar12 = psVar12 + 1;
      piVar9[1] = piVar9[1] + iVar6;
      piVar9 = piVar9 + 2;
      *piVar11 = *piVar11 + (piVar3[0x25] * iVar10 >> 0x10);
      piVar11[1] = piVar11[1] + (piVar3[0x26] * iVar6 >> 0x10);
      iVar6 = piVar3[0x1f] + -1;
      piVar3[0x21] = piVar3[0x21] + piVar3[0x23];
      piVar3[0x20] = piVar3[0x20] + piVar3[0x22];
      piVar3[0x1f] = iVar6;
      piVar3[0x25] = piVar3[0x25] + piVar3[0x27];
      piVar3[0x26] = piVar3[0x26] + piVar3[0x28];
      piVar3[0x24] = piVar3[0x24] + -1;
      piVar11 = piVar11 + 2;
    }
    for (uVar7 = uVar7 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
      iVar6 = *psVar12 * iVar5;
      iVar10 = *psVar12 * iVar4;
      *piVar9 = *piVar9 + iVar10;
      piVar9[1] = piVar9[1] + iVar6;
      *piVar11 = *piVar11 + (piVar3[0x13] * iVar10 >> 8);
      piVar11[1] = piVar11[1] + (piVar3[0x13] * iVar6 >> 8);
      iVar6 = psVar12[1] * iVar5;
      iVar10 = psVar12[1] * iVar4;
      piVar9[2] = piVar9[2] + iVar10;
      piVar9[3] = piVar9[3] + iVar6;
      piVar11[2] = piVar11[2] + (piVar3[0x13] * iVar10 >> 8);
      piVar11[3] = piVar11[3] + (piVar3[0x13] * iVar6 >> 8);
      iVar6 = psVar12[2] * iVar5;
      iVar10 = psVar12[2] * iVar4;
      piVar9[4] = piVar9[4] + iVar10;
      piVar9[5] = piVar9[5] + iVar6;
      piVar11[4] = piVar11[4] + (piVar3[0x13] * iVar10 >> 8);
      piVar11[5] = piVar11[5] + (piVar3[0x13] * iVar6 >> 8);
      iVar6 = psVar12[3] * iVar5;
      iVar10 = psVar12[3] * iVar4;
      piVar9[6] = piVar9[6] + iVar10;
      piVar9[7] = piVar9[7] + iVar6;
      piVar9 = piVar9 + 8;
      piVar11[6] = piVar11[6] + (piVar3[0x13] * iVar10 >> 8);
      piVar11[7] = piVar11[7] + (piVar3[0x13] * iVar6 >> 8);
      piVar11 = piVar11 + 8;
      psVar12 = psVar12 + 4;
    }
    for (uVar7 = *(uint *)(iVar8 + 0x28) & 3; uVar7 != 0; uVar7 = uVar7 - 1) {
      iVar8 = *psVar12 * iVar5;
      iVar6 = *psVar12 * iVar4;
      *piVar9 = *piVar9 + iVar6;
      psVar12 = psVar12 + 1;
      piVar9[1] = piVar9[1] + iVar8;
      piVar9 = piVar9 + 2;
      *piVar11 = *piVar11 + (piVar3[0x13] * iVar6 >> 8);
      piVar11[1] = piVar11[1] + (piVar3[0x13] * iVar8 >> 8);
      piVar11 = piVar11 + 2;
    }
  }
  else {
    while (iVar6 != 0) {
      if (uVar7 == 0) goto LAB_005c9cd6;
      sVar1 = psVar12[1];
      iVar8 = piVar3[0x21];
      iVar6 = (int)*psVar12 * piVar3[0x20] >> 8;
      *piVar9 = iVar6;
      iVar8 = sVar1 * iVar8 >> 8;
      *piVar11 = piVar3[0x25] * iVar6 >> 0x10;
      piVar9[1] = iVar8;
      piVar11[1] = piVar3[0x26] * iVar8 >> 0x10;
      psVar12 = psVar12 + 2;
      piVar9 = piVar9 + 2;
      piVar11 = piVar11 + 2;
      iVar6 = piVar3[0x1f] + -1;
      piVar3[0x21] = piVar3[0x21] + piVar3[0x23];
      piVar3[0x20] = piVar3[0x20] + piVar3[0x22];
      piVar3[0x1f] = iVar6;
      piVar3[0x25] = piVar3[0x25] + piVar3[0x27];
      piVar3[0x26] = piVar3[0x26] + piVar3[0x28];
      piVar3[0x24] = piVar3[0x24] + -1;
      uVar7 = uVar7 - 1;
    }
    for (; uVar7 != 0; uVar7 = uVar7 - 1) {
      sVar1 = *psVar12;
      sVar2 = psVar12[1];
      *piVar9 = *piVar9 + sVar1 * iVar4;
      psVar12 = psVar12 + 2;
      piVar9[1] = piVar9[1] + sVar2 * iVar5;
      piVar9 = piVar9 + 2;
      *piVar11 = *piVar11 + (piVar3[0x13] * sVar1 * iVar4 >> 8);
      piVar11[1] = piVar11[1] + (piVar3[0x13] * sVar2 * iVar5 >> 8);
      piVar11 = piVar11 + 2;
    }
  }
LAB_005c9cd6:
  if (*piVar3 == 1) {
    *piVar3 = 3;
    piVar3[0x18] = 0;
    piVar3[0x19] = 0;
  }
  return param_1;
}
