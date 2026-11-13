// TOOLCHAIN rw9044
// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c2620: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef long double float10;
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
float10 FUN_004c3b90(float p0);
undefined4 FUN_004c3d90(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
// MATCH: 0x004c2620 FUN_004c2620
void FUN_004c2620(int param_1)

{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  int iVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float10 fVar11;
  float10 fVar12;
  float10 fVar13;
  
  fVar3 = *(float *)(param_1 + 0x68);
  fVar4 = *(float *)(param_1 + 0x80);
  fVar5 = *(float *)(param_1 + 0x84);
  iVar6 = *(int *)(param_1 + 4);
  fVar7 = *(float *)(param_1 + 0x6c);
  pfVar1 = (float *)(param_1 + 0x124);
  *(float *)(param_1 + 0x150) = fVar4;
  pfVar2 = (float *)(param_1 + 0x94);
  *(float *)(param_1 + 0x144) = fVar4;
  *(float *)(param_1 + 0x138) = fVar4;
  *(float *)(param_1 + 300) = fVar4;
  *(float *)(param_1 + 0x180) = fVar5;
  *(float *)(param_1 + 0x174) = fVar5;
  *(float *)(param_1 + 0x168) = fVar5;
  *(float *)(param_1 + 0x15c) = fVar5;
  fVar4 = _DAT_005cc320 - fVar4;
  fVar9 = fVar4 * -*(float *)(param_1 + 0x78);
  fVar8 = fVar9 + fVar3;
  *(float *)(param_1 + 0x148) = fVar8;
  *pfVar1 = fVar8;
  fVar9 = fVar9 - fVar3;
  *(float *)(param_1 + 0x13c) = fVar9;
  *(float *)(param_1 + 0x130) = fVar9;
  fVar5 = _DAT_005cc320 - fVar5;
  fVar9 = fVar5 * -*(float *)(param_1 + 0x78);
  fVar8 = fVar9 + fVar3;
  *(float *)(param_1 + 0x178) = fVar8;
  *(float *)(param_1 + 0x154) = fVar8;
  fVar9 = fVar9 - fVar3;
  *(float *)(param_1 + 0x16c) = fVar9;
  *(float *)(param_1 + 0x160) = fVar9;
  fVar4 = fVar4 * *(float *)(param_1 + 0x7c);
  fVar3 = fVar7 + fVar4;
  *(float *)(param_1 + 0x134) = fVar3;
  *(float *)(param_1 + 0x128) = fVar3;
  fVar4 = fVar4 - fVar7;
  *(float *)(param_1 + 0x14c) = fVar4;
  *(float *)(param_1 + 0x140) = fVar4;
  fVar5 = fVar5 * *(float *)(param_1 + 0x7c);
  fVar3 = fVar7 + fVar5;
  *(float *)(param_1 + 0x164) = fVar3;
  *(float *)(param_1 + 0x158) = fVar3;
  fVar5 = fVar5 - fVar7;
  *(float *)(param_1 + 0x17c) = fVar5;
  *(float *)(param_1 + 0x170) = fVar5;
  FUN_004c3d90(pfVar1,pfVar1,8,iVar6 + 0x50);
  *pfVar2 = *(float *)(iVar6 + 0x70);
  *(undefined4 *)(param_1 + 0x98) = *(undefined4 *)(iVar6 + 0x74);
  *(undefined4 *)(param_1 + 0x9c) = *(undefined4 *)(iVar6 + 0x78);
  *(char *)(param_1 + 0xa4) = (char)((int)*pfVar2 >> 0x1f) + '\x01';
  *(char *)(param_1 + 0xa5) = (char)((int)*(undefined4 *)(param_1 + 0x98) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0xa6) = (char)((int)*(undefined4 *)(param_1 + 0x9c) >> 0x1f) + '\x01';
  *(float *)(param_1 + 0xa0) =
       *(float *)(param_1 + 0x15c) * *(float *)(param_1 + 0x9c) +
       *(float *)(param_1 + 0x154) * *pfVar2 +
       *(float *)(param_1 + 0x98) * *(float *)(param_1 + 0x158);
  *(float *)(param_1 + 0xa8) = -*pfVar2;
  *(float *)(param_1 + 0xac) = -*(float *)(param_1 + 0x98);
  *(float *)(param_1 + 0xb0) = -*(float *)(param_1 + 0x9c);
  *(char *)(param_1 + 0xb8) = (char)((int)*(undefined4 *)(param_1 + 0xa8) >> 0x1f) + '\x01';
  *(float *)(param_1 + 0xb4) =
       *(float *)(param_1 + 300) * -*(float *)(param_1 + 0x9c) +
       -*pfVar2 * *pfVar1 + *(float *)(param_1 + 0x128) * -*(float *)(param_1 + 0x98);
  *(char *)(param_1 + 0xb9) = (char)((int)*(undefined4 *)(param_1 + 0xac) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0xba) = (char)((int)*(undefined4 *)(param_1 + 0xb0) >> 0x1f) + '\x01';
  fVar3 = *(float *)(param_1 + 0x130) - *(float *)(param_1 + 0x160);
  fVar4 = *(float *)(param_1 + 0x134) - *(float *)(param_1 + 0x164);
  fVar5 = *(float *)(param_1 + 0x138) - *(float *)(param_1 + 0x168);
  fVar7 = *(float *)(param_1 + 0x16c) - *(float *)(param_1 + 0x160);
  fVar10 = *(float *)(param_1 + 0x170) - *(float *)(param_1 + 0x164);
  fVar9 = *(float *)(param_1 + 0x174) - *(float *)(param_1 + 0x168);
  fVar8 = fVar4 * fVar9 - fVar10 * fVar5;
  *(float *)(param_1 + 0xbc) = fVar8;
  fVar9 = fVar7 * fVar5 - fVar9 * fVar3;
  *(float *)(param_1 + 0xc0) = fVar9;
  fVar7 = fVar10 * fVar3 - fVar7 * fVar4;
  *(float *)(param_1 + 0xc4) = fVar7;
  fVar11 = (float10)FUN_004c3b90(fVar7 * fVar7 + fVar9 * fVar9 + fVar8 * fVar8);
  fVar12 = (float10)*(float *)(param_1 + 0xbc) * fVar11;
  *(float *)(param_1 + 0xbc) = (float)fVar12;
  fVar13 = (float10)*(float *)(param_1 + 0xc0) * fVar11;
  *(float *)(param_1 + 0xc0) = (float)fVar13;
  fVar11 = (float10)*(float *)(param_1 + 0xc4) * fVar11;
  *(float *)(param_1 + 0xc4) = (float)fVar11;
  *(float *)(param_1 + 200) =
       (float)((float10)*(float *)(param_1 + 0x138) * fVar11 +
              (float10)*(float *)(param_1 + 0x130) * fVar12 +
              (float10)*(float *)(param_1 + 0x134) * fVar13);
  *(char *)(param_1 + 0xcc) = (char)((int)*(undefined4 *)(param_1 + 0xbc) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0xcd) = (char)((int)*(undefined4 *)(param_1 + 0xc0) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0xce) = (char)((int)*(undefined4 *)(param_1 + 0xc4) >> 0x1f) + '\x01';
  fVar7 = *(float *)(param_1 + 0x154) - *(float *)(param_1 + 0x160);
  fVar10 = *(float *)(param_1 + 0x158) - *(float *)(param_1 + 0x164);
  fVar9 = *(float *)(param_1 + 0x15c) - *(float *)(param_1 + 0x168);
  fVar8 = fVar10 * fVar5 - fVar9 * fVar4;
  *(float *)(param_1 + 0xd0) = fVar8;
  fVar5 = fVar9 * fVar3 - fVar7 * fVar5;
  *(float *)(param_1 + 0xd4) = fVar5;
  fVar3 = fVar7 * fVar4 - fVar10 * fVar3;
  *(float *)(param_1 + 0xd8) = fVar3;
  fVar12 = (float10)FUN_004c3b90(fVar3 * fVar3 + fVar5 * fVar5 + fVar8 * fVar8);
  fVar13 = (float10)*(float *)(param_1 + 0xd0) * fVar12;
  *(float *)(param_1 + 0xd0) = (float)fVar13;
  fVar3 = (float)((float10)*(float *)(param_1 + 0xd4) * fVar12);
  *(float *)(param_1 + 0xd4) = fVar3;
  fVar4 = (float)((float10)*(float *)(param_1 + 0xd8) * fVar12);
  *(float *)(param_1 + 0xd8) = fVar4;
  *(char *)(param_1 + 0xe0) = (char)((int)*(undefined4 *)(param_1 + 0xd0) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0xe1) = (char)((int)fVar3 >> 0x1f) + '\x01';
  *(char *)(param_1 + 0xe2) = (char)((int)fVar4 >> 0x1f) + '\x01';
  *(float *)(param_1 + 0xdc) =
       (float)((float10)*(float *)(param_1 + 0x138) * (float10)fVar4 +
              (float10)*(float *)(param_1 + 0x130) * fVar13 +
              (float10)*(float *)(param_1 + 0x134) * (float10)fVar3);
  *(float *)(param_1 + 0xe4) = -*(float *)(param_1 + 0xbc);
  *(float *)(param_1 + 0xe8) = -*(float *)(param_1 + 0xc0);
  *(float *)(param_1 + 0xec) = -*(float *)(param_1 + 0xc4);
  *(float *)(param_1 + 0xf0) =
       *(float *)(param_1 + 0x150) * -*(float *)(param_1 + 0xc4) +
       *(float *)(param_1 + 0x14c) * -*(float *)(param_1 + 0xc0) +
       *(float *)(param_1 + 0x148) * -*(float *)(param_1 + 0xbc);
  *(float *)(param_1 + 0xf8) = (float)-fVar13;
  *(float *)(param_1 + 0xfc) = (float)-(float10)fVar3;
  *(char *)(param_1 + 0xf4) = (char)((int)*(undefined4 *)(param_1 + 0xe4) >> 0x1f) + '\x01';
  *(float *)(param_1 + 0x100) = (float)-(float10)fVar4;
  *(char *)(param_1 + 0xf5) = (char)((int)*(undefined4 *)(param_1 + 0xe8) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0xf6) = (char)((int)*(undefined4 *)(param_1 + 0xec) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0x108) = (char)((int)*(undefined4 *)(param_1 + 0xf8) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0x109) = (char)((int)*(undefined4 *)(param_1 + 0xfc) >> 0x1f) + '\x01';
  *(char *)(param_1 + 0x10a) = (char)((int)*(undefined4 *)(param_1 + 0x100) >> 0x1f) + '\x01';
  *(float *)(param_1 + 0x104) =
       (float)((float10)*(float *)(param_1 + 0x150) * -(float10)fVar4 +
              (float10)*(float *)(param_1 + 0x14c) * -(float10)fVar3 +
              (float10)*(float *)(param_1 + 0x148) * -fVar13);
  return;
}
