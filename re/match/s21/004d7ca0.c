// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d7ca0 (h2): 100% (regs 100%), relocheck OK. Teardown: when DAT_007d6c50 is set, walks it with
   FUN_004ccce0(list, FUN_004d7d70, list); unless RwGlobals+0x118 is FUN_004cca80, frees every block chain
   (+0x30 next) of the DAT_007d6c58 entries in DAT_007d6c54 through RwGlobals+0x11c(0, blk), resets the pool at
   blk+0x38 (+0x10 used -> 0, +0 = +4, +0x14 -> 0), frees the entry array through +0x10c; then
   FUN_004cc9f0(list) and clears DAT_007d6c50; returns 1.
   What moved it: FUN_004cca80 / FUN_004d7d70 as functions (the raw form declared them as byte globals) and an
   unsigned `for` loop (jbe guard); raw 78.6%. Struct field names are layout-only. */
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
typedef void (*RwFreeFn)(void *, void *);
typedef void (*RwFree1Fn)(void *);
typedef struct Pool { int cur; int base; int pad[2]; int used; int extra; } Pool;
typedef struct Blk { char pad[0x30]; struct Blk *next; int pad2; Pool *pool; } Blk;
typedef struct Ent { char pad[0x10]; Blk *blocks; } Ent;
MGLOBAL(void *, DAT_007d6c50, 0x007d6c50);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(unsigned int, DAT_007d6c58, 0x007d6c58);
MGLOBAL(Ent **, DAT_007d6c54, 0x007d6c54);
void *FUN_004cca80(void *a, void *b);
void *FUN_004d7d70(void *a, void *b);
int FUN_004cc9f0(void *list);
void *FUN_004ccce0(void *list, void *(*cb)(void *, void *), void *data);
// MATCH: 0x004d7ca0 FUN_004d7ca0
int FUN_004d7ca0(void)
{
  unsigned int i;
  if (DAT_007d6c50) {
    FUN_004ccce0(DAT_007d6c50, FUN_004d7d70, DAT_007d6c50);
    if (*(void **)(DAT_007d3ff8 + 0x118) != (void *)FUN_004cca80) {
      for (i = 0; i < DAT_007d6c58; i++) {
        Blk *b = DAT_007d6c54[i]->blocks;
        if (b) {
          Pool *pool = b->pool;
          do {
            Blk *next = b->next;
            (*(RwFreeFn *)(DAT_007d3ff8 + 0x11c))(0, b);
            b = next;
          } while (b);
          if (pool && pool->used) {
            pool->used = 0;
            pool->cur = pool->base;
            pool->extra = 0;
          }
        }
      }
      if (DAT_007d6c54) {
        (*(RwFree1Fn *)(DAT_007d3ff8 + 0x10c))(DAT_007d6c54);
        DAT_007d6c54 = 0;
      }
    }
    FUN_004cc9f0(DAT_007d6c50);
    DAT_007d6c50 = 0;
  }
  return 1;
}
