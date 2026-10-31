// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041ecc0: 100% (regs 100%), raw g2s 70.1%. By hand: typed record (objs[0x80] RpAtomic*, atomic+0x18 geometry, geometry+0x20/+0x24 material array/count), Rec *p taken once (spilled to the arg slot as in the original), for loops, and the switch on the slot index (cases 0-3/0x21-0x24 -> c2, default c1) written as a switch calling FUN_005401d0 in both arms: the compiler merges the calls and emits the byte-indexed jump table at 0x0041ed70 past the Ghidra size. Matched on the first hand pass. Residual: none. */
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
typedef struct RpGeometry_0041ecc0 { char pad0[0x20]; void **mats; int numMats; } RpGeometry_0041ecc0;
typedef struct RpAtomic_0041ecc0 { char type, subType, flags, privateFlags; void *parent; char pad8[0x10];
  RpGeometry_0041ecc0 *geometry; } RpAtomic_0041ecc0;
typedef struct S24_0041ecc0 { int v[6]; } S24_0041ecc0;
typedef struct S16_0041ecc0 { int v[4]; } S16_0041ecc0;
/* the 0x2ac-byte per-slot record at 0x0063d9e0 (4 slots: DAT_0063d830 is int[4] indexed by the same slot) */
typedef struct Rec_0041ecc0 {
  RpAtomic_0041ecc0 *objs[0x80];      /* 0x000 */
  char pad200[0x30];
  S24_0041ecc0 f230;                  /* 0x230 */
  char pad248[0x10];
  S16_0041ecc0 f258;                  /* 0x258 */
  int f268;                     /* 0x268 */
  void *clump;                  /* 0x26c */
  char pad270[0x24];
  uint flags;                   /* 0x294 */
  int f298;                     /* 0x298 */
  char pad29c[0x10];
} Rec_0041ecc0;
MGLOBALA(Rec_0041ecc0, DAT_0063d9e0, 4, 0x0063d9e0);
MGLOBAL(int, DAT_00636ad4, 0x00636ad4);
int FUN_0053fba0(RpAtomic_0041ecc0 *a);
int FUN_0053fea0(void *m);
int FUN_0053fbb0(void *m, int v);
int FUN_005401d0(void *m, int v);
// MATCH: 0x0041ecc0 FUN_0041ecc0
void FUN_0041ecc0(int i, int c1, int c2)
{
  Rec_0041ecc0 *p = &DAT_0063d9e0[i];
  int n, k;
  for (n = 0; n < 0x80; n++) {
    RpAtomic_0041ecc0 *a = p->objs[n];
    if (a != 0) {
      RpGeometry_0041ecc0 *g = a->geometry;
      if (FUN_0053fba0(a) != 0) {
        for (k = 0; k < g->numMats; k++) {
          void *m = g->mats[k];
          if (FUN_0053fea0(m) == 4) {
            if (DAT_00636ad4 != 0) {
              FUN_0053fbb0(m, 0);
            } else {
              switch (n) {
              case 0: case 1: case 2: case 3:
              case 0x21: case 0x22: case 0x23: case 0x24:
                FUN_005401d0(m, c2);
                break;
              default:
                FUN_005401d0(m, c1);
                break;
              }
            }
          }
        }
      }
    }
  }
}
