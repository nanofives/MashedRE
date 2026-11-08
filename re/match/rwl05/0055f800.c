// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055f800: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl05_20261107/results.tsv. */
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
MGLOBAL(undefined1, FUN_005697f0, 0x005697f0);
MGLOBAL(undefined1, DAT_005cc56c, 0x005cc56c);
MGLOBAL(undefined1, FUN_0056b310, 0x0056b310);
MGLOBAL(undefined1, FUN_0056a450, 0x0056a450);
MGLOBAL(undefined1, FUN_0056adb0, 0x0056adb0);
MGLOBAL(undefined1, FUN_00569140, 0x00569140);
void FUN_0055f480(int p0, int p1, int p2);
uint FUN_0055f520(uint * p0);
int FUN_0055f670(uint * p0);
void FUN_00564100(undefined4 * p0, undefined1 p1, uint p2);
undefined4 FUN_00564130(int p0, uint p1, uint p2);
void FUN_00567350(uint p0, int * p1, uint * p2, uint * p3, int p4, int p5, int p6);
void FUN_005676f0(int p0, int * p1, int * p2, uint * p3, int p4, int p5, int p6, int p7);
char FUN_00567c40(void);
void FUN_00568860(int p0, uint * p1, undefined4 p2, int p3);
void FUN_00568c80(int p0, uint * p1, int p2, int p3, int p4);
uint * FUN_00568d20(int p0, uint p1);
MGLOBAL(int, _DAT_005cc56c, 0x005cc56c);
// MATCH: 0x0055f800 FUN_0055f800
/* [C1 2026-05-19] **Top-level solver-context initializer.** Sets up a 0x424-byte (1060-byte)
   physics-solver context for a body-system
 */

int FUN_0055f800(int param_1,undefined4 param_2,uint *param_3,float param_4)

{
  float fVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 *puVar4;
  uint uVar5;
  int iVar6;
  undefined4 *puVar7;
  
  *(uint *)(param_1 + 0x6c) = *param_3;
  uVar2 = FUN_0055f520(param_3);
  *(undefined4 *)(param_1 + 0x410) = uVar2;
  uVar2 = FUN_00564130(uVar2,0x10,0x30900);
  *(undefined4 *)(param_1 + 0x40c) = uVar2;
  *(undefined4 *)(param_1 + 0x414) = uVar2;
  iVar3 = FUN_0055f670(param_3);
  iVar3 = iVar3 - *(int *)(param_1 + 0x410);
  *(int *)(param_1 + 0x41c) = iVar3;
  uVar2 = FUN_00564130(iVar3,0x10,0x30900);
  puVar7 = (undefined4 *)(param_1 + 0x418);
  *(undefined4 *)(param_1 + 0x420) = uVar2;
  *puVar7 = uVar2;
  uVar2 = FUN_00567c40();
  fVar1 = param_4 * _DAT_005cc56c;
  *(undefined4 *)(param_1 + 0x58) = uVar2;
  *(undefined4 *)(param_1 + 0x54) = 0;
  *(undefined4 *)(param_1 + 0x50) = 0;
  *(undefined4 *)(param_1 + 0x4c) = 0;
  *(float *)(param_1 + 0xc) = fVar1;
  *(undefined4 *)(param_1 + 0x10) = 0x3f19999a;
  *(undefined4 *)(param_1 + 0x18) = 0x461c4000;
  *(undefined4 *)(param_1 + 0x1c) = 0x3d4ccccd;
  *(undefined4 *)(param_1 + 0x24) = 0x3d4ccccd;
  *(undefined4 *)(param_1 + 0x28) = 0xbe23d70a;
  *(float *)(param_1 + 0x14) = param_4 + param_4;
  uVar5 = param_3[0xf];
  *(undefined4 *)(param_1 + 0x5c) = 0x3727c5ac;
  *(undefined4 *)(param_1 + 0x60) = 0x3727c5ac;
  *(uint *)(param_1 + 0x20) = uVar5;
  *(undefined4 *)(param_1 + 4) = 100;
  *(undefined4 *)(param_1 + 8) = 300;
  *(undefined4 *)(param_1 + 0x68) = 1;
  *(undefined4 *)(param_1 + 0x44) = 400;
  *(undefined4 *)(param_1 + 0x48) = 400;
  *(undefined4 *)(param_1 + 0x70) = param_2;
  uVar5 = *(int *)(param_1 + 0x420) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 0x420) = uVar5;
  *(uint *)(param_1 + 0x74) = uVar5;
  FUN_00564100(*(undefined4 *)(param_1 + 0x420),0,param_3[4] << 2);
  *(uint *)(param_1 + 0x420) = *(int *)(param_1 + 0x420) + 0xf + param_3[4] * 4 & 0xfffffff0;
  *(uint *)(param_1 + 0x7c) = param_3[4];
  *(undefined4 *)(param_1 + 0x78) = 0;
  uVar5 = *(int *)(param_1 + 0x420) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 0x420) = uVar5;
  *(uint *)(param_1 + 0x80) = uVar5;
  FUN_00564100(*(undefined4 *)(param_1 + 0x420),0,param_3[7] * 0x14);
  *(uint *)(param_1 + 0x420) = *(int *)(param_1 + 0x420) + 0xf + param_3[7] * 0x14 & 0xfffffff0;
  *(uint *)(param_1 + 0x88) = param_3[7];
  *(undefined4 *)(param_1 + 0x84) = 0;
  uVar5 = *(int *)(param_1 + 0x420) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 0x420) = uVar5;
  *(uint *)(param_1 + 0xb4) = uVar5;
  FUN_00564100(*(undefined4 *)(param_1 + 0x420),0,param_3[3] << 2);
  *(uint *)(param_1 + 0x420) = *(int *)(param_1 + 0x420) + 0xf + param_3[3] * 4 & 0xfffffff0;
  *(uint *)(param_1 + 0xbc) = param_3[3];
  *(undefined4 *)(param_1 + 0xb8) = 0;
  uVar2 = FUN_00568d20(puVar7,param_3[8]);
  *(undefined4 *)(param_1 + 0xc0) = uVar2;
  puVar4 = puVar7;
  if ((*param_3 & 8) != 0) {
    *(undefined4 *)(param_1 + 0x414) = *(undefined4 *)(param_1 + 0x40c);
    puVar4 = (undefined4 *)(param_1 + 0x40c);
  }
  FUN_00568860(puVar4,param_1 + 0x37c,param_1,param_3);
  *(uint *)(param_1 + 1000) = param_3[8];
  uVar5 = *(int *)(param_1 + 0x420) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 0x420) = uVar5;
  *(uint *)(param_1 + 0x3e0) = uVar5;
  *(uint *)(param_1 + 0x420) = *(int *)(param_1 + 0x420) + 0xf + param_3[8] * 0x14 & 0xfffffff0;
  *(undefined4 *)(param_1 + 0x3e4) = 0;
  puVar4 = puVar7;
  if (((*param_3 & 2) != 0) && (puVar4 = (undefined4 *)(param_1 + 0x40c), (*param_3 & 8) == 0)) {
    *(undefined4 *)(param_1 + 0x414) = *puVar4;
  }
  uVar5 = puVar4[2] + 0xf & 0xfffffff0;
  puVar4[2] = uVar5;
  *(uint *)(param_1 + 0xd8) = uVar5;
  uVar5 = puVar4[2] + 0x3f & 0xfffffff0;
  puVar4[2] = uVar5;
  uVar5 = (uVar5 - 0x19) + param_3[4] * 0x28 & 0xfffffff0;
  puVar4[2] = uVar5;
  *(uint *)(param_1 + 0xdc) = uVar5;
  puVar4[2] = param_3[2] * 0x10 + 0xf + puVar4[2] & 0xfffffff0;
  *(uint *)(param_1 + 0xe0) = param_3[2];
  uVar5 = *(int *)(param_1 + 0x420) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 0x420) = uVar5;
  *(uint *)(param_1 + 0xcc) = uVar5;
  FUN_00564100(*(undefined4 *)(param_1 + 0x420),0,param_3[4] << 2);
  uVar5 = *(int *)(param_1 + 0x420) + 0xf + param_3[4] * 4 & 0xfffffff0;
  *(uint *)(param_1 + 0x420) = uVar5;
  *(uint *)(param_1 + 0xd0) = uVar5;
  FUN_00564100(*(undefined4 *)(param_1 + 0x420),0,param_3[3] << 2);
  uVar5 = *(int *)(param_1 + 0x420) + 0xf + param_3[3] * 4 & 0xfffffff0;
  *(uint *)(param_1 + 0x420) = uVar5;
  *(uint *)(param_1 + 0xd4) = uVar5;
  FUN_00564100(*(undefined4 *)(param_1 + 0x420),0,param_3[4] * 0x28);
  *(uint *)(param_1 + 0x420) = *(int *)(param_1 + 0x420) + 0xf + param_3[4] * 0x28 & 0xfffffff0;
  *(uint *)(param_1 + 0xc4) = param_3[4];
  *(undefined4 *)(param_1 + 200) = 0;
  puVar4 = puVar7;
  if (((*param_3 & 4) != 0) && (puVar4 = (undefined4 *)(param_1 + 0x40c), (*param_3 & 10) == 0)) {
    *(undefined4 *)(param_1 + 0x414) = *puVar4;
  }
  FUN_0055f480(puVar4,param_1 + 0x1c,param_3);
  puVar4 = puVar7;
  if ((*param_3 & 1) != 0) {
    *(undefined4 *)(param_1 + 0x414) = *(undefined4 *)(param_1 + 0x40c);
    puVar4 = (undefined4 *)(param_1 + 0x40c);
  }
  FUN_00568c80(puVar4,param_1 + 0xe4,param_3[2],param_3[3],param_3[5]);
  uVar5 = *(int *)(param_1 + 0x420) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 0x420) = uVar5;
  *(uint *)(param_1 + 0x8c) = uVar5;
  FUN_00564100(*(undefined4 *)(param_1 + 0x420),0,param_3[8] * 0xe0);
  *(uint *)(param_1 + 0x420) = param_3[8] * 0xe0 + 0xf + *(int *)(param_1 + 0x420) & 0xfffffff0;
  *(uint *)(param_1 + 0x94) = param_3[8];
  *(undefined4 *)(param_1 + 0x90) = 0;
  puVar4 = puVar7;
  if ((*param_3 & 0x20) != 0) {
    *(undefined4 *)(param_1 + 0x414) = *(undefined4 *)(param_1 + 0x40c);
    puVar4 = (undefined4 *)(param_1 + 0x40c);
  }
  uVar5 = puVar4[2] + 0xf & 0xfffffff0;
  puVar4[2] = uVar5;
  *(uint *)(param_1 + 0x9c) = uVar5;
  FUN_00564100(puVar4[2],0,param_3[8] << 2);
  puVar4[2] = puVar4[2] + 0xf + param_3[8] * 4 & 0xfffffff0;
  *(uint *)(param_1 + 0xa4) = param_3[8];
  *(undefined4 *)(param_1 + 0xa0) = 0;
  uVar5 = puVar4[2] + 0xf & 0xfffffff0;
  puVar4[2] = uVar5;
  *(uint *)(param_1 + 0xa8) = uVar5;
  FUN_00564100(puVar4[2],0,param_3[8] << 4);
  puVar4[2] = param_3[8] * 0x10 + 0xf + puVar4[2] & 0xfffffff0;
  *(uint *)(param_1 + 0xb0) = param_3[8] << 2;
  *(undefined4 *)(param_1 + 0xac) = 0;
  puVar4 = puVar7;
  if (((*param_3 & 0x10) != 0) && (puVar4 = (undefined4 *)(param_1 + 0x40c), (*param_3 & 0x20) == 0)
     ) {
    *(undefined4 *)(param_1 + 0x414) = *puVar4;
  }
  FUN_005676f0(puVar4,param_1 + 0xfc,param_1 + 0x168,param_1 + 0x400,param_3[10] << 2,param_3[5],
               param_3[6] + param_3[8],param_3[4]);
  FUN_00567350(puVar4,param_1 + 0x364,param_1 + 0x2c4,param_1 + 0x3fc,param_3[10] << 2,param_3[5],
               param_3[4]);
  if (((*param_3 & 0x40) != 0) && (puVar7 = (undefined4 *)(param_1 + 0x40c), (*param_3 & 0x30) == 0)
     ) {
    *(undefined4 *)(param_1 + 0x414) = *puVar7;
  }
  uVar5 = puVar7[2] + 0xf & 0xfffffff0;
  puVar7[2] = uVar5;
  *(uint *)(param_1 + 0x98) = uVar5;
  FUN_00564100(puVar7[2],0,param_3[8] << 6);
  uVar5 = 0;
  puVar7[2] = param_3[8] * 0x40 + 0xf + puVar7[2] & 0xfffffff0;
  if (param_3[8] != 0) {
    iVar6 = 0;
    iVar3 = 0;
    do {
      uVar5 = uVar5 + 1;
      *(int *)(iVar6 + 0xb0 + *(int *)(param_1 + 0x8c)) = *(int *)(param_1 + 0x98) + iVar3;
      iVar3 = iVar3 + 0x40;
      iVar6 = iVar6 + 0xe0;
    } while (uVar5 < param_3[8]);
  }
  *(uint *)(param_1 + 0x44) = param_3[0xb];
  *(uint *)(param_1 + 0x48) = param_3[10];
  *(undefined4 *)(param_1 + 100) = 0;
  *(code **)(param_1 + 0x404) = FUN_0056b310;
  if (*(int *)(param_1 + 0x58) != 2) {
    *(code **)(param_1 + 0xf4) = FUN_00569140;
    *(code **)(param_1 + 0xf8) = FUN_005697f0;
    return param_1;
  }
  *(code **)(param_1 + 0xf4) = FUN_0056a450;
  *(code **)(param_1 + 0xf8) = FUN_0056adb0;
  return param_1;
}
