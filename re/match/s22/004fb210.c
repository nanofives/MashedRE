// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004fb210 (h7): best 92.2% (regs 92.2%, ops 100%) under /O2 /Ob1 /G5 (no ebp frame, unlike its
   /Oy- neighbours 0x004fb050/0x004fb0e0/0x004fb130 that call it). Tried: every term order and operand order for
   each of the three dot products in a flat sum and in both groupings (144+ variants), an RwMatrix struct form with a
   DOT macro, the permuter (154 compiles, 0 gains). Rows 0xd/0xe ([0x34] grouping) moved with z + (x + y) grouping.
   Residual class (a): x87 operand order inside two products (fld [4]; fmul [0x34] against fld [0x34]; fmul [4],
   same for [0x14]): source operand order does not move it (the compiler canonicalises the product), which is the
   x87 operand-order residual listed in the MATCH-RWCORE-G5 flags brief. */
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
// MATCH: 0x004fb210 FUN_004fb210
void FUN_004fb210(float *param_1, float *param_2)
{
  param_1[0xc] = -(param_2[0xe] * param_2[2] + (param_2[0xc] * param_2[0] + param_2[0xd] * param_2[1]));
  param_1[0] = param_2[0];
  param_1[4] = param_2[1];
  param_1[8] = param_2[2];
  param_1[0xd] = -(param_2[0xe] * param_2[6] + (param_2[0xc] * param_2[4] + param_2[0xd] * param_2[5]));
  param_1[1] = param_2[4];
  param_1[5] = param_2[5];
  param_1[9] = param_2[6];
  param_1[0xe] = -(param_2[0xc] * param_2[8] + param_2[0xd] * param_2[9] + param_2[0xe] * param_2[10]);
  param_1[2] = param_2[8];
  param_1[6] = param_2[9];
  param_1[10] = param_2[10];
}
