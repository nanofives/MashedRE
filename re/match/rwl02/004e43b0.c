// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e43b0: best 100.0% (regs 100.0%), residual class -. single list cursor advanced before the node body, obj struct with LL a/LL b. */
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
typedef struct LObj { LL a; int f8; LL b; int f14; } LObj;
MGLOBAL(int, DAT_007d7170, 0x007d7170);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d7164, 0x007d7164);
undefined4 FUN_004d8bd0(int p0);
// MATCH: 0x004e43b0 FUN_004e43b0
undefined4 FUN_004e43b0(undefined4 param_1,int param_2)
{
  LObj *piVar1;
  undefined4 *puVar2;
  LL *piVar3;
  puVar2 = (undefined4 *)(DAT_007d7170 + param_2);
  if (*(int *)(param_2 + 0x14) != 0) {
    FUN_004d8bd0(*(int *)(param_2 + 0x14));
  }
  piVar3 = *(LL **)(param_2 + 100);
  while (piVar3 != (LL *)(param_2 + 100)) {
    piVar1 = (LObj *)((char *)piVar3 - 0xc);
    piVar3 = piVar3->next;
    if ((piVar1->f8 != 0) && (piVar1->f14 != 0)) {
      piVar1->b.prev->next = piVar1->b.next;
      piVar1->b.next->prev = piVar1->b.prev;
      piVar1->a.prev->next = piVar1->a.next;
      piVar1->a.next->prev = piVar1->a.prev;
      (**(void (**)(int,LObj *))(DAT_007d3ff8 + 0x11c))(*(int *)(DAT_007d7164 + DAT_007d3ff8),piVar1);
    }
  }
  *puVar2 = 0;
  return param_1;
}
