// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e4860: best 100.0% (regs 100.0%), residual class -. list node accessed as struct (LL a at +0, LL b at +0xc), single cursor advanced before the unlinks. */
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
typedef struct LObj { LL a; int pad; LL b; } LObj;
MGLOBAL(int, DAT_007d7178, 0x007d7178);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d7164, 0x007d7164);
// MATCH: 0x004e4860 FUN_004e4860
undefined4 FUN_004e4860(undefined4 param_1,int param_2)
{
  LL *piVar3;
  LObj *piVar1;
  *(undefined4 *)(DAT_007d7178 + param_2) = 0;
  piVar3 = *(LL **)(param_2 + 0x2c);
  while (piVar3 != (LL *)(param_2 + 0x2c)) {
    piVar1 = (LObj *)((char *)piVar3 - 0xc);
    piVar3 = piVar3->next;
    piVar1->b.prev->next = piVar1->b.next;
    piVar1->b.next->prev = piVar1->b.prev;
    piVar1->a.prev->next = piVar1->a.next;
    piVar1->a.next->prev = piVar1->a.prev;
    (**(void (**)(int,LObj *))(DAT_007d3ff8 + 0x11c))(*(int *)(DAT_007d7164 + 4 + DAT_007d3ff8),piVar1);
  }
  **(undefined4 **)(param_2 + 0x38) = *(undefined4 *)(param_2 + 0x34);
  *(undefined4 *)(*(int *)(param_2 + 0x34) + 4) = *(undefined4 *)(param_2 + 0x38);
  return param_1;
}
