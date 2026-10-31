// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041f060: 100% (regs 100%), raw g2s 38.9%. objs[0x16] (+0x58) is an RpAtomic. Its frame (RwObject.parent, +4) goes to FUN_004c0ed0 (returns frame+0x50 after a sync, RwFrameGetLTM shape), and the 64-byte matrix is struct-copied out (rep movsd x16). Plain DAT_0063d9e0[i].objs[0x16]->parent gave 87.5% (edx instead of eax for the loaded frame, permuter found 0 variants). Reading the frame through a static __inline accessor (RpAtomicGetFrame shape) gives 100%: the inlined return value lands in eax. The accessor is inlined (/Ob1), not emitted, no MATCH line of its own. Residual: none. */
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
typedef struct RpGeometry_0041f060 { char pad0[0x20]; void **mats; int numMats; } RpGeometry_0041f060;
typedef struct RpAtomic_0041f060 { char type, subType, flags, privateFlags; void *parent; char pad8[0x10];
  RpGeometry_0041f060 *geometry; } RpAtomic_0041f060;
typedef struct S24_0041f060 { int v[6]; } S24_0041f060;
typedef struct S16_0041f060 { int v[4]; } S16_0041f060;
/* the 0x2ac-byte per-slot record at 0x0063d9e0 (4 slots: DAT_0063d830 is int[4] indexed by the same slot) */
typedef struct Rec_0041f060 {
  RpAtomic_0041f060 *objs[0x80];      /* 0x000 */
  char pad200[0x30];
  S24_0041f060 f230;                  /* 0x230 */
  char pad248[0x10];
  S16_0041f060 f258;                  /* 0x258 */
  int f268;                     /* 0x268 */
  void *clump;                  /* 0x26c */
  char pad270[0x24];
  uint flags;                   /* 0x294 */
  int f298;                     /* 0x298 */
  char pad29c[0x10];
} Rec_0041f060;
MGLOBALA(Rec_0041f060, DAT_0063d9e0, 4, 0x0063d9e0);
typedef struct S64_0041f060 { float m[16]; } S64_0041f060; /* 0x40-byte matrix */
S64_0041f060 *FUN_004c0ed0(void *frame);
static __inline void *RpAtomicGetFrame_0041f060(RpAtomic_0041f060 *a)
{
  return a->parent;
}
// MATCH: 0x0041f060 FUN_0041f060
void FUN_0041f060(int i, S64_0041f060 *out)
{
  *out = *FUN_004c0ed0(RpAtomicGetFrame_0041f060(DAT_0063d9e0[i].objs[0x16]));
}
