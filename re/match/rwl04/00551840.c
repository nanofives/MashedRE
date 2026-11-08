// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00551840 h1: best 46.5% (regs 81.7%, ops 87.3%). Residue: orig reuses known-zero eax on post-test failure exits (3 duplicated epilogues, no xor), ours merges exits with xor eax,eax; global/ptr register allocation differs. Class (a). */
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
MGLOBAL(int, DAT_009124f8, 0x009124f8);
int FUN_004cbe80(int p0, int p1, int p2);
int FUN_004cc580(int p0, int p1, int p2, int p3, int p4);
int FUN_004cc770(int p0, int *p1, int p2);
// MATCH: 0x00551840 FUN_00551840
int FUN_00551840(int param_1,int param_2,int param_3)
{
  int *piVar1;
  unsigned int uVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  unsigned int local_4;
  
  local_4 = 0;
  piVar4 = (int *)(DAT_009124f8 + 8 + param_3);
  iVar3 = 4;
  uVar2 = 0;
  piVar1 = piVar4;
  do {
    if (*piVar1 != 0) {
      local_4 = local_4 | 1 << uVar2;
      iVar3 = iVar3 + 0x20;
    }
    uVar2 = uVar2 + 1;
    piVar1 = piVar1 + 1;
  } while (uVar2 < 8);
  iVar3 = FUN_004cc580(param_1,1,iVar3,0x37002,10);
  if (iVar3 == 0) {
    return 0;
  }
  iVar3 = FUN_004cc770(param_1,(int *)&local_4,4);
  if (iVar3 == 0) {
    return 0;
  }
  iVar3 = 8;
  do {
    iVar5 = *piVar4;
    if (iVar5 != 0) {
      FUN_004cbe80(param_1,*(int *)(*(int *)iVar5 + 0x14),0x20);
    }
    piVar4 = piVar4 + 1;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return param_1;
}
