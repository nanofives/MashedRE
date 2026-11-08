// FLAGS: /O2 /Ob1 /G5
// h2 hand-matched.
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

typedef struct A { int pad[4]; struct A *next; struct B *b; int p18; uint bit; } A;
typedef struct B { int pad[2]; uint mask; } B;
typedef struct R { A *a; struct R *right; struct R *down; } R;
typedef struct { int p[3]; uint n; } Ctx;
// MATCH: 0x004e2b60 FUN_004e2b60
undefined4 FUN_004e2b60(Ctx *param_1, R **param_2)
{
  uint uVar7 = 0;
  R *row = *param_2;
  if (row != 0) {
    do {
      uint mask = 0;
      uint bit;
      R *c = row;
      A *a;
      do {
        for (a = c->a; a != 0; a = a->next) {
          mask |= a->b->mask;
        }
        c = c->right;
      } while (c != 0);
      bit = 0;
      for (; (mask & 1) != 0; mask >>= 1) {
        bit++;
      }
      if (bit >= uVar7) {
        uVar7 = bit + 1;
      }
      c = row;
      do {
        a = c->a;
        if (a != 0) {
          do {
            a->bit = bit;
            a->b->mask |= 1 << bit;
            a = a->next;
          } while (a != 0);
        }
        c = c->right;
      } while (c != 0);
      row = row->down;
    } while (row != 0);
  }
  param_1->n = uVar7;
  return 0;
}
