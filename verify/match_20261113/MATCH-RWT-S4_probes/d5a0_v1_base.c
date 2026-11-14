// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0053d5a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef unsigned int uint;
typedef unsigned short ushort;
#ifdef __cplusplus
typedef int (*codeptr)(...);
#else
typedef int (*codeptr)();
#endif
typedef struct S24 { uint d[6]; } S24;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_004d7ff0(undefined4 p0, int p1);
undefined4 * FUN_004d8480(undefined4 * p0);
#define NULL_P ((void *)0)
typedef struct H {
  uint flags; S24 s; ushort n1; ushort n2; void *a16; void *a2;
} H;
// MATCH: 0x0053d5a0 FUN_0053d5a0
H *FUN_0053d5a0(ushort param_1, ushort param_2, uint *param_3, uint param_4)
{
  H *h;
  uchar *mem;
  int size;
  undefined4 loc[2];

  size = 0x28;
  if (param_2 > 0) size = (uint)param_2 * 0x10 + 0x37;
  if ((param_4 & 1) != 0) size = size + 1 + (uint)param_1 * 2;
  h = (H *)(**(codeptr *)(DAT_007d3ff8 + 0x108))(size, 0x3002c);
  if (h == 0) {
    loc[0] = 0x11d;
    loc[1] = FUN_004d7ff0(0x80000013, size);
    FUN_004d8480(loc);
    return 0;
  }
  h->n1 = param_1;
  h->flags = param_4;
  h->n2 = param_2;
  h->s = *(S24 *)param_3;
  mem = (uchar *)(h + 1);
  if (param_2 > 0) {
    mem = (uchar *)(((uint)mem + 0xf) & ~0xf);
    h->a16 = mem;
    mem += param_2 * 16;
  } else {
    h->a16 = 0;
  }
  if (param_4 & 1) {
    h->a2 = (void *)(((uint)mem + 1) & ~1);
  } else {
    h->a2 = 0;
  }
  return h;
}