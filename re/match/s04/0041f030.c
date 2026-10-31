// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041f030: 100% (regs 100%), raw g2s 57.1%. Written by hand on a typed record: Rec *p = &DAT_0063d9e0[i] taken once (the original adds the base into the scaled index, then addresses p->field). That is what moved it from the raw converter byte-offset form (&DAT_0063dc74 + i*0x2ac). Residual: none. 16-byte struct copy -> 4 unrolled dword moves. */
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
typedef struct RpGeometry_0041f030 { char pad0[0x20]; void **mats; int numMats; } RpGeometry_0041f030;
typedef struct RpAtomic_0041f030 { char type, subType, flags, privateFlags; void *parent; char pad8[0x10];
  RpGeometry_0041f030 *geometry; } RpAtomic_0041f030;
typedef struct S24_0041f030 { int v[6]; } S24_0041f030;
typedef struct S16_0041f030 { int v[4]; } S16_0041f030;
/* the 0x2ac-byte per-slot record at 0x0063d9e0 (4 slots: DAT_0063d830 is int[4] indexed by the same slot) */
typedef struct Rec_0041f030 {
  RpAtomic_0041f030 *objs[0x80];      /* 0x000 */
  char pad200[0x30];
  S24_0041f030 f230;                  /* 0x230 */
  char pad248[0x10];
  S16_0041f030 f258;                  /* 0x258 */
  int f268;                     /* 0x268 */
  void *clump;                  /* 0x26c */
  char pad270[0x24];
  uint flags;                   /* 0x294 */
  int f298;                     /* 0x298 */
  char pad29c[0x10];
} Rec_0041f030;
MGLOBALA(Rec_0041f030, DAT_0063d9e0, 4, 0x0063d9e0);
// MATCH: 0x0041f030 FUN_0041f030
void FUN_0041f030(int i, S16_0041f030 *out)
{
  Rec_0041f030 *p = &DAT_0063d9e0[i];
  *out = p->f258;
}
