// TOOLCHAIN rw9044
// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c18c0 (h1 form, h6): 65.2%, regs 65.2% (the earlier 100% in this header does not reproduce: match.py gives 65.2%, 36/46 ins). RwBBoxContainsPoint shape: inf compares then sup compares. Residual (b): the original repeats xor eax,eax; ret after each of the six compares, 9176 here tail-merges them into one return-0 block. Tried: RwBBoxContainsPointMacro return expression (30.4), if(&&) return 1, reversed compare operands (47.8), nested if/else returns, negated <= forms (52.2), flagscan 19 sets (best /G5 65.2). */
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
typedef struct RwBBox { RwV3d sup; RwV3d inf; } RwBBox;
// MATCH: 0x004c18c0 FUN_004c18c0
int FUN_004c18c0(const RwBBox *boundBox, const RwV3d *vertex)
{
  if (boundBox->inf.x > vertex->x) return 0;
  if (boundBox->inf.y > vertex->y) return 0;
  if (boundBox->inf.z > vertex->z) return 0;
  if (boundBox->sup.x < vertex->x) return 0;
  if (boundBox->sup.y < vertex->y) return 0;
  if (boundBox->sup.z < vertex->z) return 0;
  return 1;
}
