// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 h3 0x00485b30: 46.9% (regs 70.3%, ops 87.5%). Raw g2s 4.5%. Hand-written with Pool/PoolNode
   structs (same layout as 0x00485a70). result=0 hoisted + single exit, cached tail local and nx temp gave the
   original's eax/ecx/edx roles. Residual (b): the original keeps a zero register in ebx (3 saved regs, separate
   early 'pop ebx; ret' exit) and the data param in edi, head->data loaded into esi; ours uses edi for zero and 2
   saved regs. Tried for/while/goto loop forms, early return vs nested if, permuter 400 s (no gain). */
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
#ifndef __cplusplus
#define true 1
#endif
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

// MATCH: 0x00485b30 FUN_00485b30
int FUN_00485b30(Pool *pool,undefined4 data)
{
  int result;
  PoolNode *n;
  PoolNode *tail;
  PoolNode *nx;

  result = 0;
  n = pool->head;
  if (n != 0) {
    if (n->data == data) {
      if (n->next != 0) {
        pool->head = n->next;
        pool->head->prev = 0;
      }
      else {
        pool->head = 0;
        pool->tail = 0;
      }
    }
    else {
      tail = pool->tail;
      if (tail->data == data) {
        nx = tail->prev;
        pool->tail = nx;
        n = tail;
        nx->next = 0;
      }
      else {
        n = n->next;
        while (1) {
          if (n == 0) return result;
          if (n == tail) return result;
          if (n->data == data) break;
          n = n->next;
        }
        n->prev->next = n->next;
        n->next->prev = n->prev;
      }
    }
    pool->count--;
    n->data = 0;
    n->next = 0;
    n->prev = 0;
    result = 1;
    pool->used[n->idx] = 0;
  }
  return result;
}
