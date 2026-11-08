// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f3030 (h3, by hand): 100% (regs 100%) under /O2 /Ob1 /G5. Raw converter 61.5%.
   Triangle-strip bin update: struct tri {edge[3] @+4, next @+0x10, prev @+0x14, used @+0x18, @+0x1c, u8 bin @+0x20},
   edge {tri[2] @+4}. What moved it: the bin unlink as a macro that reads `bins[k] = bins[k]->next` (57 -> 90%),
   the neighbour pick as one nested ?: expression (keeps the redundant `other != 0` test), and the tri param copied
   to a local (the 3-count loop counter reuses param 1's stack slot, regs -> 100%). */
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
typedef struct S22StripTri S22StripTri;
typedef struct S22StripEdge { int unk0; S22StripTri *tri[2]; } S22StripEdge;
struct S22StripTri {
  int unk0;
  S22StripEdge *edge[3];
  S22StripTri *next;
  S22StripTri *prev;
  int used;
  int flag1c;
  unsigned char numFree;
};
#define S22_UNLINK(t, bins) \
  if ((bins)[(t)->numFree] == (t)) { \
    (bins)[(t)->numFree] = (bins)[(t)->numFree]->next; \
    if ((bins)[(t)->numFree]) (bins)[(t)->numFree]->prev = 0; \
  } else { \
    if ((t)->next) (t)->next->prev = (t)->prev; \
    if ((t)->prev) (t)->prev->next = (t)->next; \
  }
// MATCH: 0x004f3030 FUN_004f3030
void FUN_004f3030(S22StripTri *t, S22StripTri **bins, int n)
{
  int i;
  S22StripTri *tri = t;
  S22StripTri *other;

  if (n < 4) {
    tri->flag1c = 1;
    return;
  }
  tri->used = 1;
  S22_UNLINK(tri, bins)
  for (i = 0; i < 3; i++) {
    S22StripEdge *e = tri->edge[i];
    other = (e->tri[0] && e->tri[0] != tri && !e->tri[0]->used) ? e->tri[0] :
            ((e->tri[1] && !e->tri[1]->used) ? e->tri[1] : 0);
    if (other) {
      S22_UNLINK(other, bins)
      other->numFree--;
      other->next = bins[other->numFree];
      if (other->next) other->next->prev = other;
      bins[other->numFree] = other;
      other->prev = 0;
    }
  }
}
