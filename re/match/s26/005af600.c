// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005af600 (109 B). Best 100.0% (regs 100.0%), exact. callbacks as function pointers, callee 0x005b10d0 (thunk to 005b10a0) declared 4-arg cdecl. */
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
typedef void (*CB)(void);
void FUN_005af670(void);
int FUN_005af180(int p0, CB p1, int p2);
void FUN_005af200(int p0, CB p1, int p2);
void FUN_005af740(int p0, int p1, int p2);
void FUN_005b10d0(int p0, int p1, int p2, int p3);
// MATCH: 0x005af600 FUN_005af600
void FUN_005af600(int param_1,int param_2,int param_3)
{
  int iVar1;
  int uVar2;
  iVar1 = *(int *)(*(int *)(param_3 + 0xc) + 4);
  if ((iVar1 != 0) || (*(int *)(param_1 + 0x38) != 1)) {
    uVar2 = FUN_005af180(param_1,FUN_005af670,iVar1);
    FUN_005af200(uVar2,FUN_005af670,*(int *)(*(int *)(param_3 + 0xc) + 4));
  }
  FUN_005af740(param_1,param_2,*(int *)(param_1 + 0x38));
  *(int *)(*(int *)(param_3 + 0xc) + 4) = 0;
  FUN_005b10d0(*(int *)(param_1 + 0x14),*(int *)(*(int *)(param_1 + 0x14) + 0x58),0,
                     *(int *)(param_3 + 0xc));
}
