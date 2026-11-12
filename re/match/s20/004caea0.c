// FLAGS: /O2 /Ob1 /G5
/* RWC-MATCH-S01 (2026-11-12): 100%, byte-identical under ddk and rw9044, relocheck OK. DAT_007d4578..0x007d4587 are cleared as one 16-byte memset (the original zeroes them from eax), which closes residual (1) below and the rest with it. */
/* MATCH-S20 0x004caea0 (h1): 77.5% (regs 86.2%). Residual: (1) the original zeroes DAT_007d4578/457c/4580/4584 from eax
   (the 16-byte-record loop counter's 0) and only DAT_007d4574 from ebx, ours uses ebx for all five; (2) the
   RwGlobals function pointer is loaded into ecx after the argument pushes (ours into eax between the pushes), three
   call sites. Tried: if/else branch order (61.3% -> 77.5%), RwGlobals as a struct with typed members (no change),
   flag sweep (game flags 68.8%), permuter 165 compiles. Class (b), item (2) as S19's (a)-class fn-pointer
   scheduling notes. Device render-state cache reset: zeroes DAT_007d4578/457c/4580/4574/4584,
   sets DAT_006181c8..d8 to -1, resets the 4 x 16-byte records at DAT_007d40c0 to {-1, 0, 0}, zeroes DAT_007d4154.
   With no pool in DAT_007d4568 creates one (FUN_004cc7f0(0x40, 0xf, 0x10, 0x30411)) and clears the 0x104-dword
   table DAT_007d4158; otherwise frees each nonzero table entry through RwGlobals+0x11c(pool, entry), clears it, and
   calls FUN_004ccc50(pool). Then allocates through RwGlobals+0x118(pool, 0x30411) into DAT_007d4558 and copies the
   64-byte block at DAT_005d8b90 into it; zeroes DAT_007d456c; frees DAT_007d4570 through RwGlobals+0x10c and
   zeroes it. Returns DAT_007d4110. */
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
typedef struct Rec16 { int a; int b; int c; int d; } Rec16;
typedef struct Blk64 { int v[16]; } Blk64;
typedef void (*RwFree2)(int pool, int p);
typedef Blk64 *(*RwAlloc2)(int pool, int flags);
typedef void (*RwFree1)(int p);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4574, 0x007d4574);
MGLOBALA(int, DAT_007d4578, 4, 0x007d4578);   /* 0x007d4578..0x007d4587, cleared as one 16-byte block */
MGLOBAL(int, DAT_006181c8, 0x006181c8);
MGLOBAL(int, DAT_006181cc, 0x006181cc);
MGLOBAL(int, DAT_006181d0, 0x006181d0);
MGLOBAL(int, DAT_006181d4, 0x006181d4);
MGLOBAL(int, DAT_006181d8, 0x006181d8);
MGLOBALA(Rec16, DAT_007d40c0, 4, 0x007d40c0);
MGLOBAL(int, DAT_007d4154, 0x007d4154);
MGLOBAL(int, DAT_007d4568, 0x007d4568);
MGLOBALA(int, DAT_007d4158, 0x104, 0x007d4158);
MGLOBAL(Blk64 *, DAT_007d4558, 0x007d4558);
MGLOBAL(Blk64, DAT_005d8b90, 0x005d8b90);
MGLOBAL(int, DAT_007d456c, 0x007d456c);
MGLOBAL(int, DAT_007d4570, 0x007d4570);
MGLOBAL(int, DAT_007d4110, 0x007d4110);
int FUN_004cc7f0(int a, int b, int c, int d);
void FUN_004ccc50(int pool);
void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)

// MATCH: 0x004caea0 FUN_004caea0
int FUN_004caea0(void)
{
  unsigned int i;
  memset(DAT_007d4578, 0, sizeof(DAT_007d4578));
  DAT_007d4574 = 0;
  DAT_006181c8 = -1;
  DAT_006181cc = -1;
  DAT_006181d0 = -1;
  DAT_006181d4 = -1;
  DAT_006181d8 = -1;
  for (i = 0; i < 4; i++) {
    DAT_007d40c0[i].a = -1;
    DAT_007d40c0[i].b = 0;
    DAT_007d40c0[i].c = 0;
  }
  DAT_007d4154 = 0;
  if (DAT_007d4568 != 0) {
    for (i = 0; i < 0x104; i++) {
      if (DAT_007d4158[i] != 0) {
        (*(RwFree2 *)(DAT_007d3ff8 + 0x11c))(DAT_007d4568, DAT_007d4158[i]);
        DAT_007d4158[i] = 0;
      }
    }
    FUN_004ccc50(DAT_007d4568);
  } else {
    DAT_007d4568 = FUN_004cc7f0(0x40, 0xf, 0x10, 0x30411);
    memset(DAT_007d4158, 0, sizeof(DAT_007d4158));
  }
  DAT_007d4558 = (*(RwAlloc2 *)(DAT_007d3ff8 + 0x118))(DAT_007d4568, 0x30411);
  *DAT_007d4558 = DAT_005d8b90;
  DAT_007d456c = 0;
  if (DAT_007d4570 != 0) {
    (*(RwFree1 *)(DAT_007d3ff8 + 0x10c))(DAT_007d4570);
    DAT_007d4570 = 0;
  }
  return DAT_007d4110;
}
