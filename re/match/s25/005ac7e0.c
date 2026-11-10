// FLAGS: /O2 /Ob1 /G5
/* MATCH-NEARMISS-2-pilot 2026-11-09: 100% BYTE-IDENTICAL by hand. The link at param_2+0x40 is zeroed through param_2 (+0x44 then +0x40, no puVar1 pointer, which moves the cmp to its original slot) and inserted in RW list-link order: link->next = list->next, prev through param_2+0x44, list->next->prev = link, list->next = link, with a {next, prev} link type. The older note below describes the pre-pilot residual. */
/* MATCH-S25 0x005ac7e0 (h2): best 98.2% (regs 98.2%), class a. cmp eax,edi scheduled one store earlier in the original; permuter and reorder tried. relocheck BAD only because the code differs from the original. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(int, DAT_009146fc, 0x009146fc);
MGLOBAL(undefined1, DAT_007dd65c, 0x007dd65c);
void FUN_005aca40(int p0, undefined4 p1, undefined4 p2);
void FUN_005ae0b0(int p0);
uint FUN_005ae800(int * p0, undefined4 p1);
typedef struct LL { struct LL *next; struct LL *prev; } LL;
// MATCH: 0x005ac7e0 FUN_005ac7e0
int FUN_005ac7e0(int param_1,int param_2)

{
  
  if (param_2 == 0) {
    param_2 = FUN_005ae800((int *)&DAT_007dd65c,0x30805);
    if (param_2 == 0) {
      return 0;
    }
    *(undefined4 *)(param_2 + 0x48) = 2;
  }
  else {
    *(undefined4 *)(param_2 + 0x48) = 0;
  }
  FUN_005ae0b0(param_2);
  *(undefined4 *)(param_2 + 0xc) = 0;
  *(undefined4 *)(param_2 + 0x10) = 0;
  *(undefined4 *)(param_2 + 0x14) = 0;
  *(undefined4 *)(param_2 + 0x1c) = 0;
  *(undefined4 *)(param_2 + 0x18) = 0;
  *(undefined4 *)(param_2 + 0x20) = 0;
  *(undefined4 *)(param_2 + 0x34) = 0;
  *(undefined4 *)(param_2 + 0x38) = 4;
  *(undefined4 *)(param_2 + 0x3c) = 0;
  FUN_005aca40(param_2,0,0);
  *(undefined4 *)(param_2 + 0x44) = 0;
  *(undefined4 *)(param_2 + 0x40) = 0;
  if (param_1 == 0) {
    param_1 = DAT_009146fc;
  }
  *(int *)(param_2 + 0x30) = param_1;
  { LL *link = (LL *)(param_2 + 0x40); LL *list = (LL *)(param_1 + 0x20);
  link->next = list->next;
  *(LL **)(param_2 + 0x44) = list;
  list->next->prev = link;
  list->next = link; }
  return param_2;
}
