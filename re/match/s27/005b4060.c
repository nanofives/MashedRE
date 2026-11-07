// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b4060: 88.4% (regs 88.4%). Hand: float->int is inline fnstcw/or ah,0xc/fldcw/fistp/fldcw in the original (cl 9176 only emits __ftol), reproduced with one FTOI __asm macro (C++ build uses (int) cast). Residual (b): original converts *param_1 and param_1[2] with fidiv/fimul dword through a zero-high stack temp, the compiler emits fild qword + fdivp/fmulp for uint; int*, int temps, (longlong) casts did not give that form. */
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
typedef unsigned int uint;
typedef unsigned char byte;
#ifdef __cplusplus
#define FTOI(res, f) (res) = (int)(f)
#else
/* original converts float->int inline: fnstcw / or ah,0xc / fldcw / fistp / fldcw */
#define FTOI(res, f) do { float t_ = (f); int r_; short c1_, c2_; \
  __asm { __asm fnstcw c1_ __asm fld t_ __asm mov ax, c1_ __asm or ah, 0xc __asm mov c2_, ax \
          __asm fldcw c2_ __asm fistp r_ __asm fldcw c1_ } (res) = r_; } while (0)
#endif
// MATCH: 0x005b4060 FUN_005b4060
int FUN_005b4060(uint *param_1, uint *param_2)
{
  uint uVar1;
  int r;
  int r2;

  uVar1 = (uint)(byte)((byte)param_2[3] >> 3);
  FTOI(r, ((float)*param_2 / (float)*param_1) * ((float)(byte)param_2[3] / (float)*(byte *)(param_1 + 3)) *
          ((float)*(byte *)((int)param_2 + 0xd) / (float)*(byte *)((int)param_1 + 0xd)) * (float)param_1[2]);
  FTOI(r2, (float)((r + -1 + uVar1) & ~(uVar1 - 1)));
  return r2;
}
