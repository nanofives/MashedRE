// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004c9ad0 (h4): 100% (regs 100%), relocheck OK. D3D9 device-state reset (raw converter did not compile:
   C2064 on the code-typed calls). Sets the five dwords at 0x006181c8..d8 and the 4 x 16-byte stage cache at
   0x007d40c0 to -1/0/0, frees the 0x104 cached blocks at 0x007d4158 back to the free list 0x007d4568 (or creates the
   list FUN_004cc7f0(0x40, 0xf, 0x10, 0x30411) and zeroes the array), allocates a 64-byte block and copies the
   defaults at 0x005d8b90 into it, then IDirect3DDevice9 calls: SetTexture(0..7, NULL) (+0x104), +0x1a0(NULL),
   SetStreamSource(0..3, NULL, 0, 0) (+0x190), +0x1ac/+0x15c/+0x170(NULL), render target 0 and depth surface
   resync through the caches 0x007d4578[4] / 0x007d4574, clears render targets 1..3, five resource callbacks and the
   optional hook at 0x007d4598. Returns 1. What moved it: unsigned loop counters, memset intrinsic for the array,
   struct assignment for the 64-byte copy, vtable calls through typed function pointers. */
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
typedef struct ComObj { void **lpVtbl; } ComObj;
typedef long (__stdcall *Fn1)(ComObj *self, void *a);
typedef long (__stdcall *Fn2)(ComObj *self, unsigned int a, void *b);
typedef long (__stdcall *Fn4)(ComObj *self, unsigned int a, void *b, unsigned int c, unsigned int d);
#define VCALL(T, o, i) ((T)(o)->lpVtbl[(i)])
typedef struct StageCache { int texture; int a; int b; int c; } StageCache;
typedef struct Block64 { int v[16]; } Block64;
typedef void (*FreeToListFn)(void *list, void *block);
typedef void *(*AllocFromListFn)(void *list, int hint);
typedef void (*CallbackFn)(void);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_006181c8, 0x006181c8);
MGLOBAL(int, DAT_006181cc, 0x006181cc);
MGLOBAL(int, DAT_006181d0, 0x006181d0);
MGLOBAL(int, DAT_006181d4, 0x006181d4);
MGLOBAL(int, DAT_006181d8, 0x006181d8);
MGLOBALA(StageCache, DAT_007d40c0, 4, 0x007d40c0);
MGLOBAL(int, DAT_007d4154, 0x007d4154);
MGLOBALA(void *, DAT_007d4158, 0x104, 0x007d4158);
MGLOBAL(void *, DAT_007d4568, 0x007d4568);
MGLOBAL(Block64 *, DAT_007d4558, 0x007d4558);
MGLOBAL(Block64, DAT_005d8b90, 0x005d8b90);
MGLOBAL(ComObj *, DAT_007d4110, 0x007d4110);
MGLOBAL(void *, DAT_007d4114, 0x007d4114);
MGLOBAL(void *, DAT_007d4118, 0x007d4118);
MGLOBAL(void *, DAT_007d4574, 0x007d4574);
MGLOBALA(void *, DAT_007d4578, 4, 0x007d4578);
MGLOBAL(CallbackFn, DAT_007d4598, 0x007d4598);
void *__cdecl memset(void *, int, unsigned int);
#pragma intrinsic(memset)
void *FUN_004cc7f0(int entrySize, int entriesPerBlock, int alignment, int hint);
int FUN_004ccc50(void *list);
void FUN_004db550(void);
void FUN_004e08b0(void);
void FUN_004dc8e0(void);
void FUN_004d1d70(void);
void FUN_004ccde0(void);

// MATCH: 0x004c9ad0 FUN_004c9ad0
int FUN_004c9ad0(void)
{
  unsigned int i;
  DAT_006181c8 = -1;
  DAT_006181cc = -1;
  DAT_006181d0 = -1;
  DAT_006181d4 = -1;
  DAT_006181d8 = -1;
  for (i = 0; i < 4; i++) {
    DAT_007d40c0[i].texture = -1;
    DAT_007d40c0[i].a = 0;
    DAT_007d40c0[i].b = 0;
  }
  DAT_007d4154 = 0;
  if (DAT_007d4568) {
    for (i = 0; i < 0x104; i++) {
      if (DAT_007d4158[i]) {
        (*(FreeToListFn *)(DAT_007d3ff8 + 0x11c))(DAT_007d4568, DAT_007d4158[i]);
        DAT_007d4158[i] = 0;
      }
    }
    FUN_004ccc50(DAT_007d4568);
  } else {
    DAT_007d4568 = FUN_004cc7f0(0x40, 0xf, 0x10, 0x30411);
    memset(DAT_007d4158, 0, sizeof(DAT_007d4158));
  }
  DAT_007d4558 = (Block64 *)(*(AllocFromListFn *)(DAT_007d3ff8 + 0x118))(DAT_007d4568, 0x30411);
  *DAT_007d4558 = DAT_005d8b90;
  for (i = 0; i < 8; i++) {
    VCALL(Fn2, DAT_007d4110, 65)(DAT_007d4110, i, 0);
  }
  VCALL(Fn1, DAT_007d4110, 104)(DAT_007d4110, 0);
  for (i = 0; i < 4; i++) {
    VCALL(Fn4, DAT_007d4110, 100)(DAT_007d4110, i, 0, 0, 0);
  }
  VCALL(Fn1, DAT_007d4110, 107)(DAT_007d4110, 0);
  VCALL(Fn1, DAT_007d4110, 87)(DAT_007d4110, 0);
  VCALL(Fn1, DAT_007d4110, 92)(DAT_007d4110, 0);
  if (DAT_007d4578[0] != DAT_007d4118) {
    DAT_007d4578[0] = DAT_007d4118;
    VCALL(Fn2, DAT_007d4110, 37)(DAT_007d4110, 0, DAT_007d4118);
  }
  for (i = 1; i < 4; i++) {
    if (DAT_007d4578[i]) {
      DAT_007d4578[i] = 0;
      VCALL(Fn2, DAT_007d4110, 37)(DAT_007d4110, i, 0);
    }
  }
  if (DAT_007d4574 != DAT_007d4114) {
    DAT_007d4574 = DAT_007d4114;
    VCALL(Fn1, DAT_007d4110, 39)(DAT_007d4110, DAT_007d4114);
  }
  FUN_004db550();
  FUN_004e08b0();
  FUN_004dc8e0();
  FUN_004d1d70();
  FUN_004ccde0();
  if (DAT_007d4598) {
    DAT_007d4598();
  }
  return 1;
}
