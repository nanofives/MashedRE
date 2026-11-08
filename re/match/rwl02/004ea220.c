// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004ea220: best 46.5% (regs 55.3%), residual class b. structure only (663 B), Ghidra gotos kept; orig frame is 0x24 vs ours 0x18 (more locals live); 46.5. */
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
typedef unsigned char byte;
uint FUN_004cbd30(uint p0, void * p1, uint p2);
undefined4 FUN_004cc5e0(uint p0, int p1, void * p2, void * p3);
undefined4 FUN_004d7ff0(undefined4 p0, ...);
undefined4 * FUN_004d8480(void * p0);
undefined4 * FUN_004e9e40(undefined4 p0, int * p1, int p2, uint p3);
void FUN_004e4f70(int p0);
void FUN_004e5020(int p0);
// MATCH: 0x004ea220 FUN_004ea220
undefined4 * FUN_004ea220(uint param_1,int *param_2,int param_3,uint param_4)

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int local_24;
  undefined4 local_20;
  undefined4 local_1c;
  int local_18 [4];
  int local_8;
  int local_4;
  
  uVar2 = param_1;
  iVar3 = FUN_004cc5e0(param_1,1,&local_24,&param_1);
  if (iVar3 == 0) {
    return (undefined4 *)0x0;
  }
  if ((param_1 < 0x35000) || (0x37002 < param_1)) {
    local_20 = 2;
    local_1c = FUN_004d7ff0(0x80000004);
    FUN_004d8480(&local_20);
    return (undefined4 *)0x0;
  }
  piVar5 = local_18;
  for (iVar3 = 6; iVar3 != 0; iVar3 = iVar3 + -1) {
    *piVar5 = 0;
    piVar5 = piVar5 + 1;
  }
  iVar4 = FUN_004cbd30(uVar2,local_18,local_24);
  iVar3 = param_3;
  piVar5 = param_2;
  if (iVar4 != local_24) {
    return (undefined4 *)0x0;
  }
  piVar1 = (int *)*param_2;
  *param_2 = (int)(piVar1 + 6);
  if (piVar1 == (int *)0x0) {
    local_20 = 2;
    local_1c = FUN_004d7ff0(0x80000013,0x18);
    FUN_004d8480(&local_20);
    return (undefined4 *)0x0;
  }
  *piVar1 = local_18[0];
  piVar1[1] = local_18[1];
  if ((param_4 & 0x40000000) == 0) {
    piVar1[4] = local_18[1];
    piVar1[5] = local_18[1];
  }
  else {
    piVar1[4] = local_8;
    piVar1[5] = local_4;
  }
  if (local_18[2] == 0) {
    iVar4 = FUN_004cc5e0(uVar2,10,0,&param_1);
    if (iVar4 == 0) goto LAB_004ea43e;
    if ((0x34fff < param_1) && (param_1 < 0x37003)) {
      iVar4 = (int)FUN_004ea220(uVar2,piVar5,iVar3,param_4);
      piVar1[2] = iVar4;
      goto joined_r0x004ea39c;
    }
  }
  else {
    iVar4 = FUN_004cc5e0(uVar2,9,0,&param_1);
    if (iVar4 == 0) goto LAB_004ea43e;
    if ((0x34fff < param_1) && (param_1 < 0x37003)) {
      iVar4 = (int)FUN_004e9e40(uVar2,piVar5,iVar3,param_4);
      piVar1[2] = iVar4;
joined_r0x004ea39c:
      if (iVar4 == 0) goto LAB_004ea43e;
      if (local_18[3] == 0) {
        iVar4 = FUN_004cc5e0(uVar2,10,0,&param_1);
        if (iVar4 == 0) goto LAB_004ea43e;
        if ((param_1 < 0x35000) || (0x37002 < param_1)) goto LAB_004ea402;
        iVar4 = (int)FUN_004ea220(uVar2,piVar5,iVar3,param_4);
      }
      else {
        iVar4 = FUN_004cc5e0(uVar2,9,0,&param_1);
        if (iVar4 == 0) goto LAB_004ea43e;
        if ((param_1 < 0x35000) || (0x37002 < param_1)) goto LAB_004ea402;
        iVar4 = (int)FUN_004e9e40(uVar2,piVar5,iVar3,param_4);
      }
      piVar1[3] = iVar4;
      if (iVar4 != 0) {
        return (undefined4 *)piVar1;
      }
      goto LAB_004ea43e;
    }
  }
LAB_004ea402:
  local_20 = 2;
  local_1c = FUN_004d7ff0(0x80000004);
  FUN_004d8480(&local_20);
LAB_004ea43e:
  if (piVar1[2] == 0) {
    return (undefined4 *)0x0;
  }
  if ((*(byte *)(iVar3 + 3) & 1) == 0) {
    FUN_004e5020(piVar1[2]);
    piVar1[2] = 0;
    return (undefined4 *)0x0;
  }
  FUN_004e4f70(piVar1[2]);
  piVar1[2] = 0;
  return (undefined4 *)0x0;
}
