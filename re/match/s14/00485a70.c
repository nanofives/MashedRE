// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 h3 0x00485a70: 100.0% (regs 100%). Hand-written with Pool/PoolNode structs (raw g2s 91.3%).
   Key: the free-slot scan is 'while (i-- != 0) if (!used[i]) break;' (the post-decrement gives 0xffff when none
   is free, matching the 'add edx,0xffff' fall-through). relocheck OK. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef struct PoolNode {
  undefined4 data;
  struct PoolNode *prev;
  struct PoolNode *next;
  ushort idx;
  ushort pad;
} PoolNode;

typedef struct Pool {
  ushort count;
  ushort max;
  PoolNode *head;
  PoolNode *tail;
  PoolNode *nodes;
  int *used;
} Pool;

// MATCH: 0x00485a70 FUN_00485a70
ushort FUN_00485a70(Pool *pool,undefined4 data)
{
  ushort i;
  PoolNode *n;

  if (pool->count >= pool->max) {
    return 0xffff;
  }
  i = pool->max;
  while (i-- != 0) {
    if (pool->used[i] == 0) break;
  }
  n = &pool->nodes[i];
  pool->used[i] = 1;
  n->idx = i;
  pool->count++;
  n->data = data;
  if (pool->head == 0) {
    pool->head = n;
    n->prev = 0;
    pool->head->next = 0;
    return n->idx;
  }
  if (pool->tail == 0) {
    pool->tail = n;
    n->next = 0;
    pool->tail->prev = pool->head;
    pool->head->next = pool->tail;
    return n->idx;
  }
  n->prev = pool->tail;
  pool->tail->next = n;
  pool->tail = n;
  n->next = 0;
  return n->idx;
}
