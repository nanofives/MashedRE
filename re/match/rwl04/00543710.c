// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00543710: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(int, DAT_007dc5f4, 0x007dc5f4);
MGLOBAL(undefined4, DAT_007dc62c, 0x007dc62c);
MGLOBAL(int, DAT_007dc5f8, 0x007dc5f8);
MGLOBAL(undefined4, DAT_007dc630, 0x007dc630);
MGLOBAL(uint, DAT_007dc624, 0x007dc624);
MGLOBAL(uint, DAT_007dc614, 0x007dc614);
MGLOBAL(uint, DAT_007dc618, 0x007dc618);
MGLOBAL(uint, DAT_007dc61c, 0x007dc61c);
MGLOBAL(undefined4, DAT_007dc628, 0x007dc628);
MGLOBAL(undefined4, DAT_007dc5fc, 0x007dc5fc);
MGLOBAL(undefined4, DAT_007dc600, 0x007dc600);
MGLOBAL(undefined4, DAT_007dc604, 0x007dc604);
MGLOBAL(undefined4, DAT_007dc608, 0x007dc608);
MGLOBAL(undefined4, DAT_007dc60c, 0x007dc60c);
MGLOBAL(undefined4, DAT_007dc610, 0x007dc610);
MGLOBAL(undefined4, DAT_007dc620, 0x007dc620);
MGLOBAL(undefined4, DAT_00623d30, 0x00623d30);
MGLOBAL(undefined4, DAT_00623d34, 0x00623d34);
MGLOBAL(undefined1, DAT_005e4580, 0x005e4580);
MGLOBAL(undefined1, DAT_005e45e0, 0x005e45e0);
MGLOBAL(undefined1, DAT_005e4630, 0x005e4630);
MGLOBAL(undefined1, DAT_005e4670, 0x005e4670);
MGLOBAL(undefined1, DAT_005e46f8, 0x005e46f8);
MGLOBAL(undefined1, DAT_005e47a8, 0x005e47a8);
void FUN_00543500(void);
void FUN_00543950(void);
void FUN_005439d0(void);
bool FUN_004cbb20(void *p0, void *p1);
int FUN_004cbb60(void);
int FUN_004d4170(void);
void FUN_004d41e0(int p0);
int FUN_004d4380(int p0);
int FUN_004d4dd0(int p0);
int FUN_004d4f00(int p0, int p1, int p2, int p3);
int FUN_004d4f90(int p0, int p1, int p2, int p3);
int FUN_004eb3c0(void);
int FUN_004eb9d0(void);
typedef void (*PFV)(void);
void FUN_004ec720(int p0, PFV p1);
undefined4 FUN_004ec730(int p0);
void FUN_004ec740(int p0, PFV p1);
// MATCH: 0x00543710 FUN_00543710
undefined4 FUN_00543710(void)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  
  iVar1 = FUN_004d4170();
  if (iVar1 == 0) goto LAB_00543758;
  *(undefined4 *)(iVar1 + 0x2c) = 0x120;
  iVar2 = FUN_004d4dd0(iVar1);
  if (iVar2 != 0) {
    uVar3 = FUN_004eb3c0();
    uVar3 = FUN_004d4f90(iVar2,0,uVar3,0);
    FUN_004d4380(uVar3);
    goto LAB_0054375a;
  }
  FUN_004d41e0(iVar1);
LAB_00543758:
  iVar1 = 0;
LAB_0054375a:
  DAT_007dc5f4 = iVar1;
  puVar4 = (undefined4 *)FUN_004eb3c0();
  uVar3 = FUN_004d4f00(DAT_007dc5f4,*puVar4,0,0);
  DAT_007dc62c = FUN_004ec730(uVar3);
  FUN_004ec720(uVar3,FUN_00543950);
  FUN_004ec740(uVar3,FUN_00543500);
  iVar1 = FUN_004d4170();
  if (iVar1 != 0) {
    *(undefined4 *)(iVar1 + 0x2c) = 0x120;
    iVar2 = FUN_004d4dd0(iVar1);
    if (iVar2 != 0) {
      uVar3 = FUN_004eb9d0();
      uVar3 = FUN_004d4f90(iVar2,0,uVar3,0);
      FUN_004d4380(uVar3);
      goto LAB_005437e1;
    }
    FUN_004d41e0(iVar1);
  }
  iVar1 = 0;
LAB_005437e1:
  DAT_007dc5f8 = iVar1;
  puVar4 = (undefined4 *)FUN_004eb9d0();
  uVar3 = FUN_004d4f00(DAT_007dc5f8,*puVar4,0,0);
  DAT_007dc630 = FUN_004ec730(uVar3);
  FUN_004ec720(uVar3,FUN_005439d0);
  FUN_004ec740(uVar3,FUN_00543500);
  iVar1 = FUN_004cbb60();
  DAT_007dc624 = *(uint *)(iVar1 + 0x94);
  if (DAT_007dc624 >= 2) {
    DAT_007dc614 = (uint)(*(uint *)(iVar1 + 0x98) >= 2);
    DAT_007dc618 = *(uint *)(iVar1 + 0x90) & 0x1000000;
    DAT_007dc61c = *(uint *)(iVar1 + 0x90) & 0x10;
  }
  else {
    DAT_007dc614 = 0;
    DAT_007dc618 = 0;
    DAT_007dc61c = 0;
    DAT_007dc620 = 0;
    DAT_00623d30 = 0;
    DAT_00623d34 = 0;
  }
  if ((*(uint *)(iVar1 + 0xcc) & 0xffff) >= 0x101) {
    DAT_007dc628 = 1;
    FUN_004cbb20(&DAT_005e4580,&DAT_007dc5fc);
    FUN_004cbb20(&DAT_005e45e0,&DAT_007dc600);
    FUN_004cbb20(&DAT_005e4630,&DAT_007dc604);
    FUN_004cbb20(&DAT_005e4670,&DAT_007dc608);
    FUN_004cbb20(&DAT_005e46f8,&DAT_007dc60c);
    FUN_004cbb20(&DAT_005e47a8,&DAT_007dc610);
    return 1;
  }
  DAT_007dc628 = 0;
  DAT_007dc5fc = 0;
  DAT_007dc600 = 0;
  DAT_007dc604 = 0;
  DAT_007dc608 = 0;
  DAT_007dc60c = 0;
  DAT_007dc610 = 0;
  return 1;
}
