// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0043af10: 22.9% (regs 29.7%), raw g2s did not compile (C2050). Raw converter body with compile fixes only: the loop index Ghidra typed as float (fStack_3c, switch on 1.4013e-45 ...) made int, its later float reuse split into fStack_3cf, FUN_0040bb50/70/90 + FUN_004739f0/FUN_00473870 calls rejoined as texture-lookup-then-draw (the Ghidra text folded the draw args into the lookup call). Does NOT compile as C++: Ghidra merged several pushes into wrong callees (FUN_0042b8b0 / FUN_0042b8c0 are 7-byte argless getters of 0x0067ea54 / 0x0067ea56 called with 4 args, FUN_00427e00 called with 2), and the extraout_ST* x87 values are converter artefacts. A real port needs this body rewritten from the listing: register-arg static callees FUN_0042ac00 (edx), FUN_0042ac50 (eax,ecx), FUN_0042aad0 (eax) as in 0x0043d2a0 / 0x0043c5b0. Residual (b): out of budget. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned __int64 ulonglong;
typedef long double float10;
typedef int code();
#define CONCAT13(a,b) ((uint)(((uint)(uchar)(a)<<24)|((uint)(b)&0xffffff)))
#define CONCAT22(a,b) ((uint)(((uint)(ushort)(a)<<16)|(ushort)(b)))
#define CONCAT31(a,b) ((uint)(((uint)(a)<<8)|(uchar)(b)))
MGLOBAL(float, DAT_007f1004, 0x007f1004);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined1, DAT_0067eca0, 0x0067eca0);
MGLOBAL(int, DAT_0067e7f0, 0x0067e7f0);
MGLOBAL(uint, DAT_0067e7f4, 0x0067e7f4);
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBAL(undefined1, DAT_005cc72c, 0x005cc72c);
MGLOBAL(undefined1, DAT_005cd660, 0x005cd660);
MGLOBAL(undefined1, DAT_005cd900, 0x005cd900);
MGLOBAL(int, DAT_0067e844, 0x0067e844);
MGLOBAL(undefined1, DAT_005cda78, 0x005cda78);
MGLOBAL(undefined1, DAT_005cd0a0, 0x005cd0a0);
MGLOBAL(undefined1, DAT_005cc560, 0x005cc560);
MGLOBAL(undefined1, DAT_005cda74, 0x005cda74);
MGLOBAL(undefined1, DAT_005cc9a4, 0x005cc9a4);
MGLOBAL(undefined1, DAT_005cd618, 0x005cd618);
MGLOBAL(undefined1, DAT_005cc574, 0x005cc574);
MGLOBAL(undefined1, DAT_005cc9a0, 0x005cc9a0);
MGLOBAL(int, DAT_0067e9fc, 0x0067e9fc);
MGLOBAL(undefined1, DAT_005cd0bc, 0x005cd0bc);
MGLOBAL(int, DAT_0067ea88, 0x0067ea88);
MGLOBAL(int, DAT_0067ea74, 0x0067ea74);
MGLOBAL(int, DAT_0067f17c, 0x0067f17c);
MGLOBAL(undefined1, DAT_005cda1c, 0x005cda1c);
MGLOBAL(undefined1, DAT_005cc35c, 0x005cc35c);
MGLOBAL(undefined1, DAT_005cd988, 0x005cd988);
MGLOBAL(undefined1, DAT_005cc56c, 0x005cc56c);
MGLOBAL(undefined1, DAT_005cc348, 0x005cc348);
MGLOBAL(undefined1, DAT_005cc32c, 0x005cc32c);
MGLOBAL(undefined1, DAT_005cd5a8, 0x005cd5a8);
MGLOBAL(undefined4, DAT_0067ea94, 0x0067ea94);
MGLOBAL(int, DAT_0067ea78, 0x0067ea78);
MGLOBAL(undefined1, DAT_005cc8f4, 0x005cc8f4);
MGLOBAL(undefined1, DAT_005cd784, 0x005cd784);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005cda14, 0x005cda14);
MGLOBAL(undefined1, DAT_005cd05c, 0x005cd05c);
MGLOBAL(int, DAT_007f1a44, 0x007f1a44);
MGLOBAL(int, DAT_0067ea64, 0x0067ea64);
MGLOBAL(int, DAT_0067eaac, 0x0067eaac);
MGLOBAL(int, DAT_0067eaa0, 0x0067eaa0);
MGLOBAL(int, DAT_0067ea9c, 0x0067ea9c);
MGLOBAL(int, DAT_0067ea98, 0x0067ea98);
MGLOBAL(int, DAT_0067ea90, 0x0067ea90);
MGLOBAL(int, DAT_0067ea7c, 0x0067ea7c);
MGLOBAL(int, DAT_0067ea80, 0x0067ea80);
MGLOBAL(undefined1, DAT_005f6668, 0x005f6668);
MGLOBAL(undefined *, PTR_DAT_005f6710, 0x005f6710);
MGLOBAL(undefined1, DAT_005f72a0, 0x005f72a0);
MGLOBAL(undefined1, DAT_005f7370, 0x005f7370);
MGLOBAL(undefined4, DAT_0067ed18, 0x0067ed18);
MGLOBAL(undefined4, DAT_0067ed38, 0x0067ed38);
MGLOBAL(undefined1, DAT_0067ed3c, 0x0067ed3c);
MGLOBAL(undefined1, DAT_0067ed44, 0x0067ed44);
MGLOBAL(undefined4, DAT_0067ed84, 0x0067ed84);
MGLOBAL(undefined1, DAT_007f0a50, 0x007f0a50);
MGLOBAL(undefined4, DAT_007f0cb0, 0x007f0cb0);
void *FUN_0040bb50(char *p0);
void *FUN_0040bb70(void *p0);
void *FUN_0040bb90(char *p0);
undefined4 FUN_00427e00();
float10 FUN_004282a0(undefined4 p0, float p1);
undefined4 FUN_0042a940(undefined4 p0);
undefined4 FUN_0042a980(undefined4 p0);
void FUN_0042aad0(void);
int __fastcall FUN_0042ac00();
int __fastcall FUN_0042ac50();
undefined2 FUN_0042b8b0(void);
undefined2 FUN_0042b8c0(void);
undefined4 FUN_00430830();
void FUN_004368e0(float p0);
void *FUN_00458630(int p0);
float10 __fastcall FUN_00472650();
void FUN_00473870(void *tex, float x, float y, float w, float h, uint col, int a);
void FUN_004739f0(void *tex, float x, float y, float w, float h, uint col, float u0, float v0, float u1, float v1, int a, int b);
ulonglong FUN_004a2c48(void);
MGLOBAL(int, _DAT_0067eca0, 0x0067eca0);
MGLOBAL(int, _DAT_005cc72c, 0x005cc72c);
MGLOBAL(int, _DAT_005cd660, 0x005cd660);
MGLOBAL(int, _DAT_005cd900, 0x005cd900);
MGLOBAL(int, _DAT_005cda78, 0x005cda78);
MGLOBAL(int, _DAT_005cd0a0, 0x005cd0a0);
MGLOBAL(int, _DAT_005cc560, 0x005cc560);
MGLOBAL(int, _DAT_005cda74, 0x005cda74);
MGLOBAL(int, _DAT_005cc9a4, 0x005cc9a4);
MGLOBAL(int, _DAT_005cd618, 0x005cd618);
MGLOBAL(int, _DAT_005cc574, 0x005cc574);
MGLOBAL(int, _DAT_005cd988, 0x005cd988);
MGLOBAL(int, _DAT_005cc56c, 0x005cc56c);
MGLOBAL(int, _DAT_005cc348, 0x005cc348);
MGLOBAL(int, _DAT_005cc32c, 0x005cc32c);
MGLOBAL(int, _DAT_005cd5a8, 0x005cd5a8);
MGLOBAL(int, _DAT_005cc8f4, 0x005cc8f4);
MGLOBAL(int, _DAT_005cd784, 0x005cd784);
MGLOBAL(int, _DAT_005cda14, 0x005cda14);
MGLOBAL(int, _DAT_005cd05c, 0x005cd05c);
MGLOBAL(int, _DAT_005cc35c, 0x005cc35c);
MGLOBAL(int, _DAT_005cda1c, 0x005cda1c);
MGLOBAL(int, _DAT_005cc9a0, 0x005cc9a0);
MGLOBAL(int, _DAT_005cd0bc, 0x005cd0bc);
// MATCH: 0x0043af10 FUN_0043af10
/* [C1 2026-05-07] Game-setup lobby options renderer */

void FUN_0043af10(void)

{
  float fVar1;
  float fVar2;
  float fVar3;
  uint uVar4;
  undefined1 uVar5;
  short sVar6;
  short sVar7;
  short sVar8;
  int iVar9;
  int iVar10;
  void *uVar11;
  int iVar12;
  int extraout_ECX;
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  uint uVar13;
  int iVar14;
  float10 extraout_ST0;
  float10 extraout_ST0_00;
  float10 extraout_ST0_01;
  float10 fVar15;
  float10 fVar16;
  float10 extraout_ST0_02;
  float10 extraout_ST0_03;
  float10 extraout_ST0_04;
  float10 extraout_ST1;
  float10 extraout_ST1_00;
  float10 extraout_ST1_01;
  float fVar17;
  undefined4 uStack_48;
  float fStack_44;
  undefined4 uStack_40;
  int fStack_3c;
  float fStack_3cf;
  int iStack_38;
  int iStack_30;
  int local_1c [7];
  
  _DAT_0067eca0 = DAT_007f1004 + _DAT_0067eca0;
  local_1c[0] = 0x7f;
  local_1c[1] = 0x79;
  local_1c[2] = 0x7a;
  local_1c[3] = 0x7b;
  local_1c[4] = 0x7c;
  local_1c[5] = 0x7d;
  local_1c[6] = 0x7e;
  (**(code **)(DAT_007d3ff8 + 0x20))(6,0);
  (**(code **)(DAT_007d3ff8 + 0x20))(8,0);
  if (((DAT_0067e7f0 == 0) || (FUN_004368e0(DAT_0067e7f4 & 0xff), DAT_0067e7f0 == 0)) &&
     ((int)DAT_0067e7f4 < 0x60)) {
    (**(code **)(DAT_007d3ff8 + 0x20))(6,1);
    (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
    return;
  }
  uVar13 = DAT_0067e7f4;
  iVar14 = DAT_0067e9f8 * 0x40;
  iVar9 = FUN_0042ac00();
  iVar10 = FUN_0042ac50();
  iStack_38 = *(int *)(&DAT_0067ed3c + iVar14);
  (*(uchar *)((char *)&uStack_48 + 3)) = (undefined1)uVar13;
  uVar4 = uVar13 & 0xff;
  fStack_44 = (float)iVar10 + _DAT_005cc72c;
  if (iStack_38 == 0x18) {
    iVar9 = FUN_0042ac00();
    iVar10 = FUN_0042ac50();
    fStack_44 = ((float)iVar10 + _DAT_005cd660) - _DAT_005cd900;
  }
  if (iStack_38 == 0x12) {
    fStack_44 = fStack_44 - _DAT_005cd900;
  }
  fStack_3c = 1;
  if (1 < iVar9) {
    do {
      iVar10 = DAT_0067e9f8 * 0x10 + (int)fStack_3c;
      if (*(int *)(&DAT_0067ed44 + iVar10 * 4) == 0) {
        FUN_0042aad0();
        FUN_0042aad0();
        iVar10 = extraout_ECX;
      }
      if ((DAT_0067e844 != 0) && ((&DAT_0067ed84)[iVar10] == 0)) {
        FUN_0042aad0();
        FUN_0042aad0();
      }
      if ((int)uVar13 < (int)(uVar4 & 0xff)) {
        uVar4 = uVar13;
      }
      uStack_48 = uVar4 << 0x18;
      sVar6 = FUN_0042b8b0();
      fVar17 = (float)(int)sVar6 * _DAT_005cda78;
      sVar6 = FUN_0042b8c0();
      fVar1 = (fStack_44 - _DAT_005cd0a0) * (float)(int)sVar6 * _DAT_005cc560;
      sVar6 = FUN_0042b8b0();
      fVar2 = (float)(int)sVar6 * _DAT_005cda74;
      sVar6 = FUN_0042b8c0();
      fVar3 = (float)(int)sVar6 * _DAT_005cc9a4;
      if (iStack_38 == 0x18) {
        if ((extraout_EDX != 3) || (iVar10 = FUN_00430830(), iVar10 != 0)) {
          FUN_004739f0(FUN_0040bb50("Arrow"),fVar17,fVar1,fVar2,fVar3,uStack_48,0x3f7ff972,0,0,0x3f800000
                                ,0,1);
        }
        switch(fStack_3c) {
        case 1:
          iStack_30 = 0x59;
          if (DAT_0067ea74 == 1) {
            iStack_30 = 0x5b;
          }
          else if (DAT_0067ea74 == 2) {
            iStack_30 = 0x5c;
          }
          break;
        case 2:
          if (DAT_0067ea90 < 1) {
            DAT_0067ea90 = 1;
            iStack_30 = 0xd4;
          }
          else if (DAT_0067ea90 < 5) {
            if (DAT_0067ea90 == 1) {
              iStack_30 = 0xd4;
            }
            else if (DAT_0067ea90 == 2) {
              iStack_30 = 0xd3;
            }
            else if (DAT_0067ea90 == 3) {
              iStack_30 = 0xd2;
            }
            else if (DAT_0067ea90 == 4) {
              iStack_30 = 0xe3;
            }
          }
          else {
            DAT_0067ea90 = 4;
            iStack_30 = 0xe3;
          }
          break;
        case 3:
          iStack_30 = FUN_0042a940(DAT_0067ea94);
          break;
        case 4:
          iStack_30 = local_1c[DAT_0067ea98];
          break;
        case 5:
          iStack_30 = local_1c[DAT_0067ea9c];
          break;
        case 6:
          iStack_30 = local_1c[DAT_0067eaa0];
          break;
        case 7:
          iStack_30 = 0x25e - (uint)(DAT_0067eaac != 0);
        }
        sVar6 = FUN_0042b8b0(fStack_44,uStack_48,0x3f147ae1,1);
        FUN_00427e00(iStack_30,
                     (float)((extraout_ST0 / (float10)(int)sVar6) * (float10)_DAT_005cd618));
        FUN_004282a0(iStack_30,0x3f147ae1);
        FUN_004a2c48();
        sVar6 = FUN_0042b8b0();
        fVar17 = (fVar17 - (float)(((int)sVar6 * (extraout_EDX_00 + 0x12)) / 0x280)) + _DAT_005cc574
        ;
        if ((fStack_3c != 3) || (iVar10 = FUN_00430830(1), iVar10 != 0)) goto LAB_0043b582;
      }
      else {
        FUN_004739f0(FUN_0040bb50("Arrow"),fVar17,fVar1,fVar2,fVar3,uStack_48,0x3f7ff972,0,0,0x3f800000,0
                              ,1);
        switch(fStack_3c) {
        case 1:
          if (DAT_0067ea74 == 0) {
            iStack_30 = 0x59;
          }
          else if (DAT_0067ea74 == 1) {
            iStack_30 = 0x5b;
          }
          else if (DAT_0067ea74 == 2) {
            iStack_30 = 0x5c;
          }
          break;
        case 2:
          if (DAT_0067ea80 == 0) {
            iStack_30 = 0xe0;
          }
          else if (DAT_0067ea80 == 1) {
            iStack_30 = 0xe1;
          }
          else if (DAT_0067ea80 == 2) {
            iStack_30 = 0xe2;
          }
          break;
        case 3:
          if (DAT_0067ea7c == 0) {
            iStack_30 = 0x59;
          }
          else if (DAT_0067ea7c == 1) {
            iStack_30 = 0xd4;
          }
          else if (DAT_0067ea7c == 2) {
            iStack_30 = 0xd3;
          }
          else if (DAT_0067ea7c == 3) {
            iStack_30 = 0xd2;
          }
          else if (DAT_0067ea7c == 4) {
            iStack_30 = 0xe3;
          }
          break;
        case 4:
          iStack_30 = FUN_0042a940(DAT_0067ea94);
          break;
        case 5:
          iStack_30 = (DAT_0067ea78 != 0) + 0x59;
          break;
        case 6:
          if (DAT_0067ea88 == 0) {
            iStack_30 = 0xec;
          }
          else if (DAT_0067ea88 == 1) {
            iStack_30 = 0x24b;
          }
          else if (DAT_0067ea88 == 2) {
            iStack_30 = 0x141;
          }
        }
        sVar6 = FUN_0042b8b0(fStack_44,uStack_48,0x3f051eb8,1);
        FUN_00427e00(iStack_30,
                     (float)((extraout_ST0_00 / (float10)(int)sVar6) * (float10)_DAT_005cd618));
        FUN_004282a0(iStack_30,0x3f051eb8);
        FUN_004a2c48();
        sVar6 = FUN_0042b8b0();
        fVar17 = (fVar17 - (float)(((int)sVar6 * (extraout_EDX_01 + 0x12)) / 0x280)) + _DAT_005cc574
        ;
LAB_0043b582:
        FUN_00473870(FUN_0040bb50("Arrow"),fVar17,fVar1,fVar2,fVar3,uStack_48,1);
      }
      iStack_38 = *(int *)(&DAT_0067ed3c + DAT_0067e9f8 * 0x40);
      fVar17 = _DAT_005cc72c;
      if (iStack_38 == 0x18) {
        fVar17 = _DAT_005cd660;
      }
      fStack_44 = fStack_44 + fVar17;
      fStack_3c = fStack_3c + 1;
      (*(uchar *)((char *)&uStack_48 + 3)) = (undefined1)DAT_0067e7f4;
      uVar13 = DAT_0067e7f4;
      uVar4 = DAT_0067e7f4;
    } while ((int)fStack_3c < iVar9);
  }
  uVar5 = (*(uchar *)((char *)&uStack_48 + 3));
  uStack_48 = CONCAT13((*(uchar *)((char *)&uStack_48 + 3)),0xffff00);
  uStack_48 = CONCAT31(((*(uint *)((char *)&uStack_48 + 1)) & 0xffffff),0xff);
  FUN_0042b8b0();
  FUN_0042b8c0();
  FUN_0042b8b0();
  FUN_0042b8c0();
  if (DAT_0067e9fc == 6) {
    if (DAT_0067ea88 != 0) {
      if (DAT_0067ea88 == 1) {
        FUN_00427e00(0x24b,0x43a50000,0x439e0000,uStack_48,0x3f19999a,0);
        FUN_00427e00(0x148,0x43a50000,0x43a70000,uStack_48,0x3f0ccccd,0);
        FUN_00427e00(0x149,0x43a50000,0x43af0000,uStack_48,0x3f0ccccd,0);
      }
      else if (DAT_0067ea88 == 2) {
        FUN_00427e00(0x141,0x43a50000,0x439e0000,uStack_48,0x3f19999a,0);
        FUN_00427e00(0x14a,0x43a50000,0x43a70000,uStack_48,0x3f0ccccd,0);
        FUN_00427e00(0x14b,0x43a50000,0x43af0000,uStack_48,0x3f0ccccd,0);
      }
      goto LAB_0043b939;
    }
    FUN_00427e00(0xec,0x43a50000,0x439e0000,uStack_48,0x3f19999a,0);
    if ((DAT_007f1a44 == -1) || (DAT_0067ea64 != 0)) {
      FUN_00427e00(300,0x43a50000,0x43a70000,uStack_48,0x3f19999a,0);
      uVar11 = 0x12d;
    }
    else {
      FUN_00427e00(0x124,0x43a50000,0x43a70000,uStack_48,0x3f19999a,0);
      uVar11 = 0x125;
    }
  }
  else {
    if (DAT_0067e9fc == 7) {
      uVar11 = 0x142;
    }
    else {
      if (DAT_0067e9fc != 8) goto LAB_0043b939;
      uVar11 = 0x143;
    }
    FUN_00427e00(uVar11,0x43a50000,0x439e0000,uStack_48,0x3f19999a,0);
    if (DAT_0067ea88 == 0) {
      if ((DAT_007f1a44 == -1) || (DAT_0067ea64 != 0)) {
        FUN_00427e00(300,0x43a50000,0x43a70000,uStack_48,0x3f19999a,0);
      }
      else {
        FUN_00427e00(0x124,0x43a50000,0x43a70000,uStack_48,0x3f19999a,0);
      }
      goto LAB_0043b939;
    }
    if (DAT_0067ea88 == 1) {
      FUN_00427e00(0x148,0x43a50000,0x43a70000,uStack_48,0x3f19999a,0);
      uVar11 = 0x149;
    }
    else {
      if (DAT_0067ea88 != 2) goto LAB_0043b939;
      FUN_00427e00(0x14a,0x43a50000,0x43a70000,uStack_48,0x3f19999a,0);
      uVar11 = 0x14b;
    }
  }
  FUN_00427e00(uVar11,0x43a50000,0x43af0000,uStack_48,0x3f19999a,0);
LAB_0043b939:
  uStack_40 = CONCAT13(uVar5,0xff0000);
  uStack_40 = CONCAT22((*(ushort *)((char *)&uStack_40 + 2)),0xff00);
  uStack_40 = CONCAT31(((*(uint *)((char *)&uStack_40 + 1)) & 0xffffff),0xff);
  if ((DAT_0067ea74 != 0) && (*(int *)(&DAT_007f0a50 + DAT_0067f17c * 0x30) != 0)) {
    iVar14 = 0;
    iVar10 = 0;
    iVar9 = DAT_0067f17c;
    do {
      FUN_0042b8b0();
      FUN_0042b8c0();
      FUN_0042b8b0();
      if (DAT_0067ea74 == 1) {
        if (-1 < (int)(&DAT_007f0cb0)[iVar14 + iVar9 * 5]) {
          iVar12 = FUN_00458630((&DAT_007f0cb0)[iVar14 + iVar9 * 5]);
          fStack_3cf = 0.0;
LAB_0043ba05:
          iVar9 = DAT_0067f17c;
          if (iVar12 != 0) {
            sVar6 = FUN_0042b8b0();
            fVar1 = (float)(int)sVar6 * _DAT_005cd988;
            sVar6 = FUN_0042b8c0();
            fVar2 = (float)(int)sVar6 * _DAT_005cc56c;
            fVar17 = (float)((float10)fStack_3cf * (float10)_DAT_005cc348);
            fVar15 = (float10)fStack_3cf * (float10)_DAT_005cc348 * (float10)_DAT_005cc32c;
            sVar6 = FUN_0042b8b0();
            fVar3 = (float)(int)sVar6 * fVar17 * _DAT_005cd5a8;
            sVar6 = FUN_0042b8c0();
            FUN_00473870(FUN_0040bb90("Powerupshadow"),(float)(fVar15 + extraout_ST1),
                                  (float)(fVar15 + extraout_ST0_01),fVar1 - fVar3,
                                  fVar2 - (float)(int)sVar6 * fVar17 * _DAT_005cc560,uStack_40,0);
            FUN_0042b8b0();
            fVar16 = (float10)FUN_0042b8c0();
            sVar6 = FUN_0042b8b0();
            FUN_0042b8b0();
            FUN_0042b8c0();
            sVar7 = FUN_0042b8b0();
            fVar15 = (float10)_DAT_005cd5a8;
            sVar8 = FUN_0042b8c0();
            FUN_00473870(iVar12,(float)((fVar16 - (float10)((sVar6 * iVar14 * 0x26) / 0x280)) +
                                       extraout_ST0_02),(float)(extraout_ST0_02 + extraout_ST1_00),
                         (float)(extraout_ST1_01 - (float10)(int)sVar7 * (float10)fStack_3cf * fVar15
                                ),(float)(extraout_ST0_03 -
                                         (float10)(int)sVar8 * (float10)fStack_3cf *
                                         (float10)_DAT_005cc560),uStack_40,0);
            iVar9 = DAT_0067f17c;
          }
        }
      }
      else if (DAT_0067ea74 == 2) {
        iVar9 = FUN_004a2c48();
        fVar15 = (float10)fsin((float10)(iVar10 - iVar9) * (float10)_DAT_005cc8f4);
        fVar15 = fVar15 * (float10)_DAT_005cd784;
        if (fVar15 < (float10)DAT_005d757c) {
          fVar15 = (float10)DAT_005d757c;
        }
        fStack_3cf = (float)(fVar15 * (float10)_DAT_005cda14);
        if ((float10)_DAT_005cd05c <= fVar15 * (float10)_DAT_005cda14) {
          FUN_00472650(0,0x41700000);
          uVar11 = FUN_004a2c48();
          (&DAT_0067ed18)[iVar14] = uVar11;
        }
        iVar12 = FUN_00458630(*(undefined4 *)(&DAT_005f6668 + ((&DAT_0067ed18)[iVar14] & 0xf) * 4));
        goto LAB_0043ba05;
      }
      iVar14 = iVar14 + 1;
      iVar10 = iVar10 + 0x1e;
    } while (iVar14 < 5);
  }
  if ((undefined *)(&DAT_0067ed38)[DAT_0067e9f8 * 0x10] == &DAT_005f7370) {
    iVar9 = FUN_0042a980(DAT_0067ea94);
    FUN_004739f0(FUN_0040bb70((&PTR_DAT_005f6710)[iVar9]),0x43e40000,0x43100000,0x43180000,0x43180000,uStack_40,0,0x3f800000,0,
                 0x3f800000,1,0);
  }
  if ((undefined *)(&DAT_0067ed38)[DAT_0067e9f8 * 0x10] == &DAT_005f72a0) {
    iVar9 = FUN_0042a980(DAT_0067ea94);
    FUN_004739f0(FUN_0040bb70((&PTR_DAT_005f6710)[iVar9]),0x43e40000,0x43100000,0x43180000,0x43180000,uStack_40,0,0x3f800000,0,
                 0x3f800000,1,0);
  }
  if (DAT_0067ea78 != 0) {
    FUN_0042b8b0();
    sVar6 = FUN_0042b8c0();
    fVar15 = (float10)_DAT_005cc35c;
    fVar17 = (float)(int)sVar6 * _DAT_005cda1c - _DAT_005cc35c;
    sVar6 = FUN_0042b8b0();
    fVar1 = (float)(int)sVar6 * _DAT_005cd988;
    sVar6 = FUN_0042b8c0();
    FUN_00473870(FUN_0040bb90("Powerupshadow"),(float)(extraout_ST0_04 - fVar15),fVar17,fVar1,
                          (float)(int)sVar6 * _DAT_005cc56c,uStack_40,0);
    fVar1 = (float)(extraout_ST0_04 - fVar15) + _DAT_005cc35c;
    fVar17 = fVar17 + _DAT_005cc35c;
    sVar6 = FUN_0042b8b0();
    fVar2 = (float)(int)sVar6 * _DAT_005cc9a0;
    sVar6 = FUN_0042b8c0();
    FUN_00473870(FUN_00458630(0x16),fVar1,fVar17,fVar2,(float)(int)sVar6 * _DAT_005cd0bc,uStack_40,0);
  }
  (**(code **)(DAT_007d3ff8 + 0x20))(6,1);
  (**(code **)(DAT_007d3ff8 + 0x20))(8,1);
  return;
}
