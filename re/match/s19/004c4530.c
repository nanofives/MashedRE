// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c4530 (h5): 100% (regs 100%). RwMatrixOptimize: tolerance (NULL -> RwGlobals module +0xc),
   normal/orthogonal/identity tests through FUN_004c42d0 / FUN_004c4270 / FUN_004c4360 (each float(const RwMatrix *)),
   flags word read once into a local, bits 1 / 2 / 0x20000 set or cleared, one store per exit.
   FLAGS: /G5 added (h5 finding, 2026-11-06): the RW core in this slice is a Pentium (/G3-/G5) build. Under the
   game flags /O2 /GS /Ob1 alone (= /G6 scheduling and no byte-register ALU ops) this file scores  86.5%: `or al,1` / `and al,0xfe` byte ops
   (9176 /G6 emits `or eax,1`) and the flags load placement. This function is what exposed the /G5 build. */
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
typedef struct RwV3d { float x; float y; float z; } RwV3d;
typedef struct RwMatrix { RwV3d right; unsigned int flags; RwV3d up; unsigned int pad1; RwV3d at; unsigned int pad2; RwV3d pos; unsigned int pad3; } RwMatrix;
typedef struct RwMatrixTolerance { float Normal; float Orthogonal; float Identity; } RwMatrixTolerance;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4028, 0x007d4028);
float FUN_004c42d0(const RwMatrix *matrix);
float FUN_004c4270(const RwMatrix *matrix);
float FUN_004c4360(const RwMatrix *matrix);

// MATCH: 0x004c4530 FUN_004c4530
RwMatrix *FUN_004c4530(RwMatrix *matrix, const RwMatrixTolerance *tolerance)
{
  int normal, orthogonal, identity;
  unsigned int flags;
  if (!tolerance) {
    tolerance = (const RwMatrixTolerance *)(DAT_007d3ff8 + DAT_007d4028 + 0xc);
  }
  normal = FUN_004c42d0(matrix) <= tolerance->Normal;
  orthogonal = FUN_004c4270(matrix) <= tolerance->Orthogonal;
  identity = normal && orthogonal && FUN_004c4360(matrix) <= tolerance->Identity;
  flags = matrix->flags;
  if (normal) {
    flags |= 1;
  } else {
    flags &= ~1;
  }
  if (orthogonal) {
    flags |= 2;
  } else {
    flags &= ~2;
  }
  if (identity) {
    matrix->flags = flags | 0x20000;
  } else {
    matrix->flags = flags & ~0x20000;
  }
  return matrix;
}
