// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d6e70: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs21_20261107/results.tsv. */
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
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(int, DAT_007d6c10, 0x007d6c10);
MGLOBAL(uint, DAT_00911fe0, 0x00911fe0);
MGLOBAL(int, DAT_007d6b14, 0x007d6b14);
MGLOBAL(int, DAT_007d6b10, 0x007d6b10);
MGLOBAL(int, DAT_007d58d4, 0x007d58d4);
MGLOBAL(undefined4, DAT_007d58d0, 0x007d58d0);
MGLOBAL(int, DAT_007d6c14, 0x007d6c14);
MGLOBAL(int, DAT_007d5870, 0x007d5870);
MGLOBAL(int, DAT_007d5874, 0x007d5874);
MGLOBAL(undefined1, DAT_005d8c94, 0x005d8c94);
MGLOBAL(undefined1, DAT_005d8ca8, 0x005d8ca8);
MGLOBAL(undefined1, DAT_005d8cac, 0x005d8cac);
MGLOBAL(undefined4, DAT_007d4f64, 0x007d4f64);
MGLOBAL(undefined1, DAT_007d4f68, 0x007d4f68);
MGLOBAL(undefined1, DAT_007d4f74, 0x007d4f74);
MGLOBAL(undefined4, DAT_007d4f78, 0x007d4f78);
MGLOBAL(undefined4, DAT_007d4f7c, 0x007d4f7c);
MGLOBAL(undefined1, DAT_007d4f88, 0x007d4f88);
MGLOBAL(undefined4, DAT_007d5168, 0x007d5168);
MGLOBAL(undefined4, DAT_007d6b30, 0x007d6b30);
MGLOBAL(undefined4, DAT_007d6b34, 0x007d6b34);
MGLOBAL(undefined4, DAT_007d6b38, 0x007d6b38);
MGLOBAL(undefined4, DAT_007d6b3c, 0x007d6b3c);
MGLOBAL(undefined4, DAT_007d6b44, 0x007d6b44);
undefined4 FUN_004d6ce0(int p0, int p1);
// MATCH: 0x004d6e70 FUN_004d6e70

undefined4 FUN_004d6e70(undefined4 *param_1,int param_2)

{
  byte bVar1;
  undefined4 uVar2;
  uint uVar3;
  int *piVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  
  if (param_1 == (undefined4 *)0x0) {
    if (((param_2 == 0) && (DAT_007d6b14 != 0)) && (DAT_007d6b14 = 0, DAT_007d6b10 == 0)) {
      DAT_007d58d0 = 0;
      if (DAT_007d58d4 == 0) {
        DAT_007d58d4 = 1;
        (&DAT_007d5168)[DAT_007d6c14] = 0x1b;
        DAT_007d6c14 = DAT_007d6c14 + 1;
      }
      if ((DAT_007d5870 != 0) && (DAT_007d5870 = 0, DAT_007d5874 == 0)) {
        DAT_007d5874 = 1;
        (&DAT_007d5168)[DAT_007d6c14] = 0xf;
        DAT_007d6c14 = DAT_007d6c14 + 1;
      }
    }
    if ((&DAT_007d6b30)[param_2 * 6] != 0) {
      (&DAT_007d6b30)[param_2 * 6] = 0;
      (**(code **)(*DAT_007d4110 + 0x104))(DAT_007d4110,param_2,0);
    }
    return 1;
  }
  uVar3 = (uint)param_1[0x14] >> 8 & 0xf;
  if (uVar3 != (&DAT_007d6b34)[param_2 * 6]) {
    (&DAT_007d6b34)[param_2 * 6] = uVar3;
    uVar2 = *(undefined4 *)(&DAT_005d8c94 + uVar3 * 4);
    (&DAT_007d4f64)[param_2 * 0xe] = uVar2;
    (**(code **)(*DAT_007d4110 + 0x114))(DAT_007d4110,param_2,1,uVar2);
  }
  uVar3 = (uint)param_1[0x14] >> 0xc & 0xf;
  if (uVar3 != (&DAT_007d6b38)[param_2 * 6]) {
    (&DAT_007d6b38)[param_2 * 6] = uVar3;
    uVar2 = *(undefined4 *)(&DAT_005d8c94 + uVar3 * 4);
    *(undefined4 *)(&DAT_007d4f68 + param_2 * 0x38) = uVar2;
    (**(code **)(*DAT_007d4110 + 0x114))(DAT_007d4110,param_2,2,uVar2);
  }
  piVar4 = DAT_007d4110;
  uVar3 = param_1[0x14] & 0xff;
  if (0 < DAT_007d6c10) {
    bVar1 = *(byte *)(DAT_007d6c10 + (int)param_1);
    uVar5 = (uint)bVar1;
    if ((&DAT_007d6b44)[param_2 * 6] != uVar5) {
      (&DAT_007d6b44)[param_2 * 6] = uVar5;
      *(uint *)(&DAT_007d4f88 + param_2 * 0x38) = uVar5;
      (**(code **)(*piVar4 + 0x114))(piVar4,param_2,10,uVar5);
    }
    if (1 < bVar1) {
      uVar3 = uVar3 + 7;
    }
  }
  piVar4 = DAT_007d4110;
  if ((&DAT_007d6b3c)[param_2 * 6] == uVar3) goto LAB_004d702d;
  iVar7 = *(int *)(&DAT_005d8ca8 + uVar3 * 8);
  (&DAT_007d6b3c)[param_2 * 6] = uVar3;
  uVar2 = *(undefined4 *)(&DAT_005d8cac + uVar3 * 8);
  iVar6 = param_2 * 0x38;
  if (iVar7 == 3) {
    (&DAT_007d4f78)[param_2 * 0xe] = 3;
    (**(code **)(*piVar4 + 0x114))(piVar4,param_2,6,3);
    piVar4 = DAT_007d4110;
    if ((DAT_00911fe0 & 0x4000000) == 0) {
      *(undefined4 *)(&DAT_007d4f74 + iVar6) = 2;
      iVar7 = 2;
      goto LAB_004d7005;
    }
    *(undefined4 *)(&DAT_007d4f74 + iVar6) = 3;
    (**(code **)(*piVar4 + 0x114))(piVar4,param_2,5,3);
  }
  else {
    (&DAT_007d4f78)[param_2 * 0xe] = iVar7;
    (**(code **)(*piVar4 + 0x114))(piVar4,param_2,6,iVar7);
    piVar4 = DAT_007d4110;
    *(int *)(&DAT_007d4f74 + iVar6) = iVar7;
LAB_004d7005:
    (**(code **)(*piVar4 + 0x114))(piVar4,param_2,5,iVar7);
  }
  piVar4 = DAT_007d4110;
  (&DAT_007d4f7c)[param_2 * 0xe] = uVar2;
  (**(code **)(*piVar4 + 0x114))(piVar4,param_2,7,uVar2);
LAB_004d702d:
  FUN_004d6ce0(*param_1,param_2);
  return 1;
}
