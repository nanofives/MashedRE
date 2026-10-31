// FLAGS: /O2 /GS /Ob1
/* 0x0040ad90 (148 B): best 97.9% exact, regs 97.9%, ops 97.9%. Raw g2s 31.9%.
   Form: m = FUN_0046d4a0 matrix, v = at * (param_2 * 360.0f), t = up * (param_3 * 360.0f), v = t + v,
   FUN_0046bf50(&v, param_1). 0x005ccac4 (= 360.0f) is written as a literal: a named global makes 9176
   load it first (fld [K]; fmul [param]); the literal gives the original's fld [param]; fmul [K]
   (61.7% -> 70.2%). Each scale held in a `double` block local gives the original's x87 schedule
   (fld mem; fmul st(1) ... fstp st(0)), as in sibling 0x0040ae30: 70.2% -> 97.9%.
   Residual (b) source shape: one instruction, the first `fmul st(1)` sits before the `push esi`
   (FUN_0046bf50's hoisted argument) in the original and after it in ours. Tried: float/double mixes,
   function-level vs block locals, declaration orders, operand orders (s*m vs m*s), cast spellings of the
   scale, an __inline V3dScale/V3dAdd pair (same code as the expanded form), /Op and /Oy- (worse), the
   permuter (2 runs, 236 compiles, nothing). */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
typedef struct { float x; float y; float z; } RwV3d;
typedef struct {
  RwV3d right; uint flags; RwV3d up; uint pad1; RwV3d at; uint pad2; RwV3d pos; uint pad3;
} RwMatrix;

int FUN_0046bf50(RwV3d *p0, int p1);
int FUN_0046d4a0(RwMatrix **p0, int p1);

// MATCH: 0x0040ad90 FUN_0040ad90
void FUN_0040ad90(int param_1, float param_2, float param_3)
{
  RwMatrix *m;
  RwV3d v;
  RwV3d t;

  FUN_0046d4a0(&m, param_1);
  {
    double s = param_2 * 360.0f;
    v.x = m->at.x * s;
    v.y = m->at.y * s;
    v.z = m->at.z * s;
  }
  {
    double s = param_3 * 360.0f;
    t.x = m->up.x * s;
    t.y = m->up.y * s;
    t.z = m->up.z * s;
  }
  v.x = t.x + v.x;
  v.y = t.y + v.y;
  v.z = t.z + v.z;
  FUN_0046bf50(&v, param_1);
}
