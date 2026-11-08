// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f3150 (h3, by hand): 16.5% exact, regs 55.1%, ops 92.3% under /O2 /Ob1 /G5 (raw converter 16.0%,
   regs 54.6%, ops 85.1%). Strip walk over the 0x004f3030 tri/edge structs: pick a free tri on the edge (flag +0x1c
   when n < 4, else used +0x18), FUN_004f3030 it, find the next vertex and the joining edge, append indices
   (swap index when the strip parity needs it). Control flow and instruction mix are close (272 vs 269 ins);
   register colouring is not: the original keeps edge in esi / tri in edi / strip in ebx and spills `vert` into the
   edge param's stack slot (5 stack locals, ours 4), ours keeps vert in a register.
   Tried: while vs if/do loop, cached indices/num locals (needed: the original reads them from the stack),
   short/int vert types, permuter (701 compiles, 19.6% only with shuffled statement order, not kept).
   Param 4 is unused by the original body. Out of budget: class (b). */
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
typedef struct S22StripEdge { unsigned short v[2]; S22StripTri *tri[2]; } S22StripEdge;
struct S22StripTri {
  int unk0;
  S22StripEdge *edge[3];
  S22StripTri *next;
  S22StripTri *prev;
  int used;
  int flag1c;
  unsigned char numFree;
};
typedef struct { unsigned short *indices; int numIndices; } S22Strip;
void FUN_004f3030(S22StripTri *t, S22StripTri **bins, int n);
// MATCH: 0x004f3150 FUN_004f3150
int FUN_004f3150(S22Strip *strip, S22StripEdge *e, S22StripTri **bins, int unused, int n)
{
  S22StripEdge *edge = e;
  S22StripEdge *next = 0;
  int numTris = 0;
  int idx;
  S22StripTri *tri;
  S22StripEdge *e2;
  S22StripEdge *join;
  unsigned short vert, last;
  int a, b;
  unsigned short *ind;
  int num;

  while (edge) {
    idx = -1;
    if (n < 4)
      tri = (edge->tri[0] && !edge->tri[0]->flag1c) ? edge->tri[0] :
            ((edge->tri[1] && !edge->tri[1]->flag1c) ? edge->tri[1] : 0);
    else
      tri = (edge->tri[0] && !edge->tri[0]->used) ? edge->tri[0] :
            ((edge->tri[1] && !edge->tri[1]->used) ? edge->tri[1] : 0);
    if (!tri) return numTris;
    numTris++;
    FUN_004f3030(tri, bins, n);
    if (edge == tri->edge[0]) idx = 0;
    else if (edge == tri->edge[1]) idx = 1;
    else if (edge == tri->edge[2]) idx = 2;
    e2 = tri->edge[(idx + 1) % 3];
    vert = (e2->tri[0] == tri) ? e2->v[1] : e2->v[0];
    ind = strip->indices;
    num = strip->numIndices;
    last = ind[num - 1];
    join = tri->edge[0];
    if (!((join->v[0] == last && join->v[1] == vert) || (join->v[0] == vert && join->v[1] == last))) {
      join = tri->edge[1];
      if (!((join->v[0] == last && join->v[1] == vert) || (join->v[0] == vert && join->v[1] == last))) {
        join = tri->edge[2];
        if (!((join->v[0] == last && join->v[1] == vert) || (join->v[0] == vert && join->v[1] == last)))
          join = 0;
      }
    }
    if (n < 4) {
      if (join) {
        a = (join->tri[0] && !join->tri[0]->flag1c) ? 1 : 0;
        b = (join->tri[1] && !join->tri[1]->flag1c) ? 1 : 0;
      } else a = b = 0;
    } else {
      if (join) {
        a = (join->tri[0] && !join->tri[0]->used) ? 1 : 0;
        b = (join->tri[1] && !join->tri[1]->used) ? 1 : 0;
      } else a = b = 0;
    }
    if (a + b != 0) {
      ind[num] = vert;
    } else {
      if (tri->edge[0] != edge && tri->edge[0] != join) next = tri->edge[0];
      else if (tri->edge[1] != edge && tri->edge[1] != join) next = tri->edge[1];
      else if (tri->edge[2] != edge && tri->edge[2] != join) next = tri->edge[2];
      join = next;
      if (n < 4) {
        if (join) {
          a = (join->tri[0] && !join->tri[0]->flag1c) ? 1 : 0;
          b = (join->tri[1] && !join->tri[1]->flag1c) ? 1 : 0;
        } else a = b = 0;
      } else {
        if (join) {
          a = (join->tri[0] && !join->tri[0]->used) ? 1 : 0;
          b = (join->tri[1] && !join->tri[1]->used) ? 1 : 0;
        } else a = b = 0;
      }
      if (a + b == 0 || (num & 1)) {
        strip->indices[strip->numIndices] = vert;
        strip->numIndices++;
        return numTris;
      }
      ind[num] = ind[num - 2];
      strip->numIndices++;
      strip->indices[strip->numIndices] = vert;
    }
    strip->numIndices++;
    edge = join;
  }
  return numTris;
}
