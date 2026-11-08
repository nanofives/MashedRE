// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR05 2026-11-08: 78.3% regs 80.7% ops 97.5% (was 69.1). Re-derived as clean C over typed structs (Island {rank, count, first, last, nA, headA, tailA, nB, headB, tailB}, World {f0, n, islands[]}): union by rank as if/else, small->nB > 0 (jbe), struct copy for the compaction. Residual: the original pushes edi at entry (ours shrink-wraps it into the merge path), which shifts every later param offset by 4; else-return, inner returns and hoisted big/small assignments did not move it. Class b. Supersedes the header below. */
/* MATCH-RWL06 0x005685f0: hand pass (h2), body kept in raw form. 69.1% regs 74.7% ops 90.1% (Ghidra output already compiles; unchanged shape). The original hoists mov esi,1 above the first null test and keeps constants/regs differently (push edi at entry); permuter 211 compiles gave nothing. Class b. */
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
// MATCH: 0x005685f0 FUN_005685f0
typedef struct Link { struct Link *next; } Link;
typedef struct Node { struct Node *f0; void *owner; struct Node *next; } Node;
typedef struct Island { uint rank; uint count; Node *first; Node *last; uint nA; Link *headA; Link *tailA; uint nB; Link *headB; Link *tailB; } Island;
typedef struct World { int f0; int n; Island islands[1]; } World;
void FUN_005685f0(World *w,Island *a,Island *b,Link *la,Link *lb)
{
  Island *big;
  Island *small;
  int n;

  if (a == b) {
    if (la != 0) {
      la->next = 0;
      if (a->nA == 0) {
        a->tailA = la;
        a->headA = la;
        a->nA = 1;
      }
      else {
        a->tailA->next = la;
        a->tailA = la;
        a->nA = a->nA + 1;
      }
    }
    if (lb != 0) {
      lb->next = 0;
      if (a->nB == 0) {
        a->tailB = lb;
        a->headB = lb;
        a->nB = 1;
      }
      else {
        a->tailB->next = lb;
        a->tailB = lb;
        a->nB = a->nB + 1;
      }
    }
    
  }
  else {
    if (a->rank > b->rank) {
      big = a;
      small = b;
    }
    else {
      big = b;
      small = a;
      if (b->rank == a->rank) {
        b->rank = b->rank + 1;
      }
    }
    small->first->f0 = big->first;
    small->first->owner = 0;
    big->last->next = small->first;
    big->last = small->last;
    big->count = big->count + small->count;
    if (small->nA != 0) {
      if (big->nA == 0) {
        big->headA = small->headA;
        big->tailA = small->tailA;
        big->nA = small->nA;
      }
      else {
        big->tailA->next = small->headA;
        big->tailA = small->tailA;
        big->nA = big->nA + small->nA;
      }
    }
    if (small->nB > 0) {
      if (big->nB == 0) {
        big->headB = small->headB;
        big->tailB = small->tailB;
        big->nB = small->nB;
      }
      else {
        big->tailB->next = small->headB;
        big->tailB = small->tailB;
        big->nB = big->nB + small->nB;
      }
    }
    if (la != 0) {
      la->next = 0;
      if (big->nA == 0) {
        big->tailA = la;
        big->headA = la;
        big->nA = 1;
      }
      else {
        big->tailA->next = la;
        big->tailA = la;
        big->nA = big->nA + 1;
      }
    }
    if (lb != 0) {
      lb->next = 0;
      if (big->nB == 0) {
        big->tailB = lb;
        big->headB = lb;
        big->nB = 1;
      }
      else {
        big->tailB->next = lb;
        big->tailB = lb;
        big->nB = big->nB + 1;
      }
    }
    n = --w->n;
    if (n != 0) {
      *small = w->islands[n];
      small->first->owner = small;
    }
  }
}
