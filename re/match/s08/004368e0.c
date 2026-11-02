// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x004368e0 (10415 B): best 25.1% (regs 25.0%, ops 36.0%), raw g2s.py body with the minimum fixes
   (raw did not compile; raw-but-fixed baseline 12.0%). Fixes: the two C2106 byte-field writes to local_6c written
   as mask/or; the parameter is a uchar alpha (`mov bl, [esp+0x74]`; Ghidra typed it float because the compiler
   reuses its stack slot for a float later, now the local `pf`); 0x0042b8b0 / 0x0042b8c0 are static callees of
   this TU (the original keeps x87 values live across them, same as 0x00439210), file holds both at 100%.
   NOT done (60 min cap reached; structure work still owed): the {x,y} struct-by-value form for
   0x00472c60 / 0x004739f0 (the store-then-reload pattern; a per-call-temporary conversion was tried and scored
   lower, 15-18%, because the original reuses a few persistent pos/size locals), the raw converter's int bit
   patterns passed to float parameters (0x42000000 etc., value-wrong), the goto structure, and the Powerupshadow
   call Ghidra misparsed. Class (b).
   C++ check FAILS (20 errors, all raw-converter artefacts: Ghidra arities for 0x0040bb50/0x0040bb90/0x00473870/
   0x00472650/sprintf 0x004a2b60, and the fsin pseudo-op). This file is a scoring baseline, not a port body yet. */
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
typedef unsigned int undefined3;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned char byte;
typedef __int64 longlong;
typedef unsigned __int64 ulonglong;
typedef long double float10;
#define CONCAT13(a,b) ((uint)(((uint)(uchar)(a)<<24)|((uint)(b)&0xffffff)))
MGLOBAL(float, DAT_007f1004, 0x007f1004);
MGLOBAL(uint, DAT_00616038, 0x00616038);
MGLOBAL(undefined1, DAT_0067ea60, 0x0067ea60);
MGLOBAL(undefined1, DAT_005cc9a0, 0x005cc9a0);
MGLOBAL(undefined1, DAT_005cd0bc, 0x005cd0bc);
MGLOBAL(int, DAT_0067e9fc, 0x0067e9fc);
MGLOBAL(int, DAT_0067f17c, 0x0067f17c);
MGLOBAL(undefined1, DAT_005cda1c, 0x005cda1c);
MGLOBAL(undefined1, DAT_005cc35c, 0x005cc35c);
MGLOBAL(undefined1, DAT_005cd988, 0x005cd988);
MGLOBAL(undefined1, DAT_005cc56c, 0x005cc56c);
MGLOBAL(undefined1, DAT_005cc348, 0x005cc348);
MGLOBAL(undefined1, DAT_005cc32c, 0x005cc32c);
MGLOBAL(undefined1, DAT_005cd5a8, 0x005cd5a8);
MGLOBAL(undefined1, DAT_005cc560, 0x005cc560);
MGLOBAL(int, DAT_00898ab0, 0x00898ab0);
MGLOBAL(undefined1, DAT_005cc9f4, 0x005cc9f4);
MGLOBAL(undefined1, DAT_005cd110, 0x005cd110);
MGLOBAL(undefined1, DAT_005cd9fc, 0x005cd9fc);
MGLOBAL(undefined1, DAT_005cd9f8, 0x005cd9f8);
MGLOBAL(undefined1, DAT_005cd9f4, 0x005cd9f4);
MGLOBAL(undefined1, DAT_005cd9f0, 0x005cd9f0);
MGLOBAL(undefined1, DAT_005cc738, 0x005cc738);
MGLOBAL(undefined1, DAT_005cd9ec, 0x005cd9ec);
MGLOBAL(undefined1, DAT_005cc574, 0x005cc574);
MGLOBAL(int, DAT_00899140, 0x00899140);
MGLOBAL(int, DAT_0067f180, 0x0067f180);
MGLOBAL(undefined1, DAT_005cc8f4, 0x005cc8f4);
MGLOBAL(undefined1, DAT_005cd784, 0x005cd784);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005cda14, 0x005cda14);
MGLOBAL(undefined1, DAT_005cd05c, 0x005cd05c);
MGLOBAL(int, DAT_007f1a14, 0x007f1a14);
MGLOBAL(int, DAT_007f1a24, 0x007f1a24);
MGLOBAL(int, DAT_007f1a34, 0x007f1a34);
MGLOBAL(int, DAT_007f1a44, 0x007f1a44);
MGLOBAL(uint, DAT_0067ea64, 0x0067ea64);
MGLOBAL(undefined4, DAT_007f1a1c, 0x007f1a1c);
MGLOBAL(undefined4, DAT_007f1a2c, 0x007f1a2c);
MGLOBAL(int, DAT_007f1a18, 0x007f1a18);
MGLOBAL(int, DAT_007f1a28, 0x007f1a28);
MGLOBAL(int, DAT_007f1a38, 0x007f1a38);
MGLOBAL(int, DAT_007f1a48, 0x007f1a48);
MGLOBAL(int, DAT_0067ea7c, 0x0067ea7c);
MGLOBAL(undefined4, DAT_007f1a3c, 0x007f1a3c);
MGLOBAL(undefined4, DAT_007f1a4c, 0x007f1a4c);
MGLOBAL(undefined1, DAT_005cda38, 0x005cda38);
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBAL(int, DAT_0067ea98, 0x0067ea98);
MGLOBAL(int, DAT_0067ea9c, 0x0067ea9c);
MGLOBAL(int, DAT_0067eaa0, 0x0067eaa0);
MGLOBAL(undefined1, DAT_005cda34, 0x005cda34);
MGLOBAL(uint, DAT_0067eaac, 0x0067eaac);
MGLOBAL(undefined1, DAT_005cd794, 0x005cd794);
MGLOBAL(undefined1, DAT_005cd798, 0x005cd798);
MGLOBAL(undefined1, DAT_005cd7a0, 0x005cd7a0);
MGLOBAL(undefined1, DAT_005cd9c4, 0x005cd9c4);
MGLOBAL(undefined1, DAT_005cda30, 0x005cda30);
MGLOBAL(undefined1, DAT_005f65c8, 0x005f65c8);
MGLOBAL(undefined1, DAT_005f6668, 0x005f6668);
MGLOBAL(undefined1, DAT_005f7370, 0x005f7370);
MGLOBAL(undefined4, DAT_0067ed38, 0x0067ed38);
MGLOBAL(undefined4, DAT_007f0a40, 0x007f0a40);
MGLOBAL(undefined4, DAT_007f0a74, 0x007f0a74);
MGLOBAL(undefined4, DAT_007f0cb0, 0x007f0cb0);
MGLOBAL(undefined4, DAT_007f0db4, 0x007f0db4);
MGLOBAL(undefined4, DAT_007f0de8, 0x007f0de8);
undefined4 FUN_0040bb50(undefined4 p0);
undefined4 FUN_0040bb90(const void *p0);
void FUN_00427e00(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, float p4, undefined4 p5);
void FUN_00427f00(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, float p4, undefined4 p5);
void FUN_0042bf30(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5);
void FUN_0042e8b0(int p0);
void FUN_0042ed70(undefined4 p0, float p1, float p2, undefined4 p3, undefined4 p4, undefined4 p5);
undefined4 FUN_0042ef40(int p0, int p1);
undefined4 FUN_0042fab0(undefined4 p0);
int FUN_00430670(int p0);
undefined4 FUN_00430760(void);
int FUN_004309b0(void);
undefined4 FUN_00458630(undefined4 p0);
float10 __fastcall FUN_00472650();
void FUN_00472c60(float p0, float p1, float p2, float p3, uint p4);
void FUN_00473870();
undefined4 FUN_004739f0(undefined4 p0, float p1, float p2, float p3, float p4, uint p5, undefined4 p6, undefined4 p7, undefined4 p8, undefined4 p9, int p10, int p11);
undefined4 FUN_004a2b60(char * p0, undefined4 p1);
ulonglong FUN_004a2c48(void);
MGLOBAL(int, _DAT_0067ea60, 0x0067ea60);
MGLOBAL(int, _DAT_005cda38, 0x005cda38);
MGLOBAL(int, _DAT_005cc9a0, 0x005cc9a0);
MGLOBAL(int, _DAT_005cda34, 0x005cda34);
MGLOBAL(int, _DAT_005cc8f4, 0x005cc8f4);
MGLOBAL(int, _DAT_005cd784, 0x005cd784);
MGLOBAL(int, _DAT_005cda14, 0x005cda14);
MGLOBAL(int, _DAT_005cd05c, 0x005cd05c);
MGLOBAL(int, _DAT_005cd988, 0x005cd988);
MGLOBAL(int, _DAT_005cc56c, 0x005cc56c);
MGLOBAL(int, _DAT_005cc348, 0x005cc348);
MGLOBAL(int, _DAT_005cc32c, 0x005cc32c);
MGLOBAL(int, _DAT_005cd5a8, 0x005cd5a8);
MGLOBAL(int, _DAT_005cc560, 0x005cc560);
MGLOBAL(int, _DAT_005cc9f4, 0x005cc9f4);
MGLOBAL(int, _DAT_005cd110, 0x005cd110);
MGLOBAL(int, _DAT_005cc35c, 0x005cc35c);
MGLOBAL(int, _DAT_005cd9fc, 0x005cd9fc);
MGLOBAL(int, _DAT_005cd9f8, 0x005cd9f8);
MGLOBAL(int, _DAT_005cd9f0, 0x005cd9f0);
MGLOBAL(int, _DAT_005cd9f4, 0x005cd9f4);
MGLOBAL(int, _DAT_005cd9ec, 0x005cd9ec);
MGLOBAL(int, _DAT_005cc738, 0x005cc738);
MGLOBAL(int, _DAT_005cc574, 0x005cc574);
MGLOBAL(short, DAT_0067ea54, 0x0067ea54);
MGLOBAL(short, DAT_0067ea56, 0x0067ea56);
// MATCH: 0x0042b8b0 FUN_0042b8b0
static short FUN_0042b8b0(void)
{
  return DAT_0067ea54;
}

// MATCH: 0x0042b8c0 FUN_0042b8c0
static short FUN_0042b8c0(void)
{
  return DAT_0067ea56;
}

// MATCH: 0x004368e0 FUN_004368e0
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* [C1 2026-05-11] Adds `DAT_007f1004` to `_DAT_0067ea60` at entry (0x004368e5); accumulates a
   frame-counter or timer. */

void FUN_004368e0(uchar a)

{
  float pf;
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  byte bVar5;
  short sVar6;
  short sVar7;
  short sVar8;
  int iVar9;
  undefined4 uVar10;
  uint uVar11;
  int iVar12;
  int iVar13;
  int iVar14;
  int iVar15;
  int iVar16;
  uint uVar17;
  uint uVar18;
  int iVar19;
  undefined4 uVar20;
  char cVar21;
  float10 extraout_ST0;
  float10 fVar22;
  float10 fVar23;
  float10 extraout_ST0_00;
  float10 extraout_ST0_01;
  float10 extraout_ST1;
  float10 extraout_ST1_00;
  float10 extraout_ST1_01;
  uint unaff_retaddr;
  undefined4 uVar24;
  undefined4 local_6c;
  undefined4 local_68;
  undefined4 local_64;
  undefined4 local_60;
  undefined4 local_5c;
  float local_54;
  float local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined1 auStack_44 [8];
  undefined1 auStack_3c [12];
  int iStack_30;
  int iStack_2c;
  int iStack_1c;
  undefined1 auStack_14 [16];
  uint local_4;
  
  _DAT_0067ea60 = DAT_007f1004 + _DAT_0067ea60;
  local_4 = DAT_00616038 ^ unaff_retaddr;
  FUN_0042e8b0(a);
  local_64 = (float)((uint)(byte)((ulonglong)
                                  ((longlong)(int)((a) * 2) * 0x55555556) >> 0x20
                                 ) << 0x18);
  FUN_00472c60(0x42000000,0x43960000,0x44100000,0x42c00000,local_64);
  local_68 = CONCAT13(a,0xffffff);
  FUN_00472c60(0x439f8000,0x439a0000,0x40000000,0x42a00000,local_68);
  local_6c = CONCAT13(a,0xffffff);
  FUN_0042b8b0();
  FUN_0042b8c0();
  FUN_0042b8b0();
  FUN_0042b8c0();
  uVar17 = 0;
  local_68 = 0;
  iVar9 = FUN_00430760();
  if (iVar9 == 0) {
LAB_004375d5:
    cVar21 = DAT_007f1a14 != -1;
    if (DAT_007f1a24 != -1) {
      cVar21 = cVar21 + '\x01';
    }
    if (DAT_007f1a34 != -1) {
      cVar21 = cVar21 + '\x01';
    }
    if (DAT_007f1a44 != -1) {
      cVar21 = cVar21 + '\x01';
    }
    if (DAT_0067ea64 == 0) {
      if (cVar21 == '\x02') {
        uVar20 = FUN_0042fab0(DAT_007f1a1c);
        FUN_004739f0(uVar20,0x42200000,0x43920000,0x42e00000,0x42e00000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,1);
        local_54 = 196.0;
        if (DAT_0067ea7c != 0) {
          local_54 = 148.0;
        }
        uVar20 = FUN_0042fab0(DAT_007f1a2c);
        FUN_004739f0(uVar20,local_54,0x43920000,0x42e00000,0x42e00000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,0);
        local_54 = 132.0;
        if (DAT_0067ea7c != 0) {
          local_54 = 108.0;
        }
        uVar20 = FUN_0040bb90(&DAT_005cda30);
        FUN_004739f0(uVar20,local_54,0x43960000,0x42b00000,0x42b00000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,1);
        if (DAT_0067ea7c != 0) {
          local_60 = 0x42400000;
          uVar20 = FUN_0040bb90("question");
          FUN_004739f0(uVar20,0x43830000,0x43980000,0x42400000,0x42400000,local_6c,0,0x3f800000,0,
                       0x3f800000,1,1);
          FUN_004739f0(uVar20,0x43830000,0x43ae0000,0x42400000,0x42400000,local_6c,0,0x3f800000,0,
                       0x3f800000,1,1);
          local_54 = 232.0;
          local_50 = 324.0;
          uVar20 = FUN_0040bb90(&DAT_005cda30);
          local_5c = 0x42400000;
          goto LAB_00437b6a;
        }
      }
      else if (cVar21 == '\x03') {
        uVar20 = FUN_0042fab0(DAT_007f1a1c);
        FUN_004739f0(uVar20,0x42200000,0x439a0000,0x42a00000,0x42a00000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,1);
        local_54 = 136.0;
        if (DAT_0067ea7c != 0) {
          local_54 = 112.0;
        }
        uVar20 = FUN_0042fab0(DAT_007f1a2c);
        FUN_004739f0(uVar20,local_54,0x439a0000,0x42a00000,0x42a00000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,0);
        local_54 = 232.0;
        if (DAT_0067ea7c != 0) {
          local_54 = 184.0;
        }
        uVar20 = FUN_0042fab0(DAT_007f1a3c);
        FUN_004739f0(uVar20,local_54,0x439a0000,0x42a00000,0x42a00000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,0);
        local_54 = 100.0;
        if (DAT_0067ea7c != 0) {
          local_54 = 88.0;
        }
        uVar20 = FUN_0040bb90(&DAT_005cda30);
        FUN_004739f0(uVar20,local_54,0x439e0000,0x42800000,0x42800000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,1);
        local_54 = 196.0;
        if (DAT_0067ea7c != 0) {
          local_54 = 160.0;
        }
        FUN_004739f0(uVar20,local_54,0x439e0000,0x42800000,0x42800000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,1);
        if (DAT_0067ea7c != 0) {
          uVar20 = FUN_0040bb90("question");
          FUN_004739f0(uVar20,0x43830000,0x439e0000,0x42800000,0x42800000,local_6c,0,0x3f800000,0,
                       0x3f800000,1,1);
          local_5c = 0x42400000;
          local_60 = 0x42400000;
          local_54 = 232.0;
          local_50 = 324.0;
          goto LAB_00438996;
        }
      }
      else if (cVar21 == '\x04') {
        uVar20 = FUN_0042fab0(DAT_007f1a1c);
        FUN_004739f0(uVar20,0x42200000,0x439a0000,0x42900000,0x42900000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,1);
        uVar20 = FUN_0042fab0(DAT_007f1a2c);
        FUN_004739f0(uVar20,0x42d00000,0x439a0000,0x42900000,0x42900000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,0);
        uVar20 = FUN_0042fab0(DAT_007f1a3c);
        FUN_004739f0(uVar20,0x43280000,0x439a0000,0x42900000,0x42900000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,0);
        uVar20 = FUN_0042fab0(DAT_007f1a4c);
        FUN_004739f0(uVar20,0x43680000,0x439a0000,0x42900000,0x42900000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,0);
        uVar20 = FUN_0040bb90(&DAT_005cda30);
        FUN_004739f0(uVar20,0x42900000,0x439e0000,0x42600000,0x42600000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,1);
        FUN_004739f0(uVar20,0x43080000,0x439e0000,0x42600000,0x42600000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,1);
        FUN_004739f0(uVar20,0x43480000,0x439e0000,0x42600000,0x42600000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,1);
      }
    }
    else {
      if (cVar21 == '\x02') {
        uVar20 = FUN_0042fab0(DAT_007f1a1c);
        FUN_004739f0(uVar20,0x42200000,0x43920000,0x42e00000,0x42e00000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,1);
        uVar20 = FUN_0042fab0(DAT_007f1a2c);
        FUN_004739f0(uVar20,0x43440000,0x43920000,0x42e00000,0x42e00000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,0);
        uVar20 = FUN_0040bb90(&DAT_005cda30);
        FUN_004739f0(uVar20,0x43040000,0x43960000,0x42b00000,0x42b00000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,1);
      }
      iVar15 = -1;
      iVar16 = -1;
      iVar9 = -1;
      iVar13 = -1;
      iStack_2c = -1;
      iStack_30 = -1;
      iVar14 = -1;
      iStack_1c = -1;
      if (cVar21 == '\x03') {
        if (DAT_007f1a18 == 0) {
          iVar13 = 0;
          if (DAT_007f1a28 == 0) {
            iVar14 = 1;
            iVar16 = iVar13;
            iVar9 = 2;
LAB_00437757:
            iVar13 = iVar9;
            iVar9 = iVar16;
          }
          else {
            iVar9 = 1;
            iVar14 = 2;
          }
        }
        else {
          iVar9 = 0;
          if (DAT_007f1a28 == 0) {
            iVar13 = 1;
            iVar14 = 2;
            iVar16 = iVar13;
            if (DAT_007f1a38 == 0) goto LAB_00437757;
          }
          else {
            iVar14 = 1;
            iVar13 = 2;
          }
        }
        uVar20 = FUN_0042fab0((&DAT_007f1a1c)[iVar13 * 4]);
        FUN_004739f0(uVar20,0x42600000,0x43920000,0x42e00000,0x42e00000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,1);
        uVar20 = FUN_0042fab0((&DAT_007f1a1c)[iVar9 * 4]);
        FUN_004739f0(uVar20,0x43580000,0x43900000,0x42800000,0x42800000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,0);
        uVar20 = FUN_0042fab0((&DAT_007f1a1c)[iVar14 * 4]);
        FUN_004739f0(uVar20,0x43580000,0x43aa0000,0x42800000,0x42800000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,0);
        local_54 = 144.0;
        local_50 = 304.0;
        goto LAB_00438986;
      }
      if (cVar21 == '\x04') {
        if (DAT_007f1a18 == 0) {
          iVar14 = 0;
        }
        else {
          iVar13 = 0;
        }
        if (DAT_007f1a28 == 0) {
          if (iVar14 == -1) {
            iVar14 = 1;
          }
          else {
            iVar16 = 1;
            iStack_30 = 1;
          }
        }
        else if (iVar13 == -1) {
          iVar13 = 1;
        }
        else {
          iVar15 = 1;
        }
        if (DAT_007f1a38 == 0) {
          if (iVar14 == -1) {
            iVar14 = 2;
          }
          else if (iVar16 == -1) {
            iVar16 = 2;
            iStack_30 = 2;
          }
          else {
            iStack_2c = 2;
          }
        }
        else if (iVar13 == -1) {
          iVar13 = 2;
        }
        else if (iVar15 == -1) {
          iVar15 = 2;
        }
        else {
          iVar9 = 2;
          iStack_1c = 2;
        }
        if (DAT_007f1a48 == 0) {
          if (iVar14 == -1) {
            iVar14 = 3;
          }
          else if (iVar16 == -1) {
            iVar16 = 3;
            iStack_30 = 3;
          }
          else {
            iStack_2c = 3;
          }
        }
        else if (iVar13 == -1) {
          iVar13 = 3;
        }
        else if (iVar15 == -1) {
          iVar15 = 3;
        }
        else {
          iVar9 = 3;
          iStack_1c = 3;
        }
        iVar12 = iVar14;
        iVar19 = iVar15;
        if (iStack_2c != -1) {
          iStack_1c = iStack_2c;
          iVar12 = iVar13;
          iVar9 = iStack_2c;
          iVar19 = iVar16;
          iVar13 = iVar14;
          iStack_30 = iVar15;
        }
        if (iVar9 == -1) {
          uVar20 = FUN_0042fab0((&DAT_007f1a1c)[iVar12 * 4]);
          FUN_004739f0(uVar20,0x42940000,0x43940000,0x42800000,0x42800000,local_6c,0,0x3f800000,0,
                       0x3f800000,1,1);
          uVar20 = FUN_0042fab0((&DAT_007f1a1c)[iVar13 * 4]);
          FUN_004739f0(uVar20,0x435c0000,0x43940000,0x42800000,0x42800000,local_6c,0,0x3f800000,0,
                       0x3f800000,1,0);
          uVar20 = FUN_0042fab0((&DAT_007f1a1c)[iStack_30 * 4]);
          FUN_004739f0(uVar20,0x42940000,0x43aa0000,0x42800000,0x42800000,local_6c,0,0x3f800000,0,
                       0x3f800000,1,0);
          uVar20 = FUN_0042fab0((&DAT_007f1a1c)[iVar19 * 4]);
          FUN_004739f0(uVar20,0x435c0000,0x43aa0000,0x42800000,0x42800000,local_6c,0,0x3f800000,0,
                       0x3f800000,1,0);
          local_5c = 0x42c00000;
          local_60 = 0x42c00000;
          local_54 = 127.0;
          local_50 = 300.0;
        }
        else {
          uVar20 = FUN_0042fab0((&DAT_007f1a1c)[iVar12 * 4]);
          FUN_004739f0(uVar20,0x42600000,0x43920000,0x42e00000,0x42e00000,local_6c,0,0x3f800000,0,
                       0x3f800000,1,1);
          uVar20 = FUN_0042fab0((&DAT_007f1a1c)[iVar13 * 4]);
          FUN_004739f0(uVar20,0x434a0000,0x43900000,0x42800000,0x42800000,local_6c,0,0x3f800000,0,
                       0x3f800000,1,0);
          uVar20 = FUN_0042fab0((&DAT_007f1a1c)[iVar19 * 4]);
          FUN_004739f0(uVar20,0x43680000,0x439e0000,0x42800000,0x42800000,local_6c,0,0x3f800000,0,
                       0x3f800000,1,0);
          uVar20 = FUN_0042fab0((&DAT_007f1a1c)[iStack_1c * 4]);
          FUN_004739f0(uVar20,0x434a0000,0x43ac0000,0x42800000,0x42800000,local_6c,0,0x3f800000,0,
                       0x3f800000,1,0);
          local_5c = 0x42b00000;
          local_60 = 0x42b00000;
          local_54 = 144.0;
          local_50 = 304.0;
        }
        uVar20 = FUN_0040bb90(&DAT_005cda30);
LAB_00437b6a:
        FUN_004739f0(uVar20,local_54,local_50,local_60,local_5c,local_6c,0,0x3f800000,0,0x3f800000,1
                     ,1);
      }
    }
  }
  else {
    switch(DAT_0067e9fc) {
    case 2:
      FUN_00427e00(0x24,0x43a50000,0x439e0000,local_6c,0x3f19999a,0);
      FUN_00427e00(0x13a,0x43a50000,0x43a70000,local_6c,0x3f0ccccd,0);
      uVar20 = (&DAT_007f0db4)[DAT_0067f17c];
      iVar9 = (&DAT_007f0de8)[DAT_0067f17c];
      iVar13 = FUN_004a2c48();
      if (iVar9 < 10) {
        FUN_004a2b60(auStack_3c,&DAT_005cd798,&DAT_005cd7a0);
      }
      else {
        FUN_004a2b60(auStack_3c,&DAT_005cd794,iVar9);
      }
      if (iVar13 < 10) {
        FUN_004a2b60(auStack_44,&DAT_005cd798,&DAT_005cd7a0);
      }
      else {
        FUN_004a2b60(auStack_44,&DAT_005cd794,iVar13);
      }
      FUN_004a2b60(auStack_14,"%d%s%s%s%s",uVar20,&DAT_005cd9c4,auStack_3c,&DAT_005cd9c4,auStack_44)
      ;
      FUN_00427e00(0x12a,0x43a50000,0x43af0000,local_6c,0x3f0ccccd,0);
      FUN_00427f00(auStack_14,0x43fe0000,0x43af0000,local_6c,0x3f0ccccd,0);
      local_68 = 1;
      uVar18 = local_68;
      break;
    case 3:
    case 4:
    case 5:
      iVar9 = 0;
      if (DAT_0067e9fc == 3) {
        uVar18 = 0x115;
      }
      else if (DAT_0067e9fc == 4) {
        iVar9 = 1;
        uVar18 = 0x116;
      }
      else {
        uVar18 = a;
        if (DAT_0067e9fc == 5) {
          iVar9 = 2;
          uVar18 = 0x117;
        }
      }
      if (9 < DAT_0067f17c) {
        uVar18 = 0x254;
        if (DAT_0067f17c == 0xb) {
          uVar18 = 599;
        }
        else if (DAT_0067f17c == 0xc) {
          uVar18 = 0x25a;
        }
        if (DAT_0067e9fc == 4) {
          uVar18 = uVar18 + 1;
        }
        else if (DAT_0067e9fc == 5) {
          uVar18 = uVar18 + 2;
        }
      }
      FUN_00427e00(uVar18,0x43a30000,0x439e0000,local_6c,0x3f0ccccd,0);
      uVar18 = *(uint *)(&DAT_005f65c8 + (iVar9 + DAT_0067f17c * 3) * 4) & 0xffff0000;
      uVar11 = *(uint *)(&DAT_005f65c8 + (iVar9 + DAT_0067f17c * 3) * 4) & 3;
      if (((uVar18 == 0) || (uVar18 == 0x10000)) || (uVar18 == 0xc0000)) {
        if (uVar11 == 1) {
          FUN_00427e00(0xec,0x43a30000,0x43a70000,local_6c,0x3f19999a,0);
          FUN_00427e00(0x118,0x43a30000,0x43af0000,local_6c,0x3f0ccccd,0);
          if (uVar18 == 0x10000) {
            FUN_00427e00(0x121,0x43a30000,0x43b70000,local_6c,0x3f0ccccd,0);
          }
        }
        else {
          if (uVar11 == 2) {
            if ((uVar18 == 0x10000) || (uVar18 == 0xc0000)) {
              FUN_00427e00(0x206,0x43a30000,0x43a70000,local_6c,0x3f19999a,0);
              FUN_00427e00(0x207,0x43a30000,0x43af0000,local_6c,0x3f0ccccd,0);
              uVar20 = 0x208;
            }
            else {
              FUN_00427e00(0x203,0x43a30000,0x43a70000,local_6c,0x3f19999a,0);
              FUN_00427e00(0x204,0x43a30000,0x43af0000,local_6c,0x3f0ccccd,0);
              uVar20 = 0x205;
            }
          }
          else if ((uVar18 == 0x10000) || (uVar18 == 0xc0000)) {
            FUN_00427e00(0x209,0x43a30000,0x43a70000,local_6c,0x3f19999a,0);
            FUN_00427e00(0x20a,0x43a30000,0x43af0000,local_6c,0x3f0ccccd,0);
            uVar20 = 0x20b;
          }
          else {
            FUN_00427e00(0x20c,0x43a30000,0x43a70000,local_6c,0x3f19999a,0);
            FUN_00427e00(0x20d,0x43a30000,0x43af0000,local_6c,0x3f0ccccd,0);
            uVar20 = 0x20e;
          }
          FUN_00427e00(uVar20,0x43a30000,0x43b70000,local_6c,0x3f0ccccd,0);
        }
        uVar17 = 2;
        local_68 = 2;
      }
      if ((uVar18 == 0x90000) || (uVar18 == 0xa0000)) {
        FUN_00427e00(0xec,0x43a30000,0x43a70000,local_6c,0x3f19999a,0);
        if (uVar11 == 1) {
          uVar20 = 0x118;
        }
        else if (uVar11 == 2) {
          uVar20 = 0x119;
        }
        else {
          uVar20 = 0x11a;
        }
        FUN_00427e00(uVar20,0x43a30000,0x43af0000,local_6c,0x3f0ccccd,0);
        if (uVar18 == 0xa0000) {
          FUN_00427e00(0x121,0x43a30000,0x43b70000,local_6c,0x3f0ccccd,0);
        }
        uVar17 = 2;
        local_68 = 2;
      }
      if (((uVar18 == 0x20000) || (uVar18 == 0x30000)) || (uVar18 == 0xb0000)) {
        FUN_00427e00(0x11f,0x43a30000,0x43a70000,local_6c,0x3f19999a,0);
        FUN_00427e00(0x120,0x43a30000,0x43af0000,local_6c,0x3f0ccccd,0);
        if (uVar18 != 0x20000) {
          FUN_00427e00(0x121,0x43a30000,0x43b70000,local_6c,0x3f0ccccd,0);
        }
        uVar17 = 2;
        local_68 = 2;
      }
      if (uVar18 == 0x40000) {
        FUN_00427e00(0xf2,0x43a30000,0x43a70000,local_6c,0x3f19999a,0);
        FUN_00427e00(0xf7,0x43a30000,0x43af0000,local_6c,0x3f0ccccd,0);
        FUN_00427e00(0xf8,0x43a30000,0x43b70000,local_6c,0x3f0ccccd,0);
        local_68 = uVar18;
      }
      else if (uVar18 == 0x50000) {
        FUN_00427e00(0xf0,0x43a30000,0x43a70000,local_6c,0x3f19999a,0);
        FUN_00427e00(0xf5,0x43a30000,0x43af0000,local_6c,0x3f0ccccd,0);
        FUN_00427e00(0xf6,0x43a30000,0x43b70000,local_6c,0x3f0ccccd,0);
        local_68 = uVar18;
      }
      else if (uVar18 == 0x60000) {
        FUN_00427e00(0xef,0x43a30000,0x43a70000,local_6c,0x3f19999a,0);
        FUN_00427e00(0xf9,0x43a30000,0x43af0000,local_6c,0x3f0ccccd,0);
        FUN_00427e00(0xfa,0x43a30000,0x43b70000,local_6c,0x3f0ccccd,0);
        uVar18 = 2000;
        local_68 = uVar18;
      }
      else if (uVar18 == 0x70000) {
        FUN_00427e00(0xee,0x43a30000,0x43a70000,local_6c,0x3f19999a,0);
        FUN_00427e00(0xf3,0x43a30000,0x43af0000,local_6c,0x3f0ccccd,0);
        FUN_00427e00(0xf4,0x43a30000,0x43b70000,local_6c,0x3f0ccccd,0);
        local_68 = uVar18;
      }
      else {
        if (uVar18 == 0x80000) {
          FUN_00427e00(0xf1,0x43a30000,0x43a70000,local_6c,0x3f19999a,0);
          FUN_00427e00(0xfb,0x43a30000,0x43af0000,local_6c,0x3f0ccccd,0);
          FUN_00427e00(0xfc,0x43a30000,0x43b70000,local_6c,0x3f0ccccd,0);
          goto LAB_00436cf8;
        }
        if (uVar17 == 0) goto switchD_00436a9b_caseD_6;
        uVar18 = uVar17;
        if ((2 < uVar17) && (uVar17 < 0x100)) goto LAB_004375d5;
      }
      break;
    default:
      goto switchD_00436a9b_caseD_6;
    case 10:
      sVar6 = FUN_0042b8b0();
      fVar3 = (float)(int)sVar6 * _DAT_005cda38;
      sVar6 = FUN_0042b8c0();
      fVar4 = (float)(int)sVar6 * _DAT_005cc9a0;
      sVar6 = FUN_0042b8b0();
      fVar1 = (float)((sVar6 * 0x142) / 0x280);
      sVar6 = FUN_0042b8c0();
      fVar2 = (float)((sVar6 * 0x142) / 0x1e0);
      FUN_00427e00(0xe6,0x43ac0000,0x439e0000,local_6c,0x3f19999a,0);
      if ((undefined *)(&DAT_0067ed38)[DAT_0067e9f8 * 0x10] == &DAT_005f7370) {
        cVar21 = DAT_0067ea98 != 0;
        if (DAT_0067ea9c != 0) {
          cVar21 = cVar21 + '\x01';
        }
        if (DAT_0067eaa0 != 0) {
          cVar21 = cVar21 + '\x01';
        }
        DAT_0067ea64 = DAT_0067eaac ^ 1;
        if ((cVar21 == '\x03') && (DAT_0067eaac != 0)) {
          FUN_00427e00(0x124,0x43ac0000,0x43a70000,local_6c,0x3f19999a,0);
          FUN_00427e00(0x125,0x43ac0000,0x43af0000,local_6c,0x3f19999a,0);
        }
        else {
          FUN_00427e00(300,0x43ac0000,0x43a70000,local_6c,0x3f19999a,0);
          FUN_00427e00(0x12d,0x43ac0000,0x43af0000,local_6c,0x3f19999a,0);
        }
      }
      else {
        FUN_00427e00(0xe7,0x43ac0000,0x43a70000,local_6c,0x3f0ccccd,0);
        uVar20 = FUN_0040bb50("check",fVar1,fVar2,fVar3,fVar4,local_6c,1);
        FUN_00473870(uVar20);
        sVar6 = FUN_0042b8c0();
        fVar2 = (float)((sVar6 * 0x10) / 0x1e0) + fVar2;
        FUN_00427e00(0xe8,0x43ac0000,0x43af0000,local_6c,0x3f0ccccd,0);
        uVar20 = FUN_0040bb50("check",fVar1,fVar2,fVar3,fVar4,local_6c,1);
        FUN_00473870(uVar20);
        sVar6 = FUN_0042b8c0();
        fVar2 = (float)((sVar6 * 0x10) / 0x1e0) + fVar2;
        FUN_00427e00(0xe9,0x43ac0000,0x43b70000,local_6c,0x3f0ccccd,0);
        uVar20 = FUN_0040bb50("check",fVar1,fVar2,fVar3,fVar4,local_6c,1);
        FUN_00473870(uVar20);
        sVar6 = FUN_0042b8c0();
        FUN_00427e00(0xea,0x43ac0000,0x43bf0000,local_6c,0x3f0ccccd,0);
        uVar20 = FUN_0040bb50("check",fVar1,(float)((sVar6 * 0x10) / 0x1e0) + fVar2,fVar3,fVar4,
                              local_6c,1);
        FUN_00473870(uVar20);
      }
LAB_00436cf8:
      uVar18 = 2;
      local_68 = uVar18;
    }
    local_54 = 40.0;
    local_50 = 292.0;
    local_48 = 0x42e00000;
    local_4c = 0x42e00000;
    if (uVar18 == 1) {
      local_54 = 128.0;
    }
    if (DAT_0067e9fc == 10) {
      cVar21 = DAT_0067ea98 != 0;
      if (DAT_0067ea9c != 0) {
        cVar21 = cVar21 + '\x01';
      }
      if (DAT_0067eaa0 != 0) {
        cVar21 = cVar21 + '\x01';
      }
      if (cVar21 == '\0') {
        local_54 = local_54 + _DAT_005cda34;
      }
    }
    uVar10 = FUN_0042fab0(DAT_007f1a1c);
    uVar20 = 0x42e00000;
    uVar24 = 0x42e00000;
    FUN_004739f0(uVar10,local_54,0x43920000,0x42e00000,0x42e00000,local_6c,0,0x3f800000,0,0x3f800000
                 ,1,1);
    if ((uVar18 == 2) || (uVar18 == 2000)) {
      if (DAT_0067e9fc == 10) {
        if (DAT_0067eaac != 0) {
LAB_004382e3:
          iVar9 = FUN_00430670(1);
          cVar21 = iVar9 != -1;
          iVar9 = FUN_00430670(2);
          if (iVar9 != -1) {
            cVar21 = '\x02';
          }
          iVar9 = FUN_00430670(3);
          if (iVar9 == -1) {
            if (cVar21 == '\x02') {
              local_48 = 0x42800000;
              local_4c = 0x42800000;
            }
            else if (cVar21 == '\x03') goto LAB_004385f2;
          }
          else {
            cVar21 = '\x03';
LAB_004385f2:
            local_48 = 0x42600000;
            local_4c = 0x42600000;
          }
          iVar9 = FUN_00430670(1);
          if (iVar9 != -1) {
            if (cVar21 == '\x01') {
              local_54 = 196.0;
              local_50 = 292.0;
            }
            else if (cVar21 == '\x02') {
              local_54 = 164.0;
              local_50 = 316.0;
            }
            else if (cVar21 == '\x03') {
              local_54 = 142.0;
              local_50 = 320.0;
            }
            uVar20 = FUN_0042fab0(iVar9);
            FUN_004739f0(uVar20,local_54,local_50,local_4c,local_48,local_6c,0,0x3f800000,0,
                         0x3f800000,1,0);
          }
          iVar9 = FUN_00430670(2);
          if (iVar9 != -1) {
            if (cVar21 == '\x02') {
              local_54 = 242.0;
              local_50 = 316.0;
            }
            else if (cVar21 == '\x03') {
              local_54 = 192.0;
              local_50 = 320.0;
            }
            uVar20 = FUN_0042fab0(iVar9);
            FUN_004739f0(uVar20,local_54,local_50,local_4c,local_48,local_6c,0,0x3f800000,0,
                         0x3f800000,1,0);
          }
          iVar9 = FUN_00430670(3);
          if (iVar9 != -1) {
            uVar20 = FUN_0042fab0(iVar9);
            FUN_004739f0(uVar20,0x437c0000,0x43a00000,local_4c,local_48,local_6c,0,0x3f800000,0,
                         0x3f800000,1,0);
          }
          uVar20 = local_48;
          uVar18 = local_68;
          uVar24 = local_4c;
          if (cVar21 == '\x01') {
            uVar10 = FUN_0040bb90(&DAT_005cda30);
            FUN_004739f0(uVar10,0x43040000,0x43960000,0x42b00000,0x42b00000,local_6c,0,0x3f800000,0,
                         0x3f800000,1,1);
          }
          else if (cVar21 == '\x02') {
            uVar10 = FUN_0040bb90(&DAT_005cda30);
            FUN_004739f0(uVar10,0x43000000,0x43a00000,0x42600000,0x42600000,local_6c,0,0x3f800000,0,
                         0x3f800000,1,1);
            FUN_004739f0(uVar10,0x434e0000,0x43a00000,0x42600000,0x42600000,local_6c,0,0x3f800000,0,
                         0x3f800000,1,1);
          }
          else if (cVar21 == '\x03') {
            uVar10 = FUN_0040bb90(&DAT_005cda30);
            FUN_004739f0(uVar10,0x42e80000,0x43a20000,0x42400000,0x42400000,local_6c,0,0x3f800000,0,
                         0x3f800000,1,1);
            FUN_004739f0(uVar10,0x43260000,0x43a20000,0x42400000,0x42400000,local_6c,0,0x3f800000,0,
                         0x3f800000,1,1);
            FUN_004739f0(uVar10,0x43580000,0x43a20000,0x42400000,0x42400000,local_6c,0,0x3f800000,0,
                         0x3f800000,1,1);
          }
          goto LAB_00438916;
        }
      }
      else if (((DAT_0067e9fc == 3) || (DAT_0067e9fc == 4)) || (DAT_0067e9fc == 5)) {
        iVar9 = 0;
        if (DAT_0067e9fc == 4) {
          iVar9 = 1;
        }
        else if (DAT_0067e9fc == 5) {
          iVar9 = 2;
        }
        if (((*(uint *)(&DAT_005f65c8 + (iVar9 + DAT_0067f17c * 3) * 4) & 0xffff0000) == 0x90000) ||
           ((*(uint *)(&DAT_005f65c8 + (iVar9 + DAT_0067f17c * 3) * 4) & 0xffff0000) == 0xa0000))
        goto LAB_004382e3;
      }
      cVar21 = '\x01';
      iVar9 = FUN_00430670(2);
      if (iVar9 != -1) {
        cVar21 = '\x02';
      }
      iVar9 = FUN_00430670(3);
      if (iVar9 != -1) {
        cVar21 = '\x03';
      }
      if (DAT_0067e9fc == 10) {
        cVar21 = DAT_0067ea98 != 0;
        if (DAT_0067ea9c != 0) {
          cVar21 = cVar21 + '\x01';
        }
        if (DAT_0067eaa0 != 0) {
          cVar21 = cVar21 + '\x01';
        }
      }
      if ((cVar21 == '\x02') || (cVar21 == '\x03')) {
        uVar24 = 0x42800000;
        uVar20 = 0x42800000;
      }
      iVar9 = FUN_00430670(1);
      if (iVar9 != -1) {
        if (cVar21 == '\x01') {
          local_54 = 196.0;
          local_50 = 292.0;
        }
        else {
          if (cVar21 == '\x02') {
            local_54 = 216.0;
          }
          else {
            if (cVar21 != '\x03') goto LAB_00438420;
            local_54 = 202.0;
          }
          local_50 = 288.0;
        }
LAB_00438420:
        uVar10 = FUN_0042fab0(iVar9);
        FUN_004739f0(uVar10,local_54,local_50,uVar24,uVar20,local_6c,0,0x3f800000,0,0x3f800000,1,0);
      }
      iVar9 = FUN_00430670(2);
      if (iVar9 != -1) {
        if (cVar21 == '\x02') {
          local_54 = 216.0;
          local_50 = 340.0;
        }
        else if (cVar21 == '\x03') {
          local_54 = 232.0;
          local_50 = 316.0;
        }
        uVar10 = FUN_0042fab0(iVar9);
        FUN_004739f0(uVar10,local_54,local_50,uVar24,uVar20,local_6c,0,0x3f800000,0,0x3f800000,1,0);
      }
      iVar9 = FUN_00430670(3);
      if (iVar9 != -1) {
        uVar10 = FUN_0042fab0(iVar9);
        FUN_004739f0(uVar10,0x434a0000,0x43ac0000,uVar24,uVar20,local_6c,0,0x3f800000,0,0x3f800000,1
                     ,0);
      }
      if (cVar21 != '\0') {
        uVar10 = FUN_0040bb90(&DAT_005cda30);
        FUN_004739f0(uVar10,0x43040000,0x43960000,0x42b00000,0x42b00000,local_6c,0,0x3f800000,0,
                     0x3f800000,1,1);
        if (uVar18 == 2000) {
          uVar20 = FUN_0042fab0(9);
          FUN_004739f0(uVar20,0x42d00000,0x43b20000,0x42400000,0x42400000,local_6c,0,0x3f800000,0,
                       0x3f800000,1,1);
          goto switchD_00436a9b_caseD_6;
        }
      }
    }
LAB_00438916:
    if (uVar18 == 0x70000) {
      uVar10 = 6;
    }
    else if (uVar18 == 0x40000) {
      uVar10 = 7;
    }
    else {
      if (uVar18 != 0x50000) goto switchD_00436a9b_caseD_6;
      uVar10 = 8;
    }
    uVar10 = FUN_0042fab0(uVar10);
    FUN_004739f0(uVar10,0x43440000,0x43920000,uVar24,uVar20,local_6c,0,0x3f800000,0,0x3f800000,1,0);
    local_54 = 132.0;
    local_50 = 300.0;
LAB_00438986:
    local_5c = 0x42b00000;
    local_60 = 0x42b00000;
LAB_00438996:
    uVar20 = FUN_0040bb90(&DAT_005cda30);
    FUN_004739f0(uVar20,local_54,local_50,local_60,local_5c,local_6c,0,0x3f800000,0,0x3f800000,1,1);
  }
switchD_00436a9b_caseD_6:
  local_64 = (float)CONCAT13(a,0xffffff);
  uVar20 = local_64;
  iVar9 = FUN_00430760();
  if ((iVar9 != 0) && (((DAT_0067e9fc == 3 || (DAT_0067e9fc == 4)) || (DAT_0067e9fc == 5)))) {
    iVar9 = 0;
    if (DAT_0067e9fc == 4) {
      iVar9 = 1;
    }
    else if (DAT_0067e9fc == 5) {
      iVar9 = 2;
    }
    uVar17 = *(uint *)(&DAT_005f65c8 + (iVar9 + DAT_0067f17c * 3) * 4) & 0xffff0000;
    if (((((uVar17 == 0x10000) || (uVar17 == 0xc0000)) ||
         ((uVar17 == 0xa0000 || ((uVar17 == 0x30000 || (uVar17 == 0xb0000)))))) ||
        (uVar17 == 0x70000)) || (uVar17 == 0x60000)) {
      iVar9 = 0;
      do {
        FUN_0042b8b0();
        FUN_0042b8c0();
        FUN_0042b8b0();
        iVar13 = 0;
        if (DAT_0067e9fc == 4) {
          iVar13 = 1;
        }
        else if (DAT_0067e9fc == 5) {
          iVar13 = 2;
        }
        uVar17 = *(uint *)(&DAT_005f65c8 + (iVar13 + DAT_0067f17c * 3) * 4) & 0xffff0000;
        if ((uVar17 == 0xb0000) || (uVar17 == 0xc0000)) {
          iVar13 = FUN_004a2c48();
          fVar22 = (float10)fsin((float10)(iVar9 * 0x1e - iVar13) * (float10)_DAT_005cc8f4);
          fVar22 = fVar22 * (float10)_DAT_005cd784;
          if (fVar22 < (float10)DAT_005d757c) {
            fVar22 = (float10)DAT_005d757c;
          }
          local_64 = (float)(fVar22 * (float10)_DAT_005cda14);
          if ((float10)_DAT_005cd05c <= fVar22 * (float10)_DAT_005cda14) {
            FUN_00472650(0,0x41700000);
            uVar24 = FUN_004a2c48();
            (&DAT_0067ed38)[iVar9] = uVar24;
          }
          iVar13 = FUN_00458630(*(undefined4 *)(&DAT_005f6668 + ((&DAT_0067ed38)[iVar9] & 0xf) * 4))
          ;
LAB_00438c1e:
          if (iVar13 != 0) {
            sVar6 = FUN_0042b8b0();
            fVar2 = (float)(int)sVar6 * _DAT_005cd988;
            sVar6 = FUN_0042b8c0();
            fVar3 = (float)(int)sVar6 * _DAT_005cc56c;
            fVar1 = (float)((float10)local_64 * (float10)_DAT_005cc348);
            fVar22 = (float10)local_64 * (float10)_DAT_005cc348 * (float10)_DAT_005cc32c;
            sVar6 = FUN_0042b8b0();
            fVar4 = (float)(int)sVar6 * fVar1 * _DAT_005cd5a8;
            sVar6 = FUN_0042b8c0();
            uVar24 = FUN_0040bb90("Powerupshadow",(float)(fVar22 + extraout_ST1),
                                  (float)(fVar22 + extraout_ST0),fVar2 - fVar4,
                                  fVar3 - (float)(int)sVar6 * fVar1 * _DAT_005cc560,uVar20,0);
            FUN_00473870(uVar24);
            FUN_0042b8b0();
            fVar23 = (float10)FUN_0042b8c0();
            sVar6 = FUN_0042b8b0();
            FUN_0042b8b0();
            FUN_0042b8c0();
            sVar7 = FUN_0042b8b0();
            fVar22 = (float10)_DAT_005cd5a8;
            sVar8 = FUN_0042b8c0();
            FUN_00473870(iVar13,(float)((fVar23 - (float10)((sVar6 * iVar9 * 0x28) / 0x280)) +
                                       extraout_ST0_00),(float)(extraout_ST0_00 + extraout_ST1_00),
                         (float)(extraout_ST1_01 - (float10)(int)sVar7 * (float10)local_64 * fVar22)
                         ,(float)(extraout_ST0_01 -
                                 (float10)(int)sVar8 * (float10)local_64 * (float10)_DAT_005cc560),
                         uVar20,0);
          }
        }
        else if (-1 < (int)(&DAT_007f0cb0)[iVar9 + DAT_0067f17c * 5]) {
          iVar13 = FUN_00458630((&DAT_007f0cb0)[iVar9 + DAT_0067f17c * 5]);
          if (uVar17 == 0x70000) {
            iVar13 = 0;
            if (iVar9 == 0) {
              uVar24 = 9;
            }
            else {
              if (iVar9 != 1) goto LAB_00438c16;
              uVar24 = 0xb;
            }
            iVar13 = FUN_00458630(uVar24);
          }
LAB_00438c16:
          local_64 = 0.0;
          goto LAB_00438c1e;
        }
        iVar9 = iVar9 + 1;
      } while (iVar9 < 5);
    }
  }
  iVar9 = DAT_0067f17c;
  if ((DAT_0067e9fc == 10) || (DAT_0067e9fc == 2)) {
LAB_0043908c:
    if (DAT_0067e9fc == 3) goto LAB_004390aa;
  }
  else {
    iVar13 = FUN_004309b0();
    iVar9 = DAT_0067f17c;
    iVar14 = FUN_0042ef40(DAT_0067f17c,iVar13 + 1000);
    if ((iVar14 == 0) && ((&DAT_007f0a40)[iVar13 + iVar9 * 0xc] == 1)) {
LAB_00438e56:
      DAT_00898ab0 = 0;
      goto LAB_0043908c;
    }
    iVar14 = (&DAT_007f0a40)[iVar13 + iVar9 * 0xc];
    if ((iVar14 != 2) && (iVar14 != 0)) {
      DAT_00898ab0 = 0;
      goto LAB_0043908c;
    }
    iVar15 = FUN_00430760();
    iVar16 = DAT_00898ab0;
    if (iVar15 == 0) goto LAB_00438e56;
    (*(uchar *)((char *)&local_6c + 3)) = a;
    if (DAT_00898ab0 == 0) {
      fsin((float10)_DAT_0067ea60 * (float10)_DAT_005cc9f4);
      (*(uchar *)((char *)&local_6c + 3)) = FUN_004a2c48();
    }
    pf = 7.5;
    fVar1 = _DAT_005cd110;
    if (0x20 < iVar16) {
      (*(uchar *)((char *)&local_6c + 3)) = FUN_004a2c48();
      pf = -(float)(iVar16 + -0x20) * _DAT_005cc35c + _DAT_005cd9fc;
      fVar1 = (float)(iVar16 + -0x20) * _DAT_005cc32c + _DAT_005cd110;
    }
    bVar5 = (*(uchar *)((char *)&local_6c + 3));
    fVar1 = fVar1 * _DAT_005cd9f8;
    local_54 = 466.0;
    local_50 = 260.0;
    pf = pf - _DAT_005cd9fc;
    if (0x20 < iVar16) {
      local_54 = _DAT_005cd9f0 - _DAT_005cd9f4 * (float)(iVar16 + -0x20);
      local_50 = _DAT_005cd9ec - (float)(iVar16 + -0x20) * _DAT_005cc738;
    }
    local_6c = (uint)(*(uchar *)((char *)&local_6c + 3)) << 0x18;
    uVar20 = 0xeb;
    if (iVar14 == 0) {
      uVar20 = 0x263;
    }
    FUN_0042ed70(uVar20,local_54,local_50,local_6c,fVar1,pf);
    local_6c = (local_6c & 0xff000000) | 0xa0e0ff;
    if ((&DAT_007f0a40)[iVar13 + DAT_0067f17c * 0xc] == 0) {
      local_6c = (local_6c & 0xff000000) | 0x8080ff;
    }
    local_6c = CONCAT13(bVar5,(undefined3)local_6c);
    FUN_0042ed70(uVar20,local_54 - _DAT_005cc574,local_50 - _DAT_005cc574,local_6c,fVar1,pf);
    iVar9 = DAT_0067f17c;
    if ((DAT_00898ab0 != 0) && (DAT_00898ab0 = DAT_00898ab0 + -1, DAT_00898ab0 < 0)) {
      DAT_00898ab0 = 0;
      goto LAB_0043908c;
    }
    if (DAT_00898ab0 != 1) goto LAB_0043908c;
    if (DAT_00899140 != 0) {
      FUN_0042bf30(DAT_00899140,0,1,0x2d,0,0);
    }
    iVar9 = DAT_0067f17c;
    if (DAT_0067e9fc == 3) {
      (*(uchar *)((char *)&local_6c + 3)) = bVar5;
      if (((5 < DAT_0067f17c) && (DAT_0067f17c != 0xc)) &&
         ((&DAT_007f0a74)[DAT_0067f17c * 0xc] != 0)) {
        DAT_0067f180 = DAT_0067f180 + -0x15;
      }
      goto LAB_004390aa;
    }
  }
  if ((DAT_0067e9fc != 4) && (DAT_0067e9fc != 5)) {
    DAT_00898ab0 = 0;
  }
LAB_004390aa:
  bVar5 = (*(uchar *)((char *)&local_6c + 3));
  if (9 < iVar9) {
    local_6c = (uint)(*(uchar *)((char *)&local_6c + 3)) << 0x18;
    FUN_0042ed70(0x253,0x43e90000,0x42e80000,local_6c,0x3f53dcb1,0);
    local_6c = CONCAT13(bVar5,0xa0e0ff);
    FUN_0042ed70(0x253,0x43e78000,0x42e20000,local_6c,0x3f53dcb1,0);
  }
  return;
}
