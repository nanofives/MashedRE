// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR01 2026-11-08: 78.6% (regs 78.6%), was 70.6%. head then cur, compare head != cur, early return param_1 inside the loop; remaining: the original loads param_1 into eax before the pushes and uses lea for head (ours keeps it in edi). Class (b). */
/* MATCH-RWL02 h3 0x004e5bf0: hand-written. */
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
// MATCH: 0x004e5bf0 FUN_004e5bf0
int FUN_004e5bf0(int param_1, int (*param_2)(int, int), int param_3)
{
  int *head;
  int *cur;
  int *next;
  head = (int *)(param_1 + 0x2c);
  cur = *(int **)(param_1 + 0x2c);
  while (head != cur) {
    next = (int *)*cur;
    if (param_2((int)(cur - 8), param_3) == 0) return param_1;
    cur = next;
  }
  return param_1;
}

