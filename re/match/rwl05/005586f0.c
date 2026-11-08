// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x005586f0 (h2): permuter-improved raw form, 62.3% (was 49.6%), class (b). C++ check FAILS (code() fn pointer arity, int[7] vs undefined4*): not fixed, budget. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef int code();
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007dc8b0, 0x007dc8b0);
MGLOBAL(int, DAT_007dc8b4, 0x007dc8b4);
MGLOBAL(int, DAT_007dc8bc, 0x007dc8bc);
MGLOBAL(int, DAT_007dc8b8, 0x007dc8b8);
undefined * FUN_00558180(undefined4 * p0, int * p1);
undefined4 FUN_00558550(void);
int FUN_00558860(int p0, uint p1, int p2, int p3, int p4, int p5, int p6, short p7, int p8);
// MATCH: 0x005586f0 FUN_005586f0
int FUN_005586f0(int param_1,char *param_2,int param_3,undefined4 param_4,undefined4 param_5)

{
  char cVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  char *pcVar5;
  uint uVar6;
  char *pcVar7;
  int aiStack_1c [7];
  
  pcVar5 = param_2;
  iVar2 = (**(code **)(DAT_007d3ff8 + 0xf4))(param_2);
  if (iVar2 == 0) {
    return 0;
  }
  FUN_00558180(aiStack_1c,&param_2);
  if ((DAT_007dc8b0 != 0) && (param_1 != DAT_007dc8b0)) {
    FUN_00558550();
  }
  uVar4 = 0xffffffff;
  pcVar7 = pcVar5;
  do {
    iVar2 = param_3;
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    cVar1 = *pcVar7;
    pcVar7 = pcVar7 + 1;
  } while (cVar1 != '\0');
  do {
    uVar4 = ~uVar4 - 1;
    DAT_007dc8b0 = param_1;
    if (uVar4 == 0) {
LAB_00558835:
      if (DAT_007dc8b4 == 0) {
        DAT_007dc8b0 = 0;
      }
      return param_1;
    }
    uVar6 = 0x400 - DAT_007dc8b4;
    param_3 = iVar2;
    if (uVar4 < uVar6) {
      iVar2 = FUN_00558860(pcVar5,uVar4,param_1,iVar2,param_4,DAT_007dc8b4 * 0x70 + DAT_007dc8b8,
                           DAT_007dc8bc + DAT_007dc8b4 * 0xc,DAT_007dc8b4 * 4,param_5);
      DAT_007dc8b4 = DAT_007dc8b4 + iVar2;
      goto LAB_00558835;
    }
    iVar3 = FUN_00558860(pcVar5,uVar6,param_1,iVar2,param_4,DAT_007dc8b4 * 0x70 + DAT_007dc8b8,
                         DAT_007dc8bc + DAT_007dc8b4 * 0xc,DAT_007dc8b4 * 4,param_5);
    DAT_007dc8b4 = DAT_007dc8b4 + iVar3;
    FUN_00558550();
    pcVar5 = pcVar5 + uVar6;
    uVar4 = 0xffffffff;
    iVar2 = aiStack_1c[0] + iVar2 * uVar6;
    pcVar7 = pcVar5;
    do {
      if (uVar4 == 0) break;
      uVar4 = uVar4 - 1;
      cVar1 = *pcVar7;
      pcVar7 = pcVar7 + 1;
    } while (cVar1 != '\0');
  } while( true );
}
