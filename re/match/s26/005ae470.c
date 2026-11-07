// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005ae470: 100% by hand: header struct (q at +8, end at +0xc) with a Q record, function returns the allocation. cxx pending. */
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
typedef struct Q { int *x; int y; int z; } Q;
typedef struct Hdr { int w0; int w1; Q *q; int *end; int w4; int w5; } Hdr;
Hdr * FUN_005aea00(int p0, int p1);
// MATCH: 0x005ae470 FUN_005ae470
Hdr * FUN_005ae470(int param_1)
{
  Hdr *h;

  h = FUN_005aea00(param_1,0x30800);
  if (h != 0) {
    h->q = (Q *)((char *)h + 0x10);
    h->end = (int *)((char *)h + param_1);
    h->q->z = 0;
    h->q->y = 0;
    h->q->x = h->end;
  }
  return h;
}
