// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RT2D-RESID run 40 (2026-11-12): still 99.3%, source text unchanged, one store out of place. Correction to
   the run 39 note below: the original's `mov [esi+0x10],eax` is at 0x005542a2 (0x005542a5 is the next instruction).
   What c2 9044 does in this block (0x0055427d..0x005542b4), measured on this file:
   (a) it treats hdr as aliased by every store through obj: with hdr.count / hdr.flags read in place instead of
       through the locals, each read is reloaded after the store before it (78.2%). No variant moved a hdr load
       across a store. So in the original's source the hdr.extra load (0x00554295) sits after the count and flagB
       stores and before the parent and flagA stores (0x0055429c, 0x0055429f), and the extra store (0x005542a2) after.
   (b) `obj->extra = hdr.extra` written directly always comes out as load, `lea ecx,[ecx+ecx*2]`, store, in that
       order with nothing else between (all 120 statement orders).
   (c) through a single-use temporary (`extra = hdr.extra; ...; obj->extra = extra;`) c2 moves the store up to the
       load, past the parent and flagA stores, and the lea follows in eax (93.6%). Dead second uses of the
       temporary are removed first (10 forms). A second use that stays (`hdr.extra = extra`) keeps the original's
       load, parent, flagA, extra order, at the price of one instruction (82.4%).
   (d) a store written through obj->parent right after `obj->parent = obj` is forwarded to [esi+0x10] but stays
       behind the parent store: `extra = hdr.extra; obj->parent = obj; obj->flagA = flags & 1;
       obj->parent->extra = extra;` gives the original's order of all six memory operations (94.3%, regs 99.3%,
       ops 99.3%). Left over in that form: the lea is `lea eax,[ecx+ecx*2]` after the extra store instead of
       `lea ecx,[ecx+ecx*2]` at 0x00554299, and eax / ecx / edx are renamed from there to the call at 0x005542b4.
   [UNCERTAIN: which source form makes c2 compute count * 3 between the hdr.extra load and the parent store while
   the extra store stays last. Evidence missing: a variant that does it. Form (d) is the closest.] Tried in run 40
   on top of runs 38 and 39, none above 99.3%: 6,000 generated bodies (store order x temporary and its position x
   one store through obj->parent x size from count / obj->count / hdr.count / obj->parent->count), stores through
   `result` (320), float fields x 120 orders, a float temporary (fld / fstp, 92.9%), `/Oa` and `/Ow` x 120 orders
   (best 97.9%), __inline / __forceinline setters (40), comma expressions (9, side effects come first, 98.6%),
   4-byte memcpy and struct copy (24), a size temporary at 5 positions in form (d), `size = obj->count * 0x18`
   pinned before the stores (30, the whole multiply moves up and extra goes to edi, 75.9%). */
/* MATCH-RT2D-RESID run 39 (2026-11-12): 0x00554200 69.5% -> 99.3% under rw9044 (regs 99.3%, ops 100%, 141/141 ins);
   0x00553e80 stays 100% under rw9044 and the DDK. Three source changes, each read off the original listing:
   (1) a second variable `ret` joined at the end of the else: header failure `ret = 0` (the original's
   `xor ebx,ebx; jmp 0x00554335` at 0x00554265 skips the result test), else `ret = result` (`mov ebx,ebp` at
   0x0055432f), tail `if (ret == 0) Destroy(obj); return ret;`. This keeps result in ebp, so c2 no longer threads
   every failure to the return (69.5 -> 88.1). (2) The 0x1a1 chunk test with the success call in the then arm and
   the failure in the else arm, and the item-array read failure as `goto done` (label before `ret = result`):
   the chunk failure then tail-merges into the malloc failure block (`je 0x005542c3` at 0x00554316) (-> 98.6).
   (3) `obj->buf = buf` written in both arms of the flags & 4 test: c2 hoists the common store above the
   branch after the condition load (`mov al,[esp+0x18]` at 0x005542f0 before `mov [esi],edi`) (-> 99.3).
   Residual: one store. The original stores obj->extra after parent and flagA (`mov [esi+0x10],eax` at
   0x005542a7), ours right after the lea. Not reached by: all 120 orders of the five field stores (twice, before
   and after (3)), extra through a temp at 20 x 5 positions, a size temp at 210 placements, float / unsigned
   field types, hdr as an array, `/Oa` `/Ow` `/G6`, match_permute.py from 88.1 / 98.6 / 99.3 (842 + 1,323 +
   1,325 compiles). */
/* MATCH-S24 0x00554200 (h4): 69.5% (regs 75.9%, ops 86.5%). Stream read of a node: freelist alloc + init
   (Create, inlined), 16-byte header, case type 1: 0x18-byte item array, flag bit 2 reads a 0x1a1 child chunk
   recursively, parent links; on failure the node is destroyed. The file also holds 0x00553e80 (Destroy, 100%):
   the original inlines one level of it here, so it is defined __forceinline before its caller (plain __inline
   is not inlined under /Ob1). Moved the score: Destroy and Create inlined (44.7 -> 56.7), header fields read
   into locals (62.4), switch for the type test (dec eax / jne, 66.0), store order from the permuter (69.5).
   Residual (b): the original keeps the result in its own register (ebp) and joins every path at one
   `ebx = result; if (result == 0) Destroy` block; ours threads the obj/NULL paths straight to the return or to
   Destroy, which moves the zero register (ebp vs edi) everywhere. Tried: result set inside the else, an early
   Destroy return on header failure (50.0), inverted final test (66.7), result = obj = Create(), the type-1 body
   as a __forceinline helper returning obj or NULL (68.8), permuter 422 compiles. */
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
typedef struct Obj554200 {
  void *buf;
  int count;
  int flagA;
  int flagB;
  int extra;
  struct Obj554200 *child;
  struct Obj554200 *parent;
} Obj554200;
typedef struct { int type; int count; unsigned int flags; int extra; } Hdr554200;
typedef void *(*FreeListAllocFn)(int list, unsigned int hint);
typedef void (*FreeListFreeFn)(int list, void *p);
typedef void *(*MallocFn)(unsigned int size, unsigned int hint);
typedef void (*FreeFn)(void *p);
MGLOBAL(int, DAT_00912a24, 0x00912a24);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
unsigned int FUN_004cbd30(void *stream, void *buf, unsigned int len);
int FUN_004cc5e0(void *stream, int type, unsigned int *size, unsigned int *version);
Obj554200 *FUN_00553f40(Obj554200 *obj);
void FUN_00554150(Obj554200 *obj);
void FUN_005c4d50(void *p);

// MATCH: 0x00553e80 FUN_00553e80
__forceinline int FUN_00553e80(Obj554200 *obj)
{
  int ok = (obj != 0);
  if (ok) {
    Obj554200 *child = obj->child;
    if (obj->buf != 0) {
      if (obj->count == -1) {
        FUN_005c4d50(obj->buf);
      } else {
        (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(obj->buf);
      }
    }
    obj->buf = 0;
    (*(FreeListFreeFn *)(DAT_007d3ff8 + 0x11c))(DAT_00912a24, obj);
    FUN_00553e80(child);
  }
  return ok;
}

static __forceinline Obj554200 *Create554200(void)
{
  Obj554200 *obj = (Obj554200 *)(*(FreeListAllocFn *)(DAT_007d3ff8 + 0x118))(DAT_00912a24, 0x301a1);
  if (obj != 0) {
    obj->buf = 0;
    obj->count = 0;
    obj->flagA = 0;
    obj->flagB = 1;
    obj->child = 0;
    obj->parent = obj;
    obj->extra = 0;
    FUN_00553f40(obj);
  }
  return obj;
}

// MATCH: 0x00554200 FUN_00554200
Obj554200 *FUN_00554200(void *param_1)
{
  Obj554200 *obj;
  Obj554200 *result;
  Obj554200 *ret;
  Obj554200 *c;
  void *buf;
  Hdr554200 hdr;
  int count;
  unsigned int flags;

  obj = Create554200();
  result = obj;
  if (FUN_004cbd30(param_1, &hdr, 0x10) != 0x10) {
    ret = 0;
  } else {
    switch (hdr.type) {
    case 1:
      FUN_00554150(obj);
      count = hdr.count;
      flags = hdr.flags;
      obj->flagB = (flags >> 1) & 1;
      obj->count = count;
      obj->extra = hdr.extra;
      obj->parent = obj;
      obj->flagA = flags & 1;
      buf = (*(MallocFn *)(DAT_007d3ff8 + 0x108))(count * 0x18, 0x301a1);
      if (buf == 0) {
        result = 0;
        break;
      }
      if (obj->count != 0 && FUN_004cbd30(param_1, buf, obj->count * 0x18) != (unsigned int)(obj->count * 0x18)) {
        result = 0;
        goto done;
      }
      if (hdr.flags & 4) {
        obj->buf = buf;
        if (FUN_004cc5e0(param_1, 0x1a1, 0, 0)) {
          obj->child = FUN_00554200(param_1);
        } else {
          result = 0;
          break;
        }
      } else {
        obj->buf = buf;
      }
      for (c = obj->child; c != 0; c = c->child) {
        c->parent = obj;
      }
      break;
    }
done:
    ret = result;
  }
  if (ret == 0) {
    FUN_00553e80(obj);
  }
  return ret;
}
