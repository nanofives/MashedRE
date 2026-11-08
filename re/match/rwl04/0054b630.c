// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x0054b630: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
typedef void *(*PFN_alloc)(int,int);
typedef void (*PFN_free)(void *);
void *__cdecl memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
typedef struct { int n; int cap; short *data; } VB;
// MATCH: 0x0054b630 FUN_0054b630
int FUN_0054b630(int param_1,int param_2,undefined2 param_3)
{
  VB *b = (VB *)param_1 + param_2;
  int cap = b->cap;
  short *old;
  short *nw;
  if (b->n == cap) {
    old = b->data;
    nw = (short *)(**(PFN_alloc *)(DAT_007d3ff8 + 0x108))((cap + 8) * 2,0x30123);
    if (old != 0) {
      memcpy(nw,old,b->n * 2);
      (**(PFN_free *)(DAT_007d3ff8 + 0x10c))(old);
    }
    b->cap = cap + 8;
    b->data = nw;
  }
  b->data[b->n] = param_3;
  b->n = b->n + 1;
  return param_1;
}
