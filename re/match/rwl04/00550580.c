// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00550580 h1: best 93.5% (regs 93.5%, ops 97.8%). Residue: orig 'xor esi,esi; mov eax,esi' on the fail return (variable kept), ours folds to xor eax,eax. File created by hand (no raw converter file existed). Class (a). */
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
typedef int (*code)();
typedef void (*cb1)(int);
MGLOBAL(cb1, DAT_007dc768, 0x007dc768);
MGLOBAL(int, DAT_007dc764, 0x007dc764);
MGLOBAL(char, DAT_009124e0, 0x009124e0);
__declspec(dllimport) void __stdcall EnterCriticalSection(void *p);
__declspec(dllimport) void __stdcall LeaveCriticalSection(void *p);
// MATCH: 0x00550580 FUN_00550580
int FUN_00550580(int param_1,int param_2,unsigned int param_3,int param_4,int param_5)
{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar3 = 0;
  if (param_1 != 0) {
    EnterCriticalSection(&DAT_009124e0);
    iVar2 = 0;
    if (0 < *(int *)(param_1 + 4)) {
      do {
        iVar1 = (*(int (**)(int, int))(param_1 + 0x14))(param_1,iVar2);
        if (*(int *)(iVar1 + 0x20) == 1) {
          *(int *)(iVar1 + 0x20) = 5;
          iVar3 = iVar1;
          break;
        }
        iVar2 = iVar2 + 1;
      } while (iVar2 < *(int *)(param_1 + 4));
    }
    LeaveCriticalSection(&DAT_009124e0);
    if (iVar3 == 0) {
      DAT_007dc764 = 8;
      if (DAT_007dc768 != 0) {
        (*DAT_007dc768)(8);
      }
      return 0;
    }
    if (((param_3 & 0x10) != 0) || (*(int *)(iVar3 + 0x18) != 0)) {
      *(int *)(iVar3 + 0x28) = 2;
      *(int *)(iVar3 + 0x30) = param_4;
      *(int *)(iVar3 + 0x34) = param_5;
    }
    if (*(int *)(param_1 + 0x10) != 0) {
      param_3 = param_3 | 0x10;
      *(int *)(iVar3 + 0x28) = 2;
      *(int *)(iVar3 + 0x30) = param_4;
      *(int *)(iVar3 + 0x34) = param_5;
    }
    iVar2 = (*(int (**)(int, int, int, unsigned int))(param_1 + 0x28))(param_1,iVar3,param_2,param_3);
    if (iVar2 != 1) {
      *(int *)(iVar3 + 0x20) = 1;
      iVar3 = 0;
      return iVar3;
    }
    *(int *)(iVar3 + 0x1c) = 2;
    return iVar3;
  }
  else {
    if (DAT_007dc768 != 0) {
      (*DAT_007dc768)(4);
    }
  }
  return 0;
}
