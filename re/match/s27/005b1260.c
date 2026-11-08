// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 63.0% (regs 69.2%, permuter best (240 s). Not classified beyond structure/register differences (large function, budget spent). Class: b. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
typedef long LONG;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(void *, PTR_DAT_00634288, 0x00634288);
MGLOBAL(undefined1, FUN_005b1240, 0x005b1240);
MGLOBAL(undefined1, FUN_005b1250, 0x005b1250);
MGLOBAL(undefined1, FUN_005b15e0, 0x005b15e0);
int * FUN_005ab090(int * p0, int * p1, int p2, int p3, undefined4 p4, int p5, int p6);
int FUN_005adfe0(int p0, undefined4 p1);
undefined4 * FUN_005ae010(undefined4 * p0, undefined4 p1);
void FUN_005ae0b0(undefined4 * p0);
uint FUN_005aea10(int p0, undefined4 p1);
uint FUN_005aeea0(undefined4 * p0, LONG p1, LONG p2);
void FUN_005b1500(int p0);
char * FUN_005b73b0(char * p0);
uint FUN_005b8040(undefined4 * p0, undefined4 p1, undefined2 p2);
MGLOBAL(int, s_rws_1_00634280, 0x00634280);
// MATCH: 0x005b1260 FUN_005b1260
undefined4 FUN_005b1260(undefined4 *param_1,int *param_2)

{
  char *pcVar1;
  undefined4 *puVar2;
  char cVar3;
  int iVar4;
  undefined4 uVar5;
  char *pcVar6;
  
  puVar2 = param_1 + 0x4c;
  param_1[0x62] = 0;
  *param_1 = 0;
  param_1[0x3c] = 0;
  param_1[0x3d] = 0;
  param_1[0x3e] = 0;
  param_1[0x3f] = 0;
  param_1[0x46] = 0;
  param_1[0x41] = *param_2;
  param_1[0x42] = param_2[1];
  param_1[0x43] = param_2[2];
  param_1[0x44] = param_2[0xf];
  param_1[0x45] = param_2[0x10];
  param_1[0x61] = param_2[8];
  param_1[0x60] = param_2[9];
  FUN_005ae0b0(puVar2);
  FUN_005adfe0((int)puVar2,param_2[7]);
  FUN_005ae010(puVar2,param_2[6]);
  param_1[0x5a] = param_2[0xe];
  param_1[0x5c] = param_2[0xd];
  param_1[0x51] = param_2[0xb];
  iVar4 = param_2[10];
  param_1[0x55] = 0;
  param_1[0x56] = 0;
  param_1[0x54] = 0;
  param_1[0x57] = 0;
  param_1[0x5e] = 0;
  param_1[0x58] = iVar4;
  param_1[0x53] = 0;
  param_1[0x52] = 0;
  param_1[0x5d] = 0;
  param_1[0x3b] = 0;
  iVar4 = param_2[0xc];
  param_1[0x5b] = 0;
  param_1[0x5f] = iVar4;
  if (*param_2 == 0) {
    *param_2 = (int)FUN_005b1240;
  }
  if (param_2[1] == 0) {
    param_2[1] = (int)FUN_005b1250;
  }
  iVar4 = FUN_005b8040(param_1 + 0x23,(undefined4)(param_1 + 0x27),3);
  if (iVar4 == 0) {
    return 0;
  }
  if (param_1[0x51] == 0) {
    if (param_1[0x58] != 0) {
      param_1[0x51] = iVar4;
      iVar4 = FUN_005aea10(param_1[0x58],0x3080e);
      if (iVar4 == 0) {
        FUN_005b1500((int)param_1);
        return 0;
      }
    }
  }
  else {
    param_1[0x62] = param_1[0x62] | 1;
  }
  if (param_1[0x5a] == 0) {
    if (param_1[0x5c] != 0) {
      iVar4 = FUN_005aea10(param_1[0x5c],0x3080e);
      param_1[0x5a] = iVar4;
      if (iVar4 == 0) {
        FUN_005b1500((int)param_1);
        return 0;
      }
    }
  }
  else {
    param_1[0x62] = param_1[0x62] | 2;
  }
  iVar4 = FUN_005aeea0(param_1 + 99,1,1);
  if (iVar4 != 0) {
    if (param_2[5] == 1) {
      uVar5 = (undefined4)(FUN_005b73b0((char *)param_2[4]));
      param_1[0x20] = 0x80d;
      pcVar6 = (char *)param_2[4];
      cVar3 = *pcVar6;
      param_1[0x62] = param_1[0x62] | 0x10;
      while (cVar3 != '\0') {
        if (cVar3 == '/') {
          *pcVar6 = '\\';
        }
        pcVar1 = pcVar6 + 1;
        pcVar6 = pcVar6 + 1;
        cVar3 = *pcVar1;
      }
      iVar4 = (**(code **)(DAT_007d3ff8 + 0xf0))(uVar5,&PTR_DAT_00634288);
      if ((iVar4 == 0) ||
         (iVar4 = (**(code **)(DAT_007d3ff8 + 0xf0))(uVar5,s_rws_1_00634280), iVar4 == 0)) {
        FUN_005ab090((int *)(param_1 + 1),(int *)(param_1 + 10),2,1,param_2[4],FUN_005b15e0,(int)param_1);
        return 1;
      }
    }
    return 0;
  }
  FUN_005b1500((int)param_1);
  return 0;
}
