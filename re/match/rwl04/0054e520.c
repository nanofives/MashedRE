// FLAGS: /O2 /Ob1 /G5
/* h3: the float at 0x005cd0a0 (6.0f in .rdata, read from MASHED.exe.unpatched) is written as the literal 6.0f: a literal makes cl emit fmul [const] in the original operand order, a global read does not (+10 points). */
/* MATCH-RWL04 0x0054e520: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
typedef unsigned char undefined1;
// MATCH: 0x0054e520 FUN_0054e520
/* [C1 2026-05-19] 4-param `(out_state, fd_matrix, u_step, v_step)`. 3-coord forward-difference
   seed. */

void FUN_0054e520(float *param_1,float *param_2,float param_3,float param_4)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  
  fVar1 = param_3 * param_3;
  fVar2 = param_3 * fVar1;
  fVar13 = param_3 * param_4;
  fVar12 = fVar1 * param_4;
  fVar10 = fVar2 * param_4;
  fVar11 = param_4 * param_4;
  fVar5 = param_3 * fVar11;
  fVar3 = fVar11 * fVar1;
  fVar8 = fVar11 * fVar2;
  fVar6 = fVar11 * param_4;
  fVar7 = fVar6 * param_3;
  fVar4 = fVar6 * fVar1;
  fVar9 = fVar6 * fVar2;
  param_1[0x10] = fVar6 * param_2[0x30];
  param_1[0x11] = fVar6 * param_2[0x31];
  param_1[0x12] = fVar6 * param_2[0x32];
  param_1[0x14] = fVar9 * *param_2;
  param_1[0x15] = fVar9 * param_2[1];
  param_1[0x16] = fVar9 * param_2[2];
  param_1[0x18] = fVar4 * param_2[0x10];
  param_1[0x19] = fVar4 * param_2[0x11];
  param_1[0x1a] = fVar4 * param_2[0x12];
  param_1[0x1c] = param_1[0x14] * 6.0f;
  param_1[0x1d] = param_1[0x15] * 6.0f;
  param_1[0x1e] = param_1[0x16] * 6.0f;
  param_1[0x14] = fVar7 * param_2[0x20] + param_1[0x14] + param_1[0x18];
  param_1[0x15] = fVar7 * param_2[0x21] + param_1[0x19] + param_1[0x15];
  param_1[0x16] = fVar7 * param_2[0x22] + param_1[0x16] + param_1[0x1a];
  param_1[0x18] = param_1[0x18] + param_1[0x18] + param_1[0x1c];
  param_1[0x19] = param_1[0x19] + param_1[0x19] + param_1[0x1d];
  param_1[0x1a] = param_1[0x1a] + param_1[0x1a] + param_1[0x1e];
  param_1[0x20] = fVar11 * param_2[0x34];
  param_1[0x21] = fVar11 * param_2[0x35];
  param_1[0x22] = fVar11 * param_2[0x36];
  param_1[0x24] = fVar8 * param_2[4];
  param_1[0x25] = fVar8 * param_2[5];
  param_1[0x26] = fVar8 * param_2[6];
  param_1[0x28] = fVar3 * param_2[0x14];
  param_1[0x29] = fVar3 * param_2[0x15];
  param_1[0x2a] = fVar3 * param_2[0x16];
  param_1[0x2c] = param_1[0x24] * 6.0f;
  param_1[0x2d] = param_1[0x25] * 6.0f;
  param_1[0x2e] = param_1[0x26] * 6.0f;
  param_1[0x24] = fVar5 * param_2[0x24] + param_1[0x28] + param_1[0x24];
  param_1[0x25] = fVar5 * param_2[0x25] + param_1[0x25] + param_1[0x29];
  param_1[0x26] = fVar5 * param_2[0x26] + param_1[0x2a] + param_1[0x26];
  param_1[0x28] = param_1[0x28] + param_1[0x28] + param_1[0x2c];
  param_1[0x29] = param_1[0x29] + param_1[0x29] + param_1[0x2d];
  param_1[0x2a] = param_1[0x2a] + param_1[0x2a] + param_1[0x2e];
  param_1[0x30] = param_1[0x10] * 6.0f;
  param_1[0x31] = param_1[0x11] * 6.0f;
  param_1[0x32] = param_1[0x12] * 6.0f;
  param_1[0x34] = param_1[0x14] * 6.0f;
  param_1[0x35] = param_1[0x15] * 6.0f;
  param_1[0x36] = param_1[0x16] * 6.0f;
  param_1[0x38] = param_1[0x18] * 6.0f;
  param_1[0x39] = param_1[0x19] * 6.0f;
  param_1[0x3a] = param_1[0x1a] * 6.0f;
  param_1[0x3c] = param_1[0x1c] * 6.0f;
  param_1[0x3d] = param_1[0x1d] * 6.0f;
  param_1[0x3e] = param_1[0x1e] * 6.0f;
  *param_1 = param_4 * param_2[0x38];
  param_1[1] = param_4 * param_2[0x39];
  param_1[2] = param_4 * param_2[0x3a];
  param_1[4] = fVar10 * param_2[8];
  param_1[5] = fVar10 * param_2[9];
  param_1[6] = fVar10 * param_2[10];
  param_1[8] = fVar12 * param_2[0x18];
  param_1[9] = fVar12 * param_2[0x19];
  param_1[10] = fVar12 * param_2[0x1a];
  param_1[0xc] = param_1[4] * 6.0f;
  param_1[0xd] = param_1[5] * 6.0f;
  param_1[0xe] = param_1[6] * 6.0f;
  param_1[4] = fVar13 * param_2[0x28] + param_1[8] + param_1[4];
  param_1[5] = fVar13 * param_2[0x29] + param_1[9] + param_1[5];
  param_1[6] = fVar13 * param_2[0x2a] + param_1[10] + param_1[6];
  param_1[8] = param_1[8] + param_1[8] + param_1[0xc];
  param_1[9] = param_1[9] + param_1[9] + param_1[0xd];
  param_1[10] = param_1[10] + param_1[10] + param_1[0xe];
  param_1[0x10] = param_1[0x20] + param_1[0x10] + *param_1;
  param_1[0x11] = param_1[0x21] + param_1[0x11] + param_1[1];
  param_1[0x12] = param_1[0x22] + param_1[0x12] + param_1[2];
  param_1[0x14] = param_1[0x14] + param_1[4] + param_1[0x24];
  param_1[0x15] = param_1[0x25] + param_1[0x15] + param_1[5];
  param_1[0x16] = param_1[0x16] + param_1[6] + param_1[0x26];
  param_1[0x18] = param_1[8] + param_1[0x28] + param_1[0x18];
  param_1[0x19] = param_1[9] + param_1[0x19] + param_1[0x29];
  param_1[0x1a] = param_1[10] + param_1[0x2a] + param_1[0x1a];
  param_1[0x1c] = param_1[0xc] + param_1[0x2c] + param_1[0x1c];
  param_1[0x1d] = param_1[0xd] + param_1[0x2d] + param_1[0x1d];
  param_1[0x1e] = param_1[0xe] + param_1[0x2e] + param_1[0x1e];
  param_1[0x20] = param_1[0x20] + param_1[0x20] + param_1[0x30];
  param_1[0x21] = param_1[0x21] + param_1[0x21] + param_1[0x31];
  param_1[0x22] = param_1[0x22] + param_1[0x22] + param_1[0x32];
  param_1[0x24] = param_1[0x24] + param_1[0x24] + param_1[0x34];
  param_1[0x25] = param_1[0x25] + param_1[0x25] + param_1[0x35];
  param_1[0x26] = param_1[0x26] + param_1[0x26] + param_1[0x36];
  param_1[0x28] = param_1[0x28] + param_1[0x28] + param_1[0x38];
  param_1[0x29] = param_1[0x29] + param_1[0x29] + param_1[0x39];
  param_1[0x2a] = param_1[0x2a] + param_1[0x2a] + param_1[0x3a];
  param_1[0x2c] = param_1[0x2c] + param_1[0x2c] + param_1[0x3c];
  param_1[0x2d] = param_1[0x2d] + param_1[0x2d] + param_1[0x3d];
  param_1[0x2e] = param_1[0x2e] + param_1[0x2e] + param_1[0x3e];
  *param_1 = param_2[0x3c];
  param_1[1] = param_2[0x3d];
  param_1[2] = param_2[0x3e];
  param_1[4] = fVar2 * param_2[0xc];
  param_1[5] = fVar2 * param_2[0xd];
  param_1[6] = fVar2 * param_2[0xe];
  param_1[8] = fVar1 * param_2[0x1c];
  param_1[9] = fVar1 * param_2[0x1d];
  param_1[10] = fVar1 * param_2[0x1e];
  param_1[0xc] = param_1[4] * 6.0f;
  param_1[0xd] = param_1[5] * 6.0f;
  param_1[0xe] = param_1[6] * 6.0f;
  param_1[4] = param_3 * param_2[0x2c] + param_1[8] + param_1[4];
  param_1[5] = param_3 * param_2[0x2d] + param_1[9] + param_1[5];
  param_1[6] = param_3 * param_2[0x2e] + param_1[10] + param_1[6];
  param_1[8] = param_1[8] + param_1[8] + param_1[0xc];
  param_1[9] = param_1[9] + param_1[9] + param_1[0xd];
  param_1[10] = param_1[10] + param_1[10] + param_1[0xe];
  return;
}
