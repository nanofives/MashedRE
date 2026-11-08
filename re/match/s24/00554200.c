// FLAGS: /O2 /Ob1 /G5
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
  Obj554200 *c;
  void *buf;
  Hdr554200 hdr;
  int count;
  unsigned int flags;

  obj = Create554200();
  result = obj;
  if (FUN_004cbd30(param_1, &hdr, 0x10) != 0x10) {
    result = 0;
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
        break;
      }
      obj->buf = buf;
      if (hdr.flags & 4) {
        if (!FUN_004cc5e0(param_1, 0x1a1, 0, 0)) {
          result = 0;
          break;
        }
        obj->child = FUN_00554200(param_1);
      }
      for (c = obj->child; c != 0; c = c->child) {
        c->parent = obj;
      }
      break;
    }
  }
  if (result != 0) {
    return result;
  }
  FUN_00553e80(obj);
  return 0;
}
