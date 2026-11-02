// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c1f80: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs19_20261102/results.tsv. */
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
typedef long double float10;
float10 FUN_004c3b90(float p0);
// MATCH: 0x004c1f80 FUN_004c1f80

void FUN_004c1f80(int param_1)

{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  int iVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  float *pfVar16;
  int iVar17;
  float10 fVar18;
  float10 fVar19;
  float10 fVar20;
  
  iVar8 = *(int *)(param_1 + 4);
  fVar3 = -*(float *)(param_1 + 0x78);
  pfVar2 = (float *)(param_1 + 0x94);
  fVar6 = *(float *)(param_1 + 0x7c) * *(float *)(iVar8 + 0x60) + fVar3 * *(float *)(iVar8 + 0x50);
  fVar7 = *(float *)(param_1 + 0x7c) * *(float *)(iVar8 + 100) + *(float *)(iVar8 + 0x54) * fVar3;
  fVar9 = *(float *)(param_1 + 0x7c) * *(float *)(iVar8 + 0x68) + *(float *)(iVar8 + 0x58) * fVar3;
  fVar4 = *(float *)(param_1 + 0x68) * *(float *)(iVar8 + 0x50);
  fVar10 = *(float *)(param_1 + 0x68) * *(float *)(iVar8 + 0x54);
  fVar12 = *(float *)(param_1 + 0x68) * *(float *)(iVar8 + 0x58);
  fVar3 = *(float *)(iVar8 + 0x78);
  fVar13 = *(float *)(param_1 + 0x6c) * *(float *)(iVar8 + 0x60);
  fVar14 = *(float *)(param_1 + 0x6c) * *(float *)(iVar8 + 100);
  fVar15 = *(float *)(param_1 + 0x6c) * *(float *)(iVar8 + 0x68);
  fVar5 = fVar13 + *(float *)(iVar8 + 0x70) + fVar4;
  fVar11 = fVar14 + *(float *)(iVar8 + 0x74) + fVar10;
  *(float *)(param_1 + 0x124) = fVar5;
  fVar3 = fVar15 + fVar3 + fVar12;
  *(float *)(param_1 + 0x128) = fVar11;
  *(float *)(param_1 + 300) = fVar3;
  pfVar1 = (float *)(param_1 + 0x130);
  fVar5 = fVar5 - (fVar4 + fVar4);
  fVar11 = fVar11 - (fVar10 + fVar10);
  *pfVar1 = fVar5;
  fVar3 = fVar3 - (fVar12 + fVar12);
  *(float *)(param_1 + 0x134) = fVar11;
  *(float *)(param_1 + 0x138) = fVar3;
  fVar5 = fVar5 - (fVar13 + fVar13);
  fVar11 = fVar11 - (fVar14 + fVar14);
  *(float *)(param_1 + 0x13c) = fVar5;
  fVar3 = fVar3 - (fVar15 + fVar15);
  *(float *)(param_1 + 0x140) = fVar11;
  *(float *)(param_1 + 0x144) = fVar3;
  *(float *)(param_1 + 0x148) = fVar5 + fVar4 + fVar4;
  *(float *)(param_1 + 0x14c) = fVar11 + fVar10 + fVar10;
  *(float *)(param_1 + 0x150) = fVar3 + fVar12 + fVar12;
  iVar17 = 4;
  pfVar16 = (float *)(param_1 + 0x15c);
  do {
    fVar3 = pfVar16[-0xe];
    fVar4 = pfVar16[-0xd];
    fVar5 = pfVar16[-0xc];
    pfVar16[-0xe] = *(float *)(iVar8 + 0x80) + fVar6;
    pfVar16[-0xd] = *(float *)(iVar8 + 0x84) + fVar7;
    pfVar16[-0xc] = *(float *)(iVar8 + 0x88) + fVar9;
    pfVar16[-0xe] = *(float *)(param_1 + 0x80) * (fVar3 - fVar6) + pfVar16[-0xe];
    pfVar16[-0xd] = *(float *)(param_1 + 0x80) * (fVar4 - fVar7) + pfVar16[-0xd];
    pfVar16[-0xc] = *(float *)(param_1 + 0x80) * (fVar5 - fVar9) + pfVar16[-0xc];
    pfVar16[-2] = *(float *)(iVar8 + 0x80) + fVar6;
    pfVar16[-1] = *(float *)(iVar8 + 0x84) + fVar7;
    *pfVar16 = *(float *)(iVar8 + 0x88) + fVar9;
    pfVar16[-2] = *(float *)(param_1 + 0x84) * (fVar3 - fVar6) + pfVar16[-2];
    pfVar16[-1] = *(float *)(param_1 + 0x84) * (fVar4 - fVar7) + pfVar16[-1];
    iVar17 = iVar17 + -1;
    *pfVar16 = *(float *)(param_1 + 0x84) * (fVar5 - fVar9) + *pfVar16;
    pfVar16 = pfVar16 + 3;
  } while (iVar17 != 0);
  *pfVar2 = *(float *)(iVar8 + 0x70);
  *(undefined4 *)(param_1 + 0x98) = *(undefined4 *)(iVar8 + 0x74);
  *(undefined4 *)(param_1 + 0x9c) = *(undefined4 *)(iVar8 + 0x78);
  fVar3 = *(float *)(iVar8 + 0x70);
  fVar6 = *(float *)(iVar8 + 0x74);
  fVar7 = *(float *)(iVar8 + 0x78);
  *(char *)(param_1 + 0xa4) = (char)((int)*pfVar2 >> 0x1f) + '\x01';
  *(float *)(param_1 + 0xa0) =
       *(float *)(param_1 + 0x15c) * fVar7 +
       *(float *)(param_1 + 0x158) * fVar6 + fVar3 * *(float *)(param_1 + 0x154);
  *(float *)(param_1 + 0xa8) = -*pfVar2;
  *(float *)(param_1 + 0xac) = -*(float *)(param_1 + 0x98);
  *(float *)(param_1 + 0xb0) = -*(float *)(param_1 + 0x9c);
  *(char *)(param_1 + 0xa5) = (char)((int)*(undefined4 *)(param_1 + 0x98) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0xa6) = (char)((int)*(undefined4 *)(param_1 + 0x9c) >> 0x1f) + '\x01';
  *(float *)(param_1 + 0xb4) =
       *(float *)(param_1 + 300) * -*(float *)(param_1 + 0x9c) +
       -*pfVar2 * *(float *)(param_1 + 0x124) +
       *(float *)(param_1 + 0x128) * -*(float *)(param_1 + 0x98);
  *(char *)(param_1 + 0xb8) = (char)((int)*(undefined4 *)(param_1 + 0xa8) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0xb9) = (char)((int)*(undefined4 *)(param_1 + 0xac) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0xba) = (char)((int)*(undefined4 *)(param_1 + 0xb0) >> 0x1f) + '\x01';
  fVar3 = *pfVar1 - *(float *)(param_1 + 0x160);
  fVar6 = *(float *)(param_1 + 0x134) - *(float *)(param_1 + 0x164);
  fVar7 = *(float *)(param_1 + 0x138) - *(float *)(param_1 + 0x168);
  fVar9 = *(float *)(param_1 + 0x16c) - *(float *)(param_1 + 0x160);
  fVar10 = *(float *)(param_1 + 0x170) - *(float *)(param_1 + 0x164);
  fVar5 = *(float *)(param_1 + 0x174) - *(float *)(param_1 + 0x168);
  fVar4 = fVar6 * fVar5 - fVar10 * fVar7;
  *(float *)(param_1 + 0xbc) = fVar4;
  fVar5 = fVar9 * fVar7 - fVar5 * fVar3;
  *(float *)(param_1 + 0xc0) = fVar5;
  fVar9 = fVar10 * fVar3 - fVar9 * fVar6;
  *(float *)(param_1 + 0xc4) = fVar9;
  fVar18 = (float10)FUN_004c3b90(fVar9 * fVar9 + fVar5 * fVar5 + fVar4 * fVar4);
  fVar19 = (float10)*(float *)(param_1 + 0xbc) * fVar18;
  *(float *)(param_1 + 0xbc) = (float)fVar19;
  fVar20 = (float10)*(float *)(param_1 + 0xc0) * fVar18;
  *(float *)(param_1 + 0xc0) = (float)fVar20;
  fVar18 = (float10)*(float *)(param_1 + 0xc4) * fVar18;
  *(float *)(param_1 + 0xc4) = (float)fVar18;
  *(float *)(param_1 + 200) =
       (float)((float10)*(float *)(param_1 + 0x138) * fVar18 +
              (float10)*pfVar1 * fVar19 + (float10)*(float *)(param_1 + 0x134) * fVar20);
  *(char *)(param_1 + 0xcc) = (char)((int)*(undefined4 *)(param_1 + 0xbc) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0xcd) = (char)((int)*(undefined4 *)(param_1 + 0xc0) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0xce) = (char)((int)*(undefined4 *)(param_1 + 0xc4) >> 0x1f) + '\x01';
  fVar9 = *(float *)(param_1 + 0x154) - *(float *)(param_1 + 0x160);
  fVar10 = *(float *)(param_1 + 0x158) - *(float *)(param_1 + 0x164);
  fVar5 = *(float *)(param_1 + 0x15c) - *(float *)(param_1 + 0x168);
  fVar4 = fVar10 * fVar7 - fVar5 * fVar6;
  *(float *)(param_1 + 0xd0) = fVar4;
  fVar7 = fVar5 * fVar3 - fVar9 * fVar7;
  *(float *)(param_1 + 0xd4) = fVar7;
  fVar3 = fVar9 * fVar6 - fVar10 * fVar3;
  *(float *)(param_1 + 0xd8) = fVar3;
  fVar18 = (float10)FUN_004c3b90(fVar3 * fVar3 + fVar7 * fVar7 + fVar4 * fVar4);
  fVar19 = (float10)*(float *)(param_1 + 0xd0) * fVar18;
  *(float *)(param_1 + 0xd0) = (float)fVar19;
  fVar20 = (float10)*(float *)(param_1 + 0xd4) * fVar18;
  *(float *)(param_1 + 0xd4) = (float)fVar20;
  fVar18 = (float10)*(float *)(param_1 + 0xd8) * fVar18;
  *(float *)(param_1 + 0xd8) = (float)fVar18;
  *(float *)(param_1 + 0xdc) =
       (float)((float10)*(float *)(param_1 + 0x138) * fVar18 +
              (float10)*(float *)(param_1 + 0x134) * fVar20 + (float10)*pfVar1 * fVar19);
  *(char *)(param_1 + 0xe0) = (char)((int)*(undefined4 *)(param_1 + 0xd0) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0xe1) = (char)((int)*(undefined4 *)(param_1 + 0xd4) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0xe2) = (char)((int)*(undefined4 *)(param_1 + 0xd8) >> 0x1f) + '\x01';
  fVar3 = *(float *)(param_1 + 0x148) - *(float *)(param_1 + 0x178);
  fVar6 = *(float *)(param_1 + 0x14c) - *(float *)(param_1 + 0x17c);
  fVar7 = *(float *)(param_1 + 0x150) - *(float *)(param_1 + 0x180);
  fVar9 = *(float *)(param_1 + 0x154) - *(float *)(param_1 + 0x178);
  fVar10 = *(float *)(param_1 + 0x158) - *(float *)(param_1 + 0x17c);
  fVar5 = *(float *)(param_1 + 0x15c) - *(float *)(param_1 + 0x180);
  fVar4 = fVar6 * fVar5 - fVar10 * fVar7;
  *(float *)(param_1 + 0xe4) = fVar4;
  fVar5 = fVar9 * fVar7 - fVar5 * fVar3;
  *(float *)(param_1 + 0xe8) = fVar5;
  fVar9 = fVar10 * fVar3 - fVar9 * fVar6;
  *(float *)(param_1 + 0xec) = fVar9;
  fVar18 = (float10)FUN_004c3b90(fVar9 * fVar9 + fVar5 * fVar5 + fVar4 * fVar4);
  fVar19 = (float10)*(float *)(param_1 + 0xe4) * fVar18;
  *(float *)(param_1 + 0xe4) = (float)fVar19;
  fVar20 = (float10)*(float *)(param_1 + 0xe8) * fVar18;
  *(float *)(param_1 + 0xe8) = (float)fVar20;
  fVar18 = (float10)*(float *)(param_1 + 0xec) * fVar18;
  *(float *)(param_1 + 0xec) = (float)fVar18;
  *(char *)(param_1 + 0xf4) = (char)((int)*(undefined4 *)(param_1 + 0xe4) >> 0x1f) + '\x01';
  *(float *)(param_1 + 0xf0) =
       (float)((float10)*(float *)(param_1 + 0x150) * fVar18 +
              (float10)*(float *)(param_1 + 0x148) * fVar19 +
              (float10)*(float *)(param_1 + 0x14c) * fVar20);
  *(char *)(param_1 + 0xf5) = (char)((int)*(undefined4 *)(param_1 + 0xe8) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0xf6) = (char)((int)*(undefined4 *)(param_1 + 0xec) >> 0x1f) + '\x01';
  fVar9 = *(float *)(param_1 + 0x16c) - *(float *)(param_1 + 0x178);
  fVar10 = *(float *)(param_1 + 0x170) - *(float *)(param_1 + 0x17c);
  fVar5 = *(float *)(param_1 + 0x174) - *(float *)(param_1 + 0x180);
  fVar4 = fVar10 * fVar7 - fVar5 * fVar6;
  *(float *)(param_1 + 0xf8) = fVar4;
  fVar7 = fVar5 * fVar3 - fVar9 * fVar7;
  *(float *)(param_1 + 0xfc) = fVar7;
  fVar3 = fVar9 * fVar6 - fVar10 * fVar3;
  *(float *)(param_1 + 0x100) = fVar3;
  fVar18 = (float10)FUN_004c3b90(fVar3 * fVar3 + fVar7 * fVar7 + fVar4 * fVar4);
  fVar19 = (float10)*(float *)(param_1 + 0xf8) * fVar18;
  *(float *)(param_1 + 0xf8) = (float)fVar19;
  fVar20 = (float10)*(float *)(param_1 + 0xfc) * fVar18;
  *(float *)(param_1 + 0xfc) = (float)fVar20;
  fVar18 = (float10)*(float *)(param_1 + 0x100) * fVar18;
  *(float *)(param_1 + 0x100) = (float)fVar18;
  *(float *)(param_1 + 0x104) =
       (float)((float10)*(float *)(param_1 + 0x150) * fVar18 +
              (float10)*(float *)(param_1 + 0x148) * fVar19 +
              (float10)*(float *)(param_1 + 0x14c) * fVar20);
  *(char *)(param_1 + 0x108) = (char)((int)*(undefined4 *)(param_1 + 0xf8) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0x109) = (char)((int)*(undefined4 *)(param_1 + 0xfc) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0x10a) = (char)((int)*(undefined4 *)(param_1 + 0x100) >> 0x1f) + '\x01';
  return;
}
