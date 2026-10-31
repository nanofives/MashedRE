// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041ec00: 100% (regs 100%), raw g2s 41.2%. By hand: typed record (objs[0x80] RpAtomic*, clump at +0x26c), Rec *p taken once, for loop with the slot-range test (n >= 0x3b && (n <= 0x3e || n == 0x40) && a) in that order. FUN_004e6e00 is RpClumpDestroy. Matched on the first hand pass. Residual: none. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef struct RpGeometry_0041ec00 { char pad0[0x20]; void **mats; int numMats; } RpGeometry_0041ec00;
typedef struct RpAtomic_0041ec00 { char type, subType, flags, privateFlags; void *parent; char pad8[0x10];
  RpGeometry_0041ec00 *geometry; } RpAtomic_0041ec00;
typedef struct S24_0041ec00 { int v[6]; } S24_0041ec00;
typedef struct S16_0041ec00 { int v[4]; } S16_0041ec00;
/* the 0x2ac-byte per-slot record at 0x0063d9e0 (4 slots: DAT_0063d830 is int[4] indexed by the same slot) */
typedef struct Rec_0041ec00 {
  RpAtomic_0041ec00 *objs[0x80];      /* 0x000 */
  char pad200[0x30];
  S24_0041ec00 f230;                  /* 0x230 */
  char pad248[0x10];
  S16_0041ec00 f258;                  /* 0x258 */
  int f268;                     /* 0x268 */
  void *clump;                  /* 0x26c */
  char pad270[0x24];
  uint flags;                   /* 0x294 */
  int f298;                     /* 0x298 */
  char pad29c[0x10];
} Rec_0041ec00;
MGLOBALA(Rec_0041ec00, DAT_0063d9e0, 4, 0x0063d9e0);
MGLOBAL(int, DAT_00636acc, 0x00636acc);
MGLOBAL(int, DAT_0063e498, 0x0063e498);
MGLOBAL(void *, DAT_0063d7e8, 0x0063d7e8);
MGLOBALA(int, DAT_0063d830, 4, 0x0063d830);
int FUN_0053d090(RpGeometry_0041ec00 *g);
int FUN_0053d060(RpGeometry_0041ec00 *g);
int FUN_004e6e00(void *clump);
void FUN_004b6520(void *p, int n);
void FUN_00422a80(int i);
int FUN_004c5930(void *p);
// MATCH: 0x0041ec00 FUN_0041ec00
void FUN_0041ec00(int i)
{
  Rec_0041ec00 *p = &DAT_0063d9e0[i];
  int n;
  for (n = 0; n < 0x80; n++) {
    RpAtomic_0041ec00 *a = p->objs[n];
    if (n >= 0x3b && (n <= 0x3e || n == 0x40) && a != 0) {
      FUN_0053d090(a->geometry);
      FUN_0053d060(a->geometry);
    }
  }
  if (p->clump != 0) {
    FUN_004e6e00(p->clump);
  }
  FUN_004b6520(p, 0x2ac);
  if (DAT_00636acc == 0) {
    FUN_00422a80(i);
  }
  if (DAT_0063e498 == 1) {
    FUN_004c5930(DAT_0063d7e8);
    DAT_0063d7e8 = 0;
  }
  DAT_0063d830[i] = 0;
  DAT_0063e498--;
}
