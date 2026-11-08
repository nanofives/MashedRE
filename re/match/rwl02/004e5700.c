// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR01 2026-11-08: 100% (regs 100%), relocheck OK, was 89.8%. Same stack-walk shape as 004e55d0, globals as DAT_007d3ff8 + DAT_007d71cc, and the flag byte test before the +0x1c load (the +0x1c field is read inside each branch). Supersedes the header below. */
/* MATCH-RWL02 h1 0x004e5700: best 89.8% (regs 92.9%), residual class b. tail duplication of the two RwFrame destroy paths and register allocation of the tree walk; permuter 89.8. */
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

typedef struct LL { struct LL *next; struct LL *prev; } LL;
typedef struct WObj { int owner; int f4; LL link; } WObj;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d71cc, 0x007d71cc);
MGLOBAL(undefined4, DAT_0061864c, 0x0061864c);
int FUN_004d8060(undefined4 *p0, int p1);
undefined4 FUN_004f0c10(int p0);
int * FUN_004f3b60(int *p0);
void FUN_004e4f70(int p0);
void FUN_004e5020(int p0);
// MATCH: 0x004e5700 FUN_004e5700
undefined4 FUN_004e5700(int param_1)
{
  LL *piVar1;
  LL *piVar2;
  WObj *obj;
  int *piVar3;
  int *piVar4;
  int iVar3;
  int aiStack_100 [64];
  piVar1 = (LL *)(DAT_007d3ff8 + DAT_007d71cc + 4);
  piVar2 = piVar1->next;
  while (piVar2 != piVar1) {
    obj = (WObj *)((char *)piVar2 - 8);
    if (obj->owner == param_1) {
      obj->link.prev->next = obj->link.next;
      obj->link.next->prev = obj->link.prev;
      (**(void (**)(int,WObj *))(DAT_007d3ff8 + 0x11c))(*(int *)(DAT_007d3ff8 + DAT_007d71cc),obj);
      break;
    }
    piVar2 = piVar2->next;
  }
  iVar3 = 0;
  piVar3 = *(int **)(param_1 + 0x1c);
  if (piVar3 != (int *)0) {
    do {
      if (*piVar3 < 0) {
        if (piVar3[0x1e] != 0) {
          FUN_004f0c10(piVar3[0x1e]);
          piVar3[0x1e] = 0;
        }
        piVar3 = (int *)aiStack_100[iVar3--];
      }
      else {
        piVar4 = piVar3;
        piVar3 = (int *)piVar3[2];
        aiStack_100[++iVar3] = piVar4[3];
      }
    } while (iVar3 >= 0);
  }
  FUN_004f3b60((int *)(param_1 + 0x10));
  if ((*(byte *)(param_1 + 3) & 1) != 0) {
    iVar3 = *(int *)(param_1 + 0x1c);
    if (iVar3 != 0) {
      FUN_004e4f70(iVar3);
    }
    FUN_004d8060(&DAT_0061864c,param_1);
    (**(void (**)(int))(DAT_007d3ff8 + 0x10c))(param_1);
    return 1;
  }
  if (*(int *)(param_1 + 0x1c) != 0) {
    FUN_004e5020(*(int *)(param_1 + 0x1c));
  }
  FUN_004d8060(&DAT_0061864c,param_1);
  (**(void (**)(int))(DAT_007d3ff8 + 0x10c))(param_1);
  return 1;
}
