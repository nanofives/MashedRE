// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005ae920 (211 B). Best 96.8% (regs 100.0%), residual class (a). regs 100% ops 100%: only SIB base/index order differs in 3 byte accesses ([edx+esi+8] vs orig [esi+edx+8]); tried (int) char-pointer and node+2 addressing spellings and permuter 200s. */
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
void FUN_004522d0(int p0);
// MATCH: 0x005ae920 FUN_005ae920
void FUN_005ae920(unsigned int *param_1,unsigned int param_2)
{
  unsigned int *node;
  unsigned int *head;
  unsigned int nbytes;
  unsigned int ds;
  unsigned int idx;
  unsigned int by;
  unsigned int *first;

  node = (unsigned int *)param_1[4];
  nbytes = param_1[2];
  head = param_1 + 4;
  if (node != head) {
    while (ds = nbytes + 8 + (unsigned int)node, param_2 < ds || param_2 > param_1[1] * param_1[0] + ds) {
      node = (unsigned int *)*node;
      if (node == head) return;
    }
    idx = (param_2 - ds) / param_1[0];
    by = idx >> 3;
    *(unsigned char *)((int)node + by + 8) = *(unsigned char *)((int)node + by + 8) & ~(0x80 >> (idx - by * 8));
    *(unsigned int *)node[1] = *node;
    *(unsigned int *)(*node + 4) = node[1];
    first = (unsigned int *)*head;
    if (first != head && (param_1[6] & 2) != 0) {
      int sum = 0;
      unsigned int k;
      for (k = 0; k < nbytes; k++) {
        sum = sum + *(unsigned char *)(k + 8 + (int)node);
      }
      if (sum == 0) {
        FUN_004522d0((int)node);
        return;
      }
      *node = (unsigned int)first;
      node[1] = (unsigned int)head;
      *(unsigned int **)(*head + 4) = node;
      *head = (unsigned int)node;
      return;
    }
    *node = (unsigned int)first;
    node[1] = (unsigned int)head;
    *(unsigned int **)(*head + 4) = node;
    *head = (unsigned int)node;
  }
}
