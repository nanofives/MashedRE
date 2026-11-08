// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004dc750: 25.6% regs 63.6%.  class (b) Free-list coalesce: two inlined find loops plus merge loop. Original spills the list head into a stack slot while keeping it in ebp and uses 5 callee-saved regs; my clean-source form reaches 25.6 only (ghidra raw form 19.4). Not reached in budget.. */
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
typedef unsigned int undefined4;
typedef int code();
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(int *, DAT_007d70b0, 0x007d70b0);
MGLOBAL(undefined4, DAT_007d70b8, 0x007d70b8);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
typedef int code2(int, int);
typedef int code3(int, int);
// MATCH: 0x004dc750 FUN_004dc750
void FUN_004dc750(int param_1,int param_2,int param_3,int param_4)
{
  int *head;
  int *p;
  int *q;
  int *n;

  head = DAT_007d70b0;
  if (head == 0) {
    return;
  }
  while (param_1 != *head) {
    head = (int *)head[3];
    if (head == 0) {
      return;
    }
  }
  for (p = (int *)head[1]; p != 0; p = (int *)p[3]) {
    if (p[2] == param_3 && (p[1] + p[0] == param_4 || param_2 + param_4 == p[0])) {
      goto merge;
    }
  }
  n = (int *)(**(code2 **)(DAT_007d3ff8 + 0x118))(DAT_007d70b8,0x30411);
  n[0] = param_4;
  n[1] = param_2;
  n[2] = param_3;
  n[3] = head[1];
  n[4] = 0;
  if (head[1] != 0) {
    *(int **)(head[1] + 0x10) = n;
  }
  head[1] = (int)n;
  return;
merge:
  for (;;) {
    if (p[0] + p[1] == param_4) {
      p[1] = p[1] + param_2;
    }
    else if (param_2 + param_4 == p[0]) {
      p[0] = param_4;
      p[1] = p[1] + param_2;
    }
    param_4 = p[0];
    param_2 = p[1];
    for (q = (int *)head[1]; q != 0; q = (int *)q[3]) {
      if (q[2] == param_3 && (q[1] + q[0] == param_4 || param_2 + param_4 == q[0])) {
        break;
      }
    }
    if (q == 0) {
      return;
    }
    if (p[4] != 0) {
      *(int *)(p[4] + 0xc) = p[3];
    }
    if (p[3] != 0) {
      *(int *)(p[3] + 0x10) = p[4];
    }
    if ((int *)head[1] == p) {
      head[1] = p[3];
    }
    (**(code3 **)(DAT_007d3ff8 + 0x11c))(DAT_007d70b8,(int)p);
    p = q;
  }
}
