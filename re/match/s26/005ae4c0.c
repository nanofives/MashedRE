// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005ae4c0: 35.5% (regs 79.0%, ops 85.5%), 62 vs 61 insns. Rewritten with Blk/Pool structs (free-list allocator: header next/prev/size, user ptr = header+0xc, pool +8 cursor, +0xc sentinel). Same operations, but the register assignment/scheduling differs from the first lea (original keeps need in ebx, cur in edx, next in esi, size in ecx), so the in-place score stays low. Tried free-space expression order, prev-before-next link order, locals for size/base. Class (b). cxx pending. */
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
typedef unsigned int uint;
typedef struct Blk { struct Blk *next; struct Blk *prev; uint size; } Blk;
typedef struct Pool { int w0; int w1; Blk *cur; Blk *end; } Pool;
// MATCH: 0x005ae4c0 FUN_005ae4c0
uint FUN_005ae4c0(Pool *param_1,int param_2,uint param_3)
{
  Blk *cur;
  Blk *next;
  Blk *nb;
  uint base;
  uint need;
  uint size;

  need = param_2 + 0xcU;
  cur = param_1->cur;
  do {
    next = cur->next;
    size = cur->size;
    if (need <= (uint)next - size - (uint)cur - 0xc) {
      base = (uint)cur + size;
      nb = (Blk *)(((base + param_3 + 0x17) & ~(param_3 - 1)) - 0xc);
      if (((uint)nb + need <= (uint)next) && (base + 0xc <= (uint)nb)) {
        if (nb == 0) {
          return 0;
        }
        nb->prev = cur;
        nb->next = cur->next;
        if (nb->next != param_1->end) {
          nb->next->prev = nb;
        }
        if (nb->prev != param_1->end) {
          nb->prev->next = nb;
        }
        nb->size = param_2;
        return (uint)nb + 0xc;
      }
    }
    cur = next;
    if (cur == param_1->end) {
      return 0;
    }
  } while (1);
}
