// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x00534d00: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
typedef int code();
MGLOBAL(undefined1, FUN_00535690, 0x00535690);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007dc57c, 0x007dc57c);
undefined4 FUN_004e6920(int p0);
int * FUN_004e8090(int * p0, int p1);
undefined4 FUN_00535330(int * p0, uint p1);
undefined1 * RpAtomicCreate(void);
// MATCH: 0x00534d00 FUN_00534d00
/* [C1 2026-05-08] int FUN_00534d00(int param_1, uint param_2, int param_3, int *param_4). */

int FUN_00534d00(int param_1,uint param_2,int param_3,int *param_4)

{
  int iVar1;
  code *pcVar2;
  byte bVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  bool bVar10;
  bool bVar11;
  bool bVar12;
  bool bVar13;
  bool bVar14;
  bool bVar15;
  bool bVar16;
  bool bVar17;
  bool bVar18;
  
  iVar4 = RpAtomicCreate();
  iVar9 = 0;
  if ((iVar4 == 0) ||
     (piVar5 = (int *)(**(code **)(DAT_007d3ff8 + 0x108))(0x138,0x3012f), piVar5 == (int *)0x0))
  goto LAB_0053530e;
  piVar5[4] = *(int *)(iVar4 + 0x48);
  *(code **)(iVar4 + 0x48) = FUN_00535690;
  *piVar5 = param_1;
  piVar5[1] = 0;
  piVar5[0x28] = param_2;
  piVar5[0x11] = param_3;
  piVar5[0x10] = -1;
  piVar5[0xf] = 0;
  piVar5[3] = 0;
  *(int **)(DAT_007dc57c + iVar4) = piVar5;
  piVar5[5] = *param_4;
  piVar5[6] = param_4[1];
  piVar5[7] = param_4[2];
  piVar5[8] = param_4[3];
  pcVar2 = (code *)piVar5[5];
  if (pcVar2 == (code *)0x0) {
    if ((param_2 & 0x20000000) == 0x20000000) {
      bVar3 = (byte)param_2;
      bVar10 = (bVar3 & 1) == 1;
      bVar11 = (bVar3 & 2) == 2;
      bVar12 = (bVar3 & 4) == 4;
      bVar13 = (bVar3 & 8) == 8;
      bVar14 = (bVar3 & 0x10) == 0x10;
      bVar15 = (bVar3 & 0x20) == 0x20;
      bVar16 = (bVar3 & 0x40) == 0x40;
      bVar17 = (bVar3 & 0x80) == 0x80;
      bVar18 = (param_2 & 0x100) == 0x100;
      uVar8 = piVar5[0x28];
      iVar6 = 0;
      piVar5[0x28] = uVar8 & 0xfffffe00;
      piVar5[0x26] = 0;
      if (bVar10) {
        piVar5[0x26] = 1;
        piVar5[0x28] = uVar8 & 0xfffffe00 | 1;
        iVar6 = 0xc;
      }
      if (bVar13) {
        piVar5[0x26] = piVar5[0x26] + 1;
        iVar6 = iVar6 + 0x40;
        piVar5[0x28] = piVar5[0x28] | 8;
      }
      if (bVar14) {
        piVar5[0x26] = piVar5[0x26] + 1;
        iVar6 = iVar6 + 0xc;
        piVar5[0x28] = piVar5[0x28] | 0x10;
      }
      if (bVar12) {
        piVar5[0x26] = piVar5[0x26] + 1;
        iVar6 = iVar6 + 8;
        piVar5[0x28] = piVar5[0x28] | 4;
      }
      if (bVar11) {
        piVar5[0x26] = piVar5[0x26] + 1;
        iVar6 = iVar6 + 4;
        piVar5[0x28] = piVar5[0x28] | 2;
      }
      if (bVar16) {
        piVar5[0x26] = piVar5[0x26] + 1;
        iVar6 = iVar6 + 0x10;
        piVar5[0x28] = piVar5[0x28] | 0x40;
      }
      if (bVar15) {
        piVar5[0x26] = piVar5[0x26] + 1;
        iVar6 = iVar6 + 4;
        piVar5[0x28] = piVar5[0x28] | 0x20;
      }
      if (bVar17) {
        piVar5[0x26] = piVar5[0x26] + 1;
        iVar6 = iVar6 + 0x10;
        piVar5[0x28] = piVar5[0x28] | 0x80;
      }
      if (bVar18) {
        piVar5[0x26] = piVar5[0x26] + 1;
        piVar5[0x28] = piVar5[0x28] | 0x100;
        iVar6 = iVar6 + 0x20;
      }
      piVar5[0x27] = iVar6;
      iVar6 = (**(code **)(DAT_007d3ff8 + 0x108))(*piVar5 * iVar6 + 0x10,0x30000);
      if (iVar6 != 0) {
        iVar9 = 1;
        uVar8 = iVar6 + 0xfU & 0xfffffff0;
        iVar7 = 0;
        if (bVar10) {
          piVar5[0x13] = uVar8;
          piVar5[0x14] = piVar5[0x27];
          iVar7 = 0xc;
        }
        if (bVar13) {
          iVar1 = uVar8 + iVar7;
          iVar7 = iVar7 + 0x40;
          piVar5[0x15] = iVar1;
          piVar5[0x16] = piVar5[0x27];
        }
        if (bVar14) {
          iVar1 = uVar8 + iVar7;
          iVar7 = iVar7 + 0xc;
          piVar5[0x17] = iVar1;
          piVar5[0x18] = piVar5[0x27];
        }
        if (bVar12) {
          iVar1 = uVar8 + iVar7;
          iVar7 = iVar7 + 8;
          piVar5[0x19] = iVar1;
          piVar5[0x1a] = piVar5[0x27];
        }
        if (bVar11) {
          iVar1 = uVar8 + iVar7;
          iVar7 = iVar7 + 4;
          piVar5[0x1b] = iVar1;
          piVar5[0x1c] = piVar5[0x27];
        }
        if (bVar16) {
          iVar1 = uVar8 + iVar7;
          iVar7 = iVar7 + 0x10;
          piVar5[0x1d] = iVar1;
          piVar5[0x1e] = piVar5[0x27];
        }
        if (bVar15) {
          iVar1 = uVar8 + iVar7;
          iVar7 = iVar7 + 4;
          piVar5[0x1f] = iVar1;
          piVar5[0x20] = piVar5[0x27];
        }
        if (bVar17) {
          iVar1 = uVar8 + iVar7;
          iVar7 = iVar7 + 0x10;
          piVar5[0x21] = iVar1;
          piVar5[0x22] = piVar5[0x27];
        }
        if (bVar18) {
          piVar5[0x23] = uVar8 + iVar7;
          piVar5[0x24] = piVar5[0x27];
        }
        piVar5[3] = iVar6;
      }
    }
    else {
      iVar9 = FUN_00535330(piVar5,param_2,param_3);
    }
    if (iVar9 != 1) goto LAB_00535124;
  }
  else {
    iVar9 = (*pcVar2)(piVar5 + 0x12,*piVar5,param_2,param_3);
    piVar5[3] = iVar9;
LAB_00535124:
    if (piVar5[3] == 0) goto LAB_0053530e;
  }
  iVar9 = (*(code *)piVar5[6])(iVar4,piVar5 + 0x12,*piVar5,param_2,param_3);
  if (iVar9 == 1) {
    FUN_004e8090(**(undefined4 **)(*(int *)(iVar4 + 0x18) + 0x20),0);
    piVar5[0x2c] = 0;
    piVar5[0x2e] = 0x3f800000;
    piVar5[0x48] = 0x3f800000;
    piVar5[0x43] = 0x3f800000;
    piVar5[0x3e] = 0x3f800000;
    piVar5[0x2f] = 0x3f800000;
    piVar5[0x41] = piVar5[0x41] | 0x20003;
    piVar5[0x31] = -0x1000000;
    piVar5[0x32] = -0x1000000;
    piVar5[0x33] = -0x1000000;
    piVar5[0x34] = -0x1000000;
    piVar5[0x35] = -0x1000000;
    piVar5[0x2d] = 0;
    piVar5[0x36] = 0;
    piVar5[0x37] = 0;
    piVar5[0x38] = 0;
    piVar5[0x39] = 0x3f800000;
    piVar5[0x3a] = 0x3f800000;
    piVar5[0x3b] = 0x3f800000;
    piVar5[0x30] = 0;
    piVar5[0x42] = 0;
    piVar5[0x40] = 0;
    piVar5[0x3f] = 0;
    piVar5[0x47] = 0;
    piVar5[0x46] = 0;
    piVar5[0x44] = 0;
    piVar5[0x4c] = 0;
    piVar5[0x4b] = 0;
    piVar5[0x4a] = 0;
    piVar5[0x3c] = 0x3f800000;
    piVar5[0x3d] = 0;
    (**(code **)(DAT_007d3ff8 + 0x24))(10,piVar5 + 0x29);
    (**(code **)(DAT_007d3ff8 + 0x24))(0xb,piVar5 + 0x2a);
    (**(code **)(DAT_007d3ff8 + 0x24))(0xc,piVar5 + 0x2b);
    return iVar4;
  }
LAB_0053530e:
  FUN_004e6920(iVar4);
  return 0;
}
