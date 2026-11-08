// FLAGS: /O2 /Ob1 /G5
/* best 88.9% (regs 88.9%, ops 98.6%): hand-typed structs V/O/O2/R for the voice/owner objects (zero register ebx + dec/flags form came from
   iVar2 = *p - 1). Residual: original compares the count with the zero register (cmp eax,ebx; jbe) while ours emits cmp eax,1; jb
   for the form that gets the ebx/edi assignment right (cnt > 0 gives the compare but swaps ebx/edi). Class (b). */
/* MATCH-S25 0x005a6cb0: hand-matched (h1). */
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

typedef struct O2 { undefined4 a[16]; uint w; } O2;
typedef struct O { O2 *k; undefined4 a[7]; uint x8; undefined4 pad; uint cnt; } O;
typedef struct V { O *o; uint x1; uint *prev; uint *link; uint flags; uint cnt; } V;
typedef struct R { undefined4 a; undefined4 b; V *c; uint d; } R;
void FUN_005a6c90(V *p0);
void FUN_005aaa00(O *p0, undefined4 p1, undefined4 p2);
undefined4 FUN_005ad2e0(uint p0);
// MATCH: 0x005a6cb0 FUN_005a6cb0
void FUN_005a6cb0(V *param_1,R *param_2)
{
  O *piVar1;
  int iVar2;
  uint *piVar3;
  R local;
  local.a = 0;
  local.b = 0;
  local.c = 0;
  local.d = 0;
  if (param_2 == 0) {
    param_2 = &local;
  }
  if (param_1->cnt >= 1) {
    param_1->cnt = param_1->cnt - 1;
    return;
  }
  if (param_1->x1 != 0) {
    iVar2 = FUN_005ad2e0(param_1->x1);
    if (iVar2 == 1) {
      param_2->d = param_1->x1;
    }
  }
  piVar1 = param_1->o;
  if (piVar1->x8 != 0) {
    piVar3 = (uint *)((piVar1->k->w & 0xfffffff) + piVar1->x8);
  }
  else {
    piVar3 = &piVar1->cnt;
  }
  iVar2 = *piVar3 - 1;
  *piVar3 = iVar2;
  if ((iVar2 == 0) && ((param_1->flags & 2) == 0)) {
    FUN_005aaa00(piVar1,param_2->a,param_2->b);
  }
  *(uint *)param_1->link = (uint)param_1->prev;
  *(uint *)((uint)param_1->prev + 4) = (uint)param_1->link;
  FUN_005a6c90(param_1);
  param_2->c = param_1;
}
