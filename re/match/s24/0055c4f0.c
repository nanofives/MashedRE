// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x0055c4f0 (h4): 100.0% (regs 100.0%). Recursive walk: type word 8 recurses over a 0x60-byte item
   array at (param_1+0x40); otherwise stores param_2 at +0x50. A for loop with an unsigned bound gives the
   original's jbe entry guard (raw do-while 97.2%); the list pointer is a local reloaded after each call
   (re-reading through param_1 in the loop condition costs the register allocation, 35%). */
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
typedef struct { char *items; unsigned int count; } List55c4f0;
// MATCH: 0x0055c4f0 FUN_0055c4f0
void FUN_0055c4f0(int param_1, int param_2)
{
  List55c4f0 *list;
  unsigned int i;

  if (**(short **)(param_1 + 0x5c) == 8) {
    list = *(List55c4f0 **)(param_1 + 0x40);
    for (i = 0; i < list->count; i++) {
      FUN_0055c4f0((int)(list->items + i * 0x60), param_2);
      list = *(List55c4f0 **)(param_1 + 0x40);
    }
  }
  else {
    *(int *)(param_1 + 0x50) = param_2;
  }
}
