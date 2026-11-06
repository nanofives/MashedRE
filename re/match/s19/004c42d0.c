// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c42d0 (h5): 100% (regs 100%). Matrix normal error: |right|^2-1, |up|^2-1, |at|^2-1 into an RwV3d
   temp, returning its squared length. Each row goes through float locals (y, x, z load order) so the compiler keeps
   them on the x87 stack; the first row's helper squares y in place (`y *= y`), which is what makes the original
   accumulate into y's register (fmulp st(3)) for row 1 only (same helper for all rows: 86.2%; permuter no gain).
   Ghidra typed it void(void); it returns float in ST0. The 1.0f literal is the original's 0x005cc320.
   FLAGS: /G5 added (h5 finding, 2026-11-06): the RW core in this slice is a Pentium (/G3-/G5) build. Under the
   game flags /O2 /GS /Ob1 alone (= /G6 scheduling and no byte-register ALU ops) this file scores  100% as well. */
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
static __inline float DOT(const RwV3d *a, const RwV3d *b) { return a->z * b->z + (a->x * b->x + a->y * b->y); }
static __inline float LEN2(const RwV3d *a) { float y = a->y, x = a->x, z = a->z; return z * z + (x * x + y * y); }
static __inline float LEN2b(const RwV3d *a) { float y = a->y, x = a->x, z = a->z; y *= y; return z * z + (x * x + y); }

// MATCH: 0x004c42d0 FUN_004c42d0
float FUN_004c42d0(const RwMatrix *matrix)
{
  RwV3d vTmp;
  vTmp.x = LEN2b(&matrix->right) - 1.0f;
  vTmp.y = LEN2(&matrix->up) - 1.0f;
  vTmp.z = LEN2(&matrix->at) - 1.0f;
  return DOT(&vTmp, &vTmp);
}
