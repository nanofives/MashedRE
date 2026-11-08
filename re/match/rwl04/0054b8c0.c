// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x0054b8c0 h1: best 69.4% (regs 69.4%, ops 71.9%). Residue: orig keeps 6 unmerged copies of the failure epilogue (pop x4, add esp 8, ret), ours cross-jumps them into one (125 vs 160 ins). return 0 / return iVar6, flags /O2 /Ox /Ob0-2 /GX /Oi- tried. Class (a). */
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
int FUN_004cc6e0(int p0, int p1, int p2);
int FUN_004cc770(int p0, int p1, int p2);
int FUN_004f3d40(int *p0, int p1);
// MATCH: 0x0054b8c0 FUN_0054b8c0
int FUN_0054b8c0(int *param_1,int param_2)
{
  unsigned char *pbVar1;
  unsigned int iVar2;
  unsigned int iVar3;
  unsigned int iVar4;
  unsigned int iVar5;
  int iVar6;
  
  iVar2 = param_1[0xf];
  iVar3 = param_1[0xe];
  iVar4 = param_1[0xd];
  iVar5 = param_1[0xc];
  pbVar1 = (unsigned char *)(param_1 + 0xb);
  iVar6 = FUN_004cc770(param_2,(int)pbVar1,0x14);
  if (iVar6 == 0) {
    return 0;
  }
  if ((*pbVar1 & 1) != 0) {
    iVar6 = FUN_004cc6e0(iVar6,*param_1,iVar5 * 0xc);
    if (iVar6 == 0) {
      return 0;
    }
  }
  if ((*pbVar1 & 2) != 0) {
    iVar6 = FUN_004cc6e0(iVar6,param_1[1],iVar5 * 0xc);
    if (iVar6 == 0) {
      return 0;
    }
  }
  if ((*pbVar1 & 4) != 0) {
    iVar6 = FUN_004cc6e0(iVar6,param_1[2],iVar5 * 4);
    if (iVar6 == 0) {
      return 0;
    }
  }
  if (iVar2 != 0) {
    iVar6 = FUN_004cc6e0(iVar6,param_1[3],iVar5 * iVar2 * 8);
    if (iVar6 == 0) {
      return 0;
    }
  }
  if (iVar3 != 0) {
    iVar6 = FUN_004cc770(iVar6,param_1[0x16],iVar3 * 0x44);
    if (iVar6 == 0) {
      return 0;
    }
  }
  if (iVar4 != 0) {
    iVar6 = FUN_004cc770(iVar6,param_1[0x17],iVar4 * 0x2c);
    if (iVar6 == 0) {
      return 0;
    }
  }
  FUN_004f3d40(param_1 + 0x12,iVar6);
  return iVar6;
}
