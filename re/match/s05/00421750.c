// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00421750 (165 B): 100.0% (regs 100, ops 100), by hand. Same file text as the file of 0x00421800 (164 B),
   also 100.0%. Both read frame record 0x0057c210(id), refresh it through 0x0055ad30 unless its
   bit is set in the owner's bit array (+0x60, index +0x20; the compiler saves EBX/EBP inside that test), then write
   the vector into the node's element (0x00421750) or call 0x0055b650 with the node's matrix pos as default
   (0x00421800). The node chain needs real struct types (A+0x10 -> B; B+0 matrices of 0x40, B+8 elements of 0x20);
   char-pointer arithmetic gave 76.7% / 85.2% (register order of the chain loads).
   Residual: none. Relocheck OK. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct V3_00421750 { float x, y, z; } V3_00421750;
typedef struct W_00421750 { char pad[0x60]; unsigned int *bits; } W_00421750;
typedef struct Mat_00421750 { float m[12]; V3_00421750 pos; float pad; } Mat_00421750;
typedef struct Elem_00421750 { char pad[0x10]; V3_00421750 v; float pad1c; } Elem_00421750;
typedef struct B_00421750 { Mat_00421750 *mats; int b4; Elem_00421750 *elems; } B_00421750;
typedef struct A_00421750 { char pad[0x10]; B_00421750 *b; } A_00421750;
typedef struct Node_00421750 { A_00421750 *a; int idx; } Node_00421750;
typedef struct H_00421750 { int h0; Node_00421750 *node; } H_00421750;
typedef struct F_00421750 {
  char pad0[0x10];
  H_00421750 *h;          /* +0x10 */
  char pad14[0x0c];
  unsigned short idx;     /* +0x20 */
  unsigned short pad22;
  W_00421750 *w;          /* +0x24 */
} F_00421750;
F_00421750 *FUN_0057c210(int id);
void FUN_0055ad30(W_00421750 *w, F_00421750 *f);
void FUN_0055ac00(W_00421750 *w, F_00421750 *f, int n);
void FUN_0055b650(Node_00421750 *n, V3_00421750 *v, void *m);
// MATCH: 0x00421750 FUN_00421750
void FUN_00421750(int id, V3_00421750 *v)
{
  V3_00421750 zero;
  F_00421750 *f;
  W_00421750 *w;
  Node_00421750 *n;
  zero.x = 0.0f;
  zero.y = 0.0f;
  zero.z = 0.0f;
  if (v == 0)
    v = &zero;
  f = FUN_0057c210(id);
  w = f->w;
  if (w == 0 || !(w->bits[f->idx >> 5] & (1 << (f->idx & 0x1f))))
    FUN_0055ad30(w, f);
  n = f->h->node;
  n->a->b->elems[n->idx].v = *v;
  FUN_0055ac00(f->w, f, 1);
}
// MATCH: 0x00421800 FUN_00421800
void FUN_00421800(int id, V3_00421750 *v, void *m)
{
  V3_00421750 zero;
  F_00421750 *f;
  W_00421750 *w;
  Node_00421750 *n;
  zero.x = 0.0f;
  zero.y = 0.0f;
  zero.z = 0.0f;
  if (v == 0)
    v = &zero;
  f = FUN_0057c210(id);
  w = f->w;
  if (w == 0 || !(w->bits[f->idx >> 5] & (1 << (f->idx & 0x1f))))
    FUN_0055ad30(w, f);
  n = f->h->node;
  if (m == 0)
    m = &n->a->b->mats[n->idx].pos;
  FUN_0055b650(n, v, m);
  FUN_0055ac00(f->w, f, 1);
}
