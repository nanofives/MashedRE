// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004dc5b0: 80.7% regs 93.6%.  class (b) COM call is __stdcall (no add esp) and compare orientation fixed (+4.3). Remaining: third alloc block register scheduling (original loads DAT_007d70bc into eax before the DAT_007d3ff8 load) and the final remaining-size update/test (unsigned ja form).. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef int code2(int, int);
typedef int (__stdcall *cm_t)(int *, int, int, int, int, int *, int);
MGLOBAL(uint *, DAT_007d70b0, 0x007d70b0);
MGLOBAL(undefined4, DAT_007d70b4, 0x007d70b4);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_007d70b8, 0x007d70b8);
MGLOBAL(int, DAT_00618434, 0x00618434);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(undefined4, DAT_007d70bc, 0x007d70bc);
// MATCH: 0x004dc5b0 FUN_004dc5b0
undefined4 FUN_004dc5b0(uint param_1,uint param_2,int *param_3,int *param_4)

{
  uint uVar1;
  int iVar2;
  uint *puVar3;
  int *piVar4;
  int iVar5;
  int *piVar6;
  
  for (puVar3 = DAT_007d70b0; puVar3 != (uint *)0x0; puVar3 = (uint *)puVar3[3]) {
    if (param_1 == *puVar3) goto LAB_004dc602;
  }
  puVar3 = (uint *)(**(code2 **)(DAT_007d3ff8 + 0x118))(DAT_007d70b4,0x30411);
  *puVar3 = param_1;
  puVar3[1] = 0;
  puVar3[2] = 0;
  puVar3[3] = (uint)DAT_007d70b0;
  DAT_007d70b0 = puVar3;
LAB_004dc602:
  for (piVar4 = (int *)puVar3[1]; piVar4 != (int *)0x0; piVar4 = (int *)piVar4[3]) {
    if ((uint)piVar4[1] >= param_2) goto LAB_004dc6d7;
  }
  piVar4 = (int *)(**(code2 **)(DAT_007d3ff8 + 0x118))(DAT_007d70b8,0x30411);
  *piVar4 = 0;
  param_1 = ((DAT_00618434 + -1 + param_1) / param_1) * param_1;
  piVar4[1] = param_1;
  if (param_2 >= param_1) {
    piVar4[1] = param_2;
  }
  iVar5 = (*(cm_t *)(*DAT_007d4110 + 0x68))(DAT_007d4110,piVar4[1],8,0,1,piVar4 + 2,0);
  if (iVar5 != 0) {
    (**(code2 **)(DAT_007d3ff8 + 0x11c))(DAT_007d70b8,(int)piVar4);
    return 0;
  }
  uVar1 = puVar3[1];
  piVar4[4] = 0;
  piVar4[3] = uVar1;
  if (puVar3[1] != 0) {
    *(int **)(puVar3[1] + 0x10) = piVar4;
  }
  puVar3[1] = (uint)piVar4;
  piVar6 = (int *)(**(code2 **)(DAT_007d3ff8 + 0x118))(DAT_007d70bc,0x30411);
  *piVar6 = piVar4[2];
  piVar6[1] = puVar3[2];
  puVar3[2] = (uint)piVar6;
LAB_004dc6d7:
  *param_3 = piVar4[2];
  iVar5 = *piVar4;
  *param_4 = iVar5;
  iVar2 = *piVar4;
  iVar5 = iVar5 + param_2;
  *piVar4 = iVar5;
  iVar5 = piVar4[1] + (iVar2 - iVar5);
  piVar4[1] = iVar5;
  if (iVar5 == 0) {
    if (piVar4[4] != 0) {
      *(int *)(piVar4[4] + 0xc) = piVar4[3];
    }
    if (piVar4[3] != 0) {
      *(int *)(piVar4[3] + 0x10) = piVar4[4];
    }
    if ((int *)puVar3[1] == piVar4) {
      puVar3[1] = piVar4[3];
    }
    (**(code2 **)(DAT_007d3ff8 + 0x11c))(DAT_007d70b8,(int)piVar4);
  }
  return 1;
}
