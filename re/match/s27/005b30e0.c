// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005b30e0: 16.7% (regs 40.6%), raw Ghidra shape (LPLONG noise fixed); not iterated, budget spent on small rows. Class: b.
/* MATCH-S27 0x005b30e0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs27_20261107/results.tsv. */
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
typedef unsigned short ushort;
typedef unsigned char byte;
typedef unsigned __int64 ulonglong;
typedef void *HANDLE;
typedef int BOOL;
typedef unsigned long DWORD;
typedef long LONG;
void FUN_005aeda0(uint p0, uint p1, uint * p2, uint * p3);
uint FUN_005b3020(int p0);
undefined4 FUN_005b8090(int * p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5);
__declspec(dllimport) BOOL __stdcall ReleaseSemaphore(HANDLE p0, LONG p1, long *p2);
__declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE p0, DWORD p1);
/* converter noise: extern int LPLONG; dropped (no address) */
// MATCH: 0x005b30e0 FUN_005b30e0
undefined4 FUN_005b30e0(uint param_1,uint param_2,uint param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  undefined4 uVar6;
  uint uVar7;
  uint uVar8;
  ushort *puVar9;
  
  uVar8 = param_3;
  iVar3 = param_1;
  iVar5 = *(int *)(*(int *)(param_1 + 0x118) + 0x18);
  uVar7 = *(uint *)(iVar5 + 0x20 + param_2 * 0x28);
  iVar5 = iVar5 + param_2 * 0x28;
  uVar4 = param_3 / uVar7;
  FUN_005aeda0(uVar4,uVar7,&param_3,&param_2);
  param_3 = uVar8 - param_2;
  if ((*(byte *)(*(int *)(iVar5 + 0x14) + 0x18) & 4) == 0) {
    param_2 = (uint)*(byte *)(*(int *)(iVar5 + 0x14) + 0xd);
  }
  else {
    param_2 = 1;
  }
  uVar7 = param_2;
  uVar8 = *(ushort *)(iVar5 + 0x1a) * param_2;
  param_1 = param_3;
  if (param_3 % uVar8 != 0) {
    param_1 = (((uVar8 - 1) + param_3) / uVar8) * (uint)*(ushort *)(iVar5 + 0x1a) * param_2;
  }
  param_1 = param_1 / (*(ushort *)(iVar5 + 0x1a) * param_2);
  FUN_005aeda0(param_1,(int)(0xffffffff /
                            ((ulonglong)*(uint *)(iVar5 + 0x20) /
                            (ulonglong)(*(ushort *)(iVar5 + 0x1a) * param_2))),&param_2,&param_1);
  if ((param_1 & 0xfffffffe) != 0) {
    param_1 = param_1 + 1;
  }
  iVar5 = *(int *)(iVar3 + 0x118);
  if (*(uint *)(iVar5 + 0x14) != 0) {
    puVar9 = (ushort *)(*(int *)(iVar5 + 0x18) + 0x1a);
    param_2 = *(uint *)(iVar5 + 0x14);
    do {
      uVar8 = *puVar9 * uVar7;
      if (param_3 % uVar8 != 0) {
        param_3 = (((uVar8 - 1) + param_3) / uVar8) * uVar8;
      }
      puVar9 = puVar9 + 0x14;
      param_2 = param_2 - 1;
    } while (param_2 != 0);
  }
  iVar1 = *(int *)(iVar5 + 0x20);
  iVar2 = *(int *)(*(int *)(iVar3 + 0x140) + 0x1c);
  iVar5 = *(int *)(iVar5 + 0x24);
  WaitForSingleObject(*(HANDLE *)(iVar3 + 0x18c),0xffffffff);
  iVar5 = FUN_005b8090((int *)(iVar3 + 0x8c),0,0,iVar1 * uVar4 + iVar2 + iVar5,0,1);
  if (iVar5 == 0) {
    ReleaseSemaphore(*(HANDLE *)(iVar3 + 0x18c),1,(long *)0);
    return 0;
  }
  if (*(int *)(iVar3 + 0x13c) != *(int *)(iVar3 + 0x140)) {
    *(int *)(iVar3 + 0x13c) = *(int *)(iVar3 + 0x140);
    uVar6 = FUN_005b3020(iVar3);
    *(undefined4 *)(iVar3 + 0xec) = uVar6;
  }
  *(uint *)(iVar3 + 0xf4) = param_1;
  uVar7 = *(uint *)(*(int *)(iVar3 + 0x13c) + 0x18) / *(uint *)(*(int *)(iVar3 + 0x118) + 0x20);
  *(uint *)(iVar3 + 0x100) = uVar7;
  *(uint *)(iVar3 + 0xf8) = uVar7 - uVar4;
  *(uint *)(iVar3 + 0xfc) = *(int *)(iVar3 + 0x100) - uVar4;
  ReleaseSemaphore(*(HANDLE *)(iVar3 + 0x18c),1,(long *)0);
  return 1;
}
