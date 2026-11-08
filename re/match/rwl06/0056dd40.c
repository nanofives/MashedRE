// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x0056dd40: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl06_20261108/results.tsv. */
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
typedef unsigned int uint;
typedef unsigned __int64 ulonglong;
MGLOBAL(undefined1, DAT_005e5a60, 0x005e5a60);
MGLOBAL(undefined1, UNK_005e5a64, 0x005e5a64);
MGLOBAL(undefined1, UNK_005e5a68, 0x005e5a68);
MGLOBAL(undefined1, UNK_005e5a6c, 0x005e5a6c);
MGLOBAL(undefined1, DAT_005e5a20, 0x005e5a20);
MGLOBAL(undefined1, UNK_005e5a24, 0x005e5a24);
MGLOBAL(undefined1, UNK_005e5a28, 0x005e5a28);
MGLOBAL(undefined1, UNK_005e5a2c, 0x005e5a2c);
MGLOBAL(undefined1, DAT_005e5a30, 0x005e5a30);
MGLOBAL(undefined1, UNK_005e5a34, 0x005e5a34);
MGLOBAL(undefined1, UNK_005e5a38, 0x005e5a38);
MGLOBAL(undefined1, UNK_005e5a3c, 0x005e5a3c);
MGLOBAL(undefined1, DAT_005e5a70, 0x005e5a70);
MGLOBAL(undefined1, UNK_005e5a74, 0x005e5a74);
MGLOBAL(undefined1, UNK_005e5a78, 0x005e5a78);
MGLOBAL(undefined1, UNK_005e5a7c, 0x005e5a7c);
MGLOBAL(undefined1, DAT_005e5a40, 0x005e5a40);
MGLOBAL(undefined1, UNK_005e5a44, 0x005e5a44);
MGLOBAL(undefined1, UNK_005e5a48, 0x005e5a48);
MGLOBAL(undefined1, UNK_005e5a4c, 0x005e5a4c);
MGLOBAL(int, _DAT_005e5a60, 0x005e5a60);
MGLOBAL(int, _UNK_005e5a64, 0x005e5a64);
MGLOBAL(int, _UNK_005e5a68, 0x005e5a68);
MGLOBAL(int, _DAT_005e5a30, 0x005e5a30);
MGLOBAL(int, _DAT_005e5a20, 0x005e5a20);
MGLOBAL(int, _UNK_005e5a34, 0x005e5a34);
MGLOBAL(int, _UNK_005e5a24, 0x005e5a24);
MGLOBAL(int, _UNK_005e5a38, 0x005e5a38);
MGLOBAL(int, _UNK_005e5a28, 0x005e5a28);
MGLOBAL(int, _UNK_005e5a3c, 0x005e5a3c);
MGLOBAL(int, _UNK_005e5a2c, 0x005e5a2c);
MGLOBAL(int, _DAT_005e5a70, 0x005e5a70);
MGLOBAL(int, _UNK_005e5a74, 0x005e5a74);
MGLOBAL(int, _UNK_005e5a78, 0x005e5a78);
MGLOBAL(int, _UNK_005e5a7c, 0x005e5a7c);
MGLOBAL(int, _UNK_005e5a44, 0x005e5a44);
MGLOBAL(int, _UNK_005e5a48, 0x005e5a48);
MGLOBAL(int, _UNK_005e5a4c, 0x005e5a4c);
MGLOBAL(int, _DAT_005e5a40, 0x005e5a40);
// MATCH: 0x0056dd40 FUN_0056dd40
/* [C1 2026-05-19] Signature: `void FUN_0056dd40(int param_1)`. */

void FUN_0056dd40(int param_1)

{
  float *pfVar1;
  undefined1 auVar2 [16];
  undefined1 auVar3 [12];
  undefined1 auVar4 [12];
  uint uVar5;
  uint uVar6;
  undefined1 auVar7 [16];
  uint *puVar8;
  int iVar9;
  float *pfVar10;
  undefined1 (*pauVar11) [16];
  float *pfVar12;
  int *piVar13;
  uint uVar14;
  uint uVar15;
  uint uVar16;
  uint uVar17;
  float fVar18;
  float fVar20;
  float fVar21;
  float fVar22;
  undefined1 auVar19 [16];
  float fVar23;
  float fVar27;
  undefined1 in_XMM2 [16];
  float fVar28;
  undefined1 auVar24 [16];
  undefined1 auVar25 [16];
  undefined1 auVar26 [16];
  float fVar29;
  float fVar30;
  float fVar31;
  float fVar32;
  float fVar33;
  float fVar34;
  float fVar35;
  float fVar36;
  float fVar37;
  float fVar38;
  float fVar39;
  float fVar40;
  float fVar41;
  float fVar42;
  float fVar43;
  float fVar44;
  float fVar45;
  float fVar46;
  float fVar47;
  uint uVar48;
  uint uVar49;
  uint uVar50;
  int in_stack_00000014;
  float *in_stack_00000020;
  int in_stack_0000002c;
  int in_stack_00000038;
  int in_stack_00000044;
  int in_stack_00000050;
  int in_stack_0000005c;
  int in_stack_00000060;
  int in_stack_00000068;
  float in_stack_00000074;
  int local_f8;
  
  if (in_stack_00000060 != 0) {
    piVar13 = (int *)(param_1 + 0x2c);
    local_f8 = in_stack_00000060;
    in_stack_00000044 = in_stack_00000044 - (int)in_stack_00000020;
    pfVar12 = in_stack_00000020;
    do {
      fVar41 = 0.0;
      fVar22 = 0.0;
      fVar29 = 0.0;
      fVar31 = 0.0;
      if (piVar13[-8] != -1) {
        puVar8 = (uint *)(piVar13[-8] * 0x30 + in_stack_00000068);
        fVar41 = (float)puVar8[3];
        uVar14 = puVar8[4];
        uVar15 = puVar8[5];
        uVar16 = puVar8[6];
        uVar17 = *puVar8;
        uVar48 = puVar8[1];
        uVar49 = puVar8[2];
        fVar35 = (float)(_DAT_005e5a60 & piVar13[-0xb]) * fVar41;
        fVar38 = (float)(_UNK_005e5a64 & piVar13[-10]) * fVar41;
        fVar41 = (float)(_UNK_005e5a68 & piVar13[-9]) * fVar41;
        fVar29 = (float)(_DAT_005e5a60 & piVar13[-7]);
        fVar31 = (float)(_UNK_005e5a64 & piVar13[-6]);
        fVar33 = (float)(_UNK_005e5a68 & piVar13[-5]);
        uVar50 = puVar8[8];
        uVar5 = puVar8[9];
        uVar6 = puVar8[10];
        fVar22 = (float)puVar8[3];
        fVar18 = (float)(_UNK_005e5a68 & uVar49) * fVar33 + (float)(_DAT_005e5a60 & uVar17) * fVar29
                 + (float)(_UNK_005e5a64 & uVar48) * fVar31;
        fVar20 = (float)(_UNK_005e5a68 & uVar16) * fVar33 + (float)(_DAT_005e5a60 & uVar14) * fVar29
                 + (float)(_UNK_005e5a64 & uVar15) * fVar31;
        fVar21 = (float)(_UNK_005e5a68 & uVar6) * fVar33 + (float)(_DAT_005e5a60 & uVar50) * fVar29
                 + (float)(_UNK_005e5a64 & uVar5) * fVar31;
        fVar31 = (float)(_DAT_005e5a60 & piVar13[5]) * fVar22;
        fVar33 = (float)(_UNK_005e5a64 & piVar13[6]) * fVar22;
        fVar22 = (float)(_UNK_005e5a68 & piVar13[7]) * fVar22;
        fVar36 = (float)(_DAT_005e5a60 & piVar13[9]);
        fVar39 = (float)(_UNK_005e5a64 & piVar13[10]);
        fVar42 = (float)(_UNK_005e5a68 & piVar13[0xb]);
        fVar29 = (float)puVar8[3];
        fVar23 = (float)(_UNK_005e5a68 & uVar49) * fVar42 + (float)(_DAT_005e5a60 & uVar17) * fVar36
                 + (float)(_UNK_005e5a64 & uVar48) * fVar39;
        fVar27 = (float)(_UNK_005e5a68 & uVar16) * fVar42 + (float)(_DAT_005e5a60 & uVar14) * fVar36
                 + (float)(_UNK_005e5a64 & uVar15) * fVar39;
        fVar42 = (float)(_UNK_005e5a68 & uVar6) * fVar42 + (float)(_DAT_005e5a60 & uVar50) * fVar36
                 + (float)(_UNK_005e5a64 & uVar5) * fVar39;
        fVar28 = fVar22 * fVar22 + fVar31 * fVar31 + fVar33 * fVar33;
        fVar23 = fVar23 * fVar23;
        fVar27 = fVar27 * fVar27;
        fVar42 = fVar42 * fVar42;
        fVar31 = (float)(_DAT_005e5a60 & piVar13[0x15]) * fVar29;
        fVar33 = (float)(_UNK_005e5a64 & piVar13[0x16]) * fVar29;
        fVar29 = (float)(_UNK_005e5a68 & piVar13[0x17]) * fVar29;
        fVar36 = (float)(_DAT_005e5a60 & piVar13[0x19]);
        fVar39 = (float)(_UNK_005e5a64 & piVar13[0x1a]);
        fVar45 = (float)(_UNK_005e5a68 & piVar13[0x1b]);
        fVar22 = (float)puVar8[3];
        fVar30 = (float)(_UNK_005e5a68 & uVar49) * fVar45 + (float)(_DAT_005e5a60 & uVar17) * fVar36
                 + (float)(_UNK_005e5a64 & uVar48) * fVar39;
        fVar32 = (float)(_UNK_005e5a68 & uVar16) * fVar45 + (float)(_DAT_005e5a60 & uVar14) * fVar36
                 + (float)(_UNK_005e5a64 & uVar15) * fVar39;
        fVar45 = (float)(_UNK_005e5a68 & uVar6) * fVar45 + (float)(_DAT_005e5a60 & uVar50) * fVar36
                 + (float)(_UNK_005e5a64 & uVar5) * fVar39;
        fVar34 = fVar29 * fVar29 + fVar31 * fVar31 + fVar33 * fVar33;
        fVar45 = fVar45 * fVar45;
        fVar29 = (float)(_DAT_005e5a60 & piVar13[0x25]) * fVar22;
        fVar39 = (float)(_UNK_005e5a64 & piVar13[0x26]) * fVar22;
        fVar22 = (float)(_UNK_005e5a68 & piVar13[0x27]) * fVar22;
        fVar36 = (float)(_DAT_005e5a60 & piVar13[0x29]);
        fVar43 = (float)(_UNK_005e5a64 & piVar13[0x2a]);
        fVar46 = (float)(_UNK_005e5a68 & piVar13[0x2b]);
        (*(uint *)((char *)&auVar26 + 4)) = fVar27;
        (*(uint *)((char *)&auVar26 + 0)) = fVar23;
        (*(uint *)((char *)&auVar26 + 8)) = fVar42;
        (*(uint *)((char *)&auVar26 + 12)) = fVar28;
        fVar31 = (float)(_UNK_005e5a68 & uVar49) * fVar46 + (float)(_DAT_005e5a60 & uVar17) * fVar36
                 + (float)(_UNK_005e5a64 & uVar48) * fVar43;
        fVar33 = (float)(_UNK_005e5a68 & uVar16) * fVar46 + (float)(_DAT_005e5a60 & uVar14) * fVar36
                 + (float)(_UNK_005e5a64 & uVar15) * fVar43;
        fVar36 = (float)(_UNK_005e5a68 & uVar6) * fVar46 + (float)(_DAT_005e5a60 & uVar50) * fVar36
                 + (float)(_UNK_005e5a64 & uVar5) * fVar43;
        fVar39 = fVar22 * fVar22 + fVar29 * fVar29 + fVar39 * fVar39;
        fVar33 = fVar33 * fVar33;
        fVar36 = fVar36 * fVar36;
        (*(ulonglong *)((char *)&auVar3 + 4)) = (*(ulonglong *)((char *)&auVar26 + 8));
        (*(uint *)((char *)&auVar3 + 0)) = fVar33;
        (*(ulonglong *)((char *)&auVar24 + 0)) = (*(ulonglong *)((char *)&auVar3 + 0)) << 0x20;
        (*(uint *)((char *)&auVar24 + 8)) = fVar36;
        (*(uint *)((char *)&auVar24 + 12)) = fVar39;
        (*(ulonglong *)((char *)&in_XMM2 + 8)) = (*(ulonglong *)((char *)&auVar24 + 8));
        (*(uint *)((char *)&in_XMM2 + 4)) = fVar34;
        (*(uint *)((char *)&in_XMM2 + 0)) = fVar45;
        fVar41 = fVar20 * fVar20 + fVar41 * fVar41 + fVar35 * fVar35 + fVar38 * fVar38 +
                 fVar18 * fVar18 + fVar21 * fVar21;
        fVar22 = fVar27 + fVar28 + fVar23 + fVar42;
        fVar29 = fVar32 * fVar32 + fVar34 + fVar30 * fVar30 + fVar45;
        fVar31 = fVar33 + fVar39 + fVar31 * fVar31 + fVar36;
      }
      if (*piVar13 != -1) {
        puVar8 = (uint *)(*piVar13 * 0x30 + in_stack_00000068);
        fVar33 = (float)puVar8[3];
        uVar14 = *puVar8;
        uVar15 = puVar8[1];
        uVar16 = puVar8[2];
        fVar37 = (float)(_DAT_005e5a60 & piVar13[-3]) * fVar33;
        fVar40 = (float)(_UNK_005e5a64 & piVar13[-2]) * fVar33;
        fVar33 = (float)(_UNK_005e5a68 & piVar13[-1]) * fVar33;
        uVar17 = puVar8[4];
        uVar48 = puVar8[5];
        uVar49 = puVar8[6];
        fVar39 = (float)(_DAT_005e5a60 & piVar13[1]);
        fVar18 = (float)(_UNK_005e5a64 & piVar13[2]);
        fVar20 = (float)(_UNK_005e5a68 & piVar13[3]);
        uVar50 = puVar8[8];
        uVar5 = puVar8[9];
        uVar6 = puVar8[10];
        fVar36 = (float)puVar8[3];
        fVar21 = (float)(_UNK_005e5a68 & uVar16) * fVar20 + (float)(_DAT_005e5a60 & uVar14) * fVar39
                 + (float)(_UNK_005e5a64 & uVar15) * fVar18;
        fVar27 = (float)(_UNK_005e5a68 & uVar49) * fVar20 + (float)(_DAT_005e5a60 & uVar17) * fVar39
                 + (float)(_UNK_005e5a64 & uVar48) * fVar18;
        fVar28 = (float)(_UNK_005e5a68 & uVar6) * fVar20 + (float)(_DAT_005e5a60 & uVar50) * fVar39
                 + (float)(_UNK_005e5a64 & uVar5) * fVar18;
        fVar18 = (float)(_DAT_005e5a60 & piVar13[0xd]) * fVar36;
        fVar20 = (float)(_UNK_005e5a64 & piVar13[0xe]) * fVar36;
        fVar36 = (float)(_UNK_005e5a68 & piVar13[0xf]) * fVar36;
        fVar23 = (float)(_DAT_005e5a60 & piVar13[0x11]);
        fVar42 = (float)(_UNK_005e5a64 & piVar13[0x12]);
        fVar45 = (float)(_UNK_005e5a68 & piVar13[0x13]);
        fVar39 = (float)puVar8[3];
        fVar30 = (float)(_UNK_005e5a68 & uVar16) * fVar45 + (float)(_DAT_005e5a60 & uVar14) * fVar23
                 + (float)(_UNK_005e5a64 & uVar15) * fVar42;
        fVar32 = (float)(_UNK_005e5a68 & uVar49) * fVar45 + (float)(_DAT_005e5a60 & uVar17) * fVar23
                 + (float)(_UNK_005e5a64 & uVar48) * fVar42;
        fVar45 = (float)(_UNK_005e5a68 & uVar6) * fVar45 + (float)(_DAT_005e5a60 & uVar50) * fVar23
                 + (float)(_UNK_005e5a64 & uVar5) * fVar42;
        fVar34 = fVar36 * fVar36 + fVar18 * fVar18 + fVar20 * fVar20;
        fVar30 = fVar30 * fVar30;
        fVar32 = fVar32 * fVar32;
        fVar45 = fVar45 * fVar45;
        fVar18 = (float)(_DAT_005e5a60 & piVar13[0x1d]) * fVar39;
        fVar20 = (float)(_UNK_005e5a64 & piVar13[0x1e]) * fVar39;
        fVar39 = (float)(_UNK_005e5a68 & piVar13[0x1f]) * fVar39;
        fVar23 = (float)(_DAT_005e5a60 & piVar13[0x21]);
        fVar42 = (float)(_UNK_005e5a64 & piVar13[0x22]);
        fVar43 = (float)(_UNK_005e5a68 & piVar13[0x23]);
        fVar36 = (float)puVar8[3];
        fVar35 = (float)(_UNK_005e5a68 & uVar16) * fVar43 + (float)(_DAT_005e5a60 & uVar14) * fVar23
                 + (float)(_UNK_005e5a64 & uVar15) * fVar42;
        fVar38 = (float)(_UNK_005e5a68 & uVar49) * fVar43 + (float)(_DAT_005e5a60 & uVar17) * fVar23
                 + (float)(_UNK_005e5a64 & uVar48) * fVar42;
        fVar43 = (float)(_UNK_005e5a68 & uVar6) * fVar43 + (float)(_DAT_005e5a60 & uVar50) * fVar23
                 + (float)(_UNK_005e5a64 & uVar5) * fVar42;
        fVar46 = fVar39 * fVar39 + fVar18 * fVar18 + fVar20 * fVar20;
        fVar43 = fVar43 * fVar43;
        fVar23 = (float)(_DAT_005e5a60 & piVar13[0x2d]) * fVar36;
        fVar42 = (float)(_UNK_005e5a64 & piVar13[0x2e]) * fVar36;
        fVar36 = (float)(_UNK_005e5a68 & piVar13[0x2f]) * fVar36;
        fVar20 = (float)(_DAT_005e5a60 & piVar13[0x31]);
        fVar44 = (float)(_UNK_005e5a64 & piVar13[0x32]);
        fVar47 = (float)(_UNK_005e5a68 & piVar13[0x33]);
        (*(uint *)((char *)&auVar7 + 4)) = fVar32;
        (*(uint *)((char *)&auVar7 + 0)) = fVar30;
        (*(uint *)((char *)&auVar7 + 8)) = fVar45;
        (*(uint *)((char *)&auVar7 + 12)) = fVar34;
        fVar39 = (float)(_UNK_005e5a68 & uVar16) * fVar47 + (float)(_DAT_005e5a60 & uVar14) * fVar20
                 + (float)(_UNK_005e5a64 & uVar15) * fVar44;
        fVar18 = (float)(_UNK_005e5a68 & uVar49) * fVar47 + (float)(_DAT_005e5a60 & uVar17) * fVar20
                 + (float)(_UNK_005e5a64 & uVar48) * fVar44;
        fVar20 = (float)(_UNK_005e5a68 & uVar6) * fVar47 + (float)(_DAT_005e5a60 & uVar50) * fVar20
                 + (float)(_UNK_005e5a64 & uVar5) * fVar44;
        fVar36 = fVar36 * fVar36 + fVar23 * fVar23 + fVar42 * fVar42;
        fVar18 = fVar18 * fVar18;
        fVar20 = fVar20 * fVar20;
        (*(ulonglong *)((char *)&auVar4 + 4)) = (*(ulonglong *)((char *)&auVar7 + 8));
        (*(uint *)((char *)&auVar4 + 0)) = fVar18;
        (*(ulonglong *)((char *)&auVar25 + 0)) = (*(ulonglong *)((char *)&auVar4 + 0)) << 0x20;
        (*(uint *)((char *)&auVar25 + 8)) = fVar20;
        (*(uint *)((char *)&auVar25 + 12)) = fVar36;
        (*(ulonglong *)((char *)&in_XMM2 + 8)) = (*(ulonglong *)((char *)&auVar25 + 8));
        (*(uint *)((char *)&in_XMM2 + 4)) = fVar46;
        (*(uint *)((char *)&in_XMM2 + 0)) = fVar43;
        fVar41 = fVar41 + fVar27 * fVar27 + fVar33 * fVar33 + fVar37 * fVar37 + fVar40 * fVar40 +
                          fVar21 * fVar21 + fVar28 * fVar28;
        fVar22 = fVar22 + fVar32 + fVar34 + fVar30 + fVar45;
        fVar29 = fVar29 + fVar38 * fVar38 + fVar46 + fVar35 * fVar35 + fVar43;
        fVar31 = fVar31 + fVar18 + fVar36 + fVar39 * fVar39 + fVar20;
      }
      fVar32 = 0.0;
      fVar30 = 0.0;
      fVar28 = 0.0;
      fVar42 = 0.0;
      pfVar10 = (float *)((int)pfVar12 + in_stack_00000044);
      fVar33 = *pfVar10;
      fVar36 = pfVar10[1];
      fVar39 = pfVar10[2];
      fVar18 = pfVar10[3];
      fVar41 = fVar41 + in_stack_00000074 * fVar33;
      fVar22 = fVar22 + in_stack_00000074 * fVar36;
      fVar29 = fVar29 + in_stack_00000074 * fVar39;
      fVar31 = fVar31 + in_stack_00000074 * fVar18;
      (*(uint *)((char *)&auVar19 + 4)) = -(uint)(fVar22 != 0.0);
      (*(uint *)((char *)&auVar19 + 0)) = -(uint)(fVar41 != 0.0);
      (*(uint *)((char *)&auVar19 + 8)) = -(uint)(fVar29 != 0.0);
      (*(uint *)((char *)&auVar19 + 12)) = -(uint)(fVar31 != 0.0);
      iVar9 = movmskps(in_stack_00000044,auVar19);
      fVar20 = fVar41;
      fVar21 = fVar22;
      fVar23 = fVar29;
      fVar27 = fVar31;
      if (iVar9 != 0) {
        (*(uint *)((char *)&auVar2 + 4)) = fVar22;
        (*(uint *)((char *)&auVar2 + 0)) = fVar41;
        (*(uint *)((char *)&auVar2 + 8)) = fVar29;
        (*(uint *)((char *)&auVar2 + 12)) = fVar31;
        auVar26 = rsqrtps(in_XMM2,auVar2);
        fVar20 = (*(uint *)((char *)&auVar26 + 0));
        fVar21 = (*(uint *)((char *)&auVar26 + 4));
        fVar23 = (*(uint *)((char *)&auVar26 + 8));
        fVar27 = (*(uint *)((char *)&auVar26 + 12));
        fVar20 = (float)(-(uint)(fVar41 != 0.0) &
                        (uint)(_DAT_005e5a30 * fVar20 * (_DAT_005e5a20 - fVar41 * fVar20 * fVar20)))
        ;
        fVar21 = (float)(-(uint)(fVar22 != 0.0) &
                        (uint)(_UNK_005e5a34 * fVar21 * (_UNK_005e5a24 - fVar22 * fVar21 * fVar21)))
        ;
        fVar23 = (float)(-(uint)(fVar29 != 0.0) &
                        (uint)(_UNK_005e5a38 * fVar23 * (_UNK_005e5a28 - fVar29 * fVar23 * fVar23)))
        ;
        fVar27 = (float)(-(uint)(fVar31 != 0.0) &
                        (uint)(_UNK_005e5a3c * fVar27 * (_UNK_005e5a2c - fVar31 * fVar27 * fVar27)))
        ;
        fVar42 = fVar41 * fVar20;
        fVar28 = fVar22 * fVar21;
        fVar30 = fVar29 * fVar23;
        fVar32 = fVar31 * fVar27;
      }
      pfVar10 = (float *)(piVar13 + 9);
      iVar9 = 2;
      do {
        pfVar10[-0x14] = pfVar10[-0x14] * fVar20;
        pfVar10[-0x13] = pfVar10[-0x13] * fVar20;
        pfVar10[-0x12] = pfVar10[-0x12] * fVar20;
        pfVar10[-0x11] = pfVar10[-0x11];
        pfVar10[-4] = pfVar10[-4] * fVar21;
        pfVar10[-3] = pfVar10[-3] * fVar21;
        pfVar10[-2] = pfVar10[-2] * fVar21;
        pfVar10[-1] = pfVar10[-1];
        pfVar10[0xc] = pfVar10[0xc] * fVar23;
        pfVar10[0xd] = pfVar10[0xd] * fVar23;
        pfVar10[0xe] = pfVar10[0xe] * fVar23;
        pfVar10[0xf] = pfVar10[0xf];
        pfVar10[0x1c] = pfVar10[0x1c] * fVar27;
        pfVar10[0x1d] = pfVar10[0x1d] * fVar27;
        pfVar10[0x1e] = pfVar10[0x1e] * fVar27;
        pfVar10[0x1f] = pfVar10[0x1f];
        pfVar10[-0x10] = pfVar10[-0x10] * fVar20;
        pfVar10[-0xf] = pfVar10[-0xf] * fVar20;
        pfVar10[-0xe] = pfVar10[-0xe] * fVar20;
        pfVar10[-0xd] = pfVar10[-0xd];
        *pfVar10 = *pfVar10 * fVar21;
        pfVar10[1] = pfVar10[1] * fVar21;
        pfVar10[2] = pfVar10[2] * fVar21;
        pfVar10[3] = pfVar10[3];
        pfVar10[0x10] = pfVar10[0x10] * fVar23;
        pfVar10[0x11] = pfVar10[0x11] * fVar23;
        pfVar10[0x12] = pfVar10[0x12] * fVar23;
        pfVar10[0x13] = pfVar10[0x13];
        pfVar10[0x20] = pfVar10[0x20] * fVar27;
        pfVar10[0x21] = pfVar10[0x21] * fVar27;
        pfVar10[0x22] = pfVar10[0x22] * fVar27;
        pfVar10[0x23] = pfVar10[0x23];
        pfVar10 = pfVar10 + 8;
        iVar9 = iVar9 + -1;
      } while (iVar9 != 0);
      pfVar10 = (float *)((int)pfVar12 + (in_stack_00000014 - (int)in_stack_00000020));
      fVar41 = pfVar10[1];
      fVar22 = pfVar10[2];
      fVar29 = pfVar10[3];
      piVar13 = piVar13 + 0x40;
      fVar31 = (float)(_DAT_005e5a70 ^ 0x7f7fffff);
      fVar45 = (float)(_UNK_005e5a74 ^ 0x7f7fffff);
      fVar34 = (float)(_UNK_005e5a78 ^ 0x7f7fffff);
      fVar35 = (float)(_UNK_005e5a7c ^ 0x7f7fffff);
      uVar14 = -(uint)(*pfVar10 != fVar31);
      uVar15 = -(uint)(fVar41 != fVar45);
      uVar16 = -(uint)(fVar22 != fVar34);
      uVar17 = -(uint)(fVar29 != fVar35);
      uVar48 = ~uVar15 & _UNK_005e5a44;
      uVar49 = ~uVar16 & _UNK_005e5a48;
      uVar50 = ~uVar17 & _UNK_005e5a4c;
      pfVar1 = (float *)((int)pfVar12 + (in_stack_00000014 - (int)in_stack_00000020));
      *pfVar1 = *pfVar10 * (float)(uVar14 & (uint)fVar42 | ~uVar14 & _DAT_005e5a40);
      pfVar1[1] = fVar41 * (float)(uVar15 & (uint)fVar28 | uVar48);
      pfVar1[2] = fVar22 * (float)(uVar16 & (uint)fVar30 | uVar49);
      pfVar1[3] = fVar29 * (float)(uVar17 & (uint)fVar32 | uVar50);
      pfVar10 = (float *)((in_stack_0000002c - (int)in_stack_00000020) + (int)pfVar12);
      uVar14 = -(uint)(*pfVar12 != 3.4028235e+38);
      uVar15 = -(uint)(pfVar12[1] != 3.4028235e+38);
      uVar16 = -(uint)(pfVar12[2] != 3.4028235e+38);
      uVar17 = -(uint)(pfVar12[3] != 3.4028235e+38);
      uVar48 = ~uVar15 & _UNK_005e5a44;
      uVar49 = ~uVar16 & _UNK_005e5a48;
      uVar50 = ~uVar17 & _UNK_005e5a4c;
      *pfVar12 = *pfVar12 * (float)(uVar14 & (uint)fVar42 | ~uVar14 & _DAT_005e5a40);
      pfVar12[1] = pfVar12[1] * (float)(uVar15 & (uint)fVar28 | uVar48);
      pfVar12[2] = pfVar12[2] * (float)(uVar16 & (uint)fVar30 | uVar49);
      pfVar12[3] = pfVar12[3] * (float)(uVar17 & (uint)fVar32 | uVar50);
      uVar14 = -(uint)(*pfVar10 != fVar31);
      uVar15 = -(uint)(pfVar10[1] != fVar45);
      uVar16 = -(uint)(pfVar10[2] != fVar34);
      uVar17 = -(uint)(pfVar10[3] != fVar35);
      uVar48 = ~uVar15 & _UNK_005e5a44;
      uVar49 = ~uVar16 & _UNK_005e5a48;
      uVar50 = ~uVar17 & _UNK_005e5a4c;
      *pfVar10 = *pfVar10 * (float)(uVar14 & (uint)fVar42 | ~uVar14 & _DAT_005e5a40);
      pfVar10[1] = pfVar10[1] * (float)(uVar15 & (uint)fVar28 | uVar48);
      pfVar10[2] = pfVar10[2] * (float)(uVar16 & (uint)fVar30 | uVar49);
      pfVar10[3] = pfVar10[3] * (float)(uVar17 & (uint)fVar32 | uVar50);
      pfVar10 = (float *)((in_stack_00000038 - (int)in_stack_00000020) + (int)pfVar12);
      uVar14 = -(uint)(*pfVar10 != 3.4028235e+38);
      uVar15 = -(uint)(pfVar10[1] != 3.4028235e+38);
      uVar16 = -(uint)(pfVar10[2] != 3.4028235e+38);
      uVar17 = -(uint)(pfVar10[3] != 3.4028235e+38);
      uVar48 = ~uVar15 & _UNK_005e5a44;
      uVar49 = ~uVar16 & _UNK_005e5a48;
      uVar50 = ~uVar17 & _UNK_005e5a4c;
      *pfVar10 = *pfVar10 * (float)(uVar14 & (uint)fVar42 | ~uVar14 & _DAT_005e5a40);
      pfVar10[1] = pfVar10[1] * (float)(uVar15 & (uint)fVar28 | uVar48);
      pfVar10[2] = pfVar10[2] * (float)(uVar16 & (uint)fVar30 | uVar49);
      pfVar10[3] = pfVar10[3] * (float)(uVar17 & (uint)fVar32 | uVar50);
      pfVar10 = (float *)((in_stack_00000050 - (int)in_stack_00000020) + (int)pfVar12);
      *pfVar10 = fVar20 * *pfVar10;
      pfVar10[1] = fVar21 * pfVar10[1];
      pfVar10[2] = fVar23 * pfVar10[2];
      pfVar10[3] = fVar27 * pfVar10[3];
      pauVar11 = (undefined1 (*) [16])((in_stack_0000005c - (int)in_stack_00000020) + (int)pfVar12);
      (*(uint *)((char *)&in_XMM2 + 0)) = fVar20 * *(float *)*pauVar11;
      (*(uint *)((char *)&in_XMM2 + 4)) = fVar21 * *(float *)(*pauVar11 + 4);
      (*(uint *)((char *)&in_XMM2 + 8)) = fVar23 * *(float *)(*pauVar11 + 8);
      (*(uint *)((char *)&in_XMM2 + 12)) = fVar27 * *(float *)(*pauVar11 + 0xc);
      *pauVar11 = in_XMM2;
      pfVar10 = (float *)((int)pfVar12 + in_stack_00000044);
      *pfVar10 = fVar33 * fVar20 * fVar20;
      pfVar10[1] = fVar36 * fVar21 * fVar21;
      pfVar10[2] = fVar39 * fVar23 * fVar23;
      pfVar10[3] = fVar18 * fVar27 * fVar27;
      local_f8 = local_f8 + -1;
      pfVar12 = pfVar12 + 4;
    } while (local_f8 != 0);
  }
  return;
}
