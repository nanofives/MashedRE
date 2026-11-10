// FLAGS: /O2 /Ob1 /G5
/* MATCH-NEARMISS-2-pilot 2026-11-09: 100% BYTE-IDENTICAL by hand. List insert written in RW list-link order (next, prev, list->next->prev, list->next) with the link fields addressed from param_2 (+0x20 / +0x24) and the list typed as a {next, prev} link at param_1+0x2c; this also put lea ebp (the plugin slot) back before the mov ebx. Last statement reads DAT_007d3ff8 first (relocheck had the two globals swapped). The older note below describes the pre-pilot residual. */
/* MATCH-RWL02 h3 0x004e4450: hand-written + permuter. 98.1%, residual (a/b): position of one lea ebp,[eax+edi] relative to the mov ebx,[edi+4] (scheduling). */
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
MGLOBAL(int, DAT_007d7174, 0x007d7174);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d7164, 0x007d7164);
int FUN_004e44e0(int p0, int p1);
int FUN_004e4520(int p0, int p1);
int FUN_004e4570(int p0, int p1);
void FUN_004c0e50(int p0);
void FUN_004c4530(int p0, int p1);
void FUN_004e6710(int p0, int (*p1)(int, int), int p2);
void FUN_004e66d0(int p0, int (*p1)(int, int), int p2);
void FUN_004e6760(int p0, int (*p1)(int, int), int p2);
typedef struct LL { struct LL *next; struct LL *prev; } LL;
// MATCH: 0x004e4450 FUN_004e4450
int FUN_004e4450(int param_1, int param_2)
{
  int *piVar3;
  int iVar5;

  piVar3 = (int *)(DAT_007d7174 + param_2);
  iVar5 = *(int *)(param_2 + 4);
  { LL *list = (LL *)(param_1 + 0x2c);
  *(LL **)(param_2 + 0x20) = list->next;
  *(LL **)(param_2 + 0x24) = list;
  list->next->prev = (LL *)(param_2 + 0x20);
  list->next = (LL *)(param_2 + 0x20); }
  *(int *)(param_1 + 0x24) = *(int *)(param_1 + 0x24) + 1;
  *piVar3 = param_1;
  FUN_004e66d0(param_2, FUN_004e44e0, param_1);
  FUN_004e6760(param_2, FUN_004e4520, param_1);
  FUN_004e6710(param_2, FUN_004e4570, param_1);
  if (iVar5 != 0) {
    FUN_004c4530(iVar5 + 0x10, 0);
    FUN_004c0e50(iVar5);
  }
  piVar3[1] = *(int *)(DAT_007d3ff8 + DAT_007d7164 + 8);
  return param_1;
}


