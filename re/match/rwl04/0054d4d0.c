// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x0054d4d0 h1: best 90.5% (regs 90.5%, ops 90.5%) with /O1 (win over /O2 /Ob1 /G5 90.4%/70.8%). Global is float (was int, gave fimul). Residue: orig reloads the written param_1 floats from memory, ours forwards them in registers/ebp frame (399 vs 387 ins). Class (a). */
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
MGLOBAL(float, DAT_005cc31c, 0x005cc31c);
// MATCH: 0x0054d4d0 FUN_0054d4d0
/* [C1 2026-05-19] 2-param `(out_dst*[float], src_control_net*[float])`. Forward-difference matrix
   builder, 3-component (xyz). */

void FUN_0054d4d0(float *param_1,float *param_2)
{
  volatile float *v = param_1;
  param_1[0x3c] = *param_2;
  param_1[0x3d] = param_2[1];
  param_1[0x3e] = param_2[2];
  param_1[0x38] = (param_2[4] - *param_2) * DAT_005cc31c;
  param_1[0x39] = (param_2[5] - param_2[1]) * DAT_005cc31c;
  param_1[0x3a] = (param_2[6] - param_2[2]) * DAT_005cc31c;
  param_1[0x34] = (param_2[8] - param_2[4]) * DAT_005cc31c;
  param_1[0x35] = (param_2[9] - param_2[5]) * DAT_005cc31c;
  param_1[0x36] = (param_2[10] - param_2[6]) * DAT_005cc31c;
  param_1[0x30] = (param_2[0xc] - *param_2) - v[0x34];
  param_1[0x31] = (param_2[0xd] - param_2[1]) - v[0x35];
  param_1[0x32] = (param_2[0xe] - param_2[2]) - v[0x36];
  param_1[0x34] = v[0x34] - v[0x38];
  param_1[0x35] = v[0x35] - v[0x39];
  param_1[0x36] = v[0x36] - v[0x3a];
  param_1[0x2c] = param_2[0x10];
  param_1[0x2d] = param_2[0x11];
  param_1[0x2e] = param_2[0x12];
  param_1[0x28] = (param_2[0x14] - param_2[0x10]) * DAT_005cc31c;
  param_1[0x29] = (param_2[0x15] - param_2[0x11]) * DAT_005cc31c;
  param_1[0x2a] = (param_2[0x16] - param_2[0x12]) * DAT_005cc31c;
  param_1[0x24] = (param_2[0x18] - param_2[0x14]) * DAT_005cc31c;
  param_1[0x25] = (param_2[0x19] - param_2[0x15]) * DAT_005cc31c;
  param_1[0x26] = (param_2[0x1a] - param_2[0x16]) * DAT_005cc31c;
  param_1[0x20] = (param_2[0x1c] - param_2[0x10]) - v[0x24];
  param_1[0x21] = (param_2[0x1d] - param_2[0x11]) - v[0x25];
  param_1[0x22] = (param_2[0x1e] - param_2[0x12]) - v[0x26];
  param_1[0x24] = v[0x24] - v[0x28];
  param_1[0x25] = v[0x25] - v[0x29];
  param_1[0x26] = v[0x26] - v[0x2a];
  param_1[0x1c] = param_2[0x20];
  param_1[0x1d] = param_2[0x21];
  param_1[0x1e] = param_2[0x22];
  param_1[0x18] = (param_2[0x24] - param_2[0x20]) * DAT_005cc31c;
  param_1[0x19] = (param_2[0x25] - param_2[0x21]) * DAT_005cc31c;
  param_1[0x1a] = (param_2[0x26] - param_2[0x22]) * DAT_005cc31c;
  param_1[0x14] = (param_2[0x28] - param_2[0x24]) * DAT_005cc31c;
  param_1[0x15] = (param_2[0x29] - param_2[0x25]) * DAT_005cc31c;
  param_1[0x16] = (param_2[0x2a] - param_2[0x26]) * DAT_005cc31c;
  param_1[0x10] = (param_2[0x2c] - param_2[0x20]) - v[0x14];
  param_1[0x11] = (param_2[0x2d] - param_2[0x21]) - v[0x15];
  param_1[0x12] = (param_2[0x2e] - param_2[0x22]) - v[0x16];
  param_1[0x14] = v[0x14] - v[0x18];
  param_1[0x15] = v[0x15] - v[0x19];
  param_1[0x16] = v[0x16] - v[0x1a];
  param_1[0xc] = param_2[0x30];
  param_1[0xd] = param_2[0x31];
  param_1[0xe] = param_2[0x32];
  param_1[8] = (param_2[0x34] - param_2[0x30]) * DAT_005cc31c;
  param_1[9] = (param_2[0x35] - param_2[0x31]) * DAT_005cc31c;
  param_1[10] = (param_2[0x36] - param_2[0x32]) * DAT_005cc31c;
  param_1[4] = (param_2[0x38] - param_2[0x34]) * DAT_005cc31c;
  param_1[5] = (param_2[0x39] - param_2[0x35]) * DAT_005cc31c;
  param_1[6] = (param_2[0x3a] - param_2[0x36]) * DAT_005cc31c;
  *param_1 = (param_2[0x3c] - param_2[0x30]) - v[4];
  param_1[1] = (param_2[0x3d] - param_2[0x31]) - v[5];
  param_1[2] = (param_2[0x3e] - param_2[0x32]) - v[6];
  param_1[4] = v[4] - v[8];
  param_1[5] = v[5] - v[9];
  param_1[6] = v[6] - v[10];
  param_1[0x10] = (v[0x10] - v[0x20]) * DAT_005cc31c;
  param_1[0x11] = (v[0x11] - v[0x21]) * DAT_005cc31c;
  param_1[0x12] = (v[0x12] - v[0x22]) * DAT_005cc31c;
  param_1[0x14] = (v[0x14] - v[0x24]) * DAT_005cc31c;
  param_1[0x15] = (v[0x15] - v[0x25]) * DAT_005cc31c;
  param_1[0x16] = (v[0x16] - v[0x26]) * DAT_005cc31c;
  param_1[0x18] = (v[0x18] - v[0x28]) * DAT_005cc31c;
  param_1[0x19] = (v[0x19] - v[0x29]) * DAT_005cc31c;
  param_1[0x1a] = (v[0x1a] - v[0x2a]) * DAT_005cc31c;
  param_1[0x1c] = (v[0x1c] - v[0x2c]) * DAT_005cc31c;
  param_1[0x1d] = (v[0x1d] - v[0x2d]) * DAT_005cc31c;
  param_1[0x1e] = (v[0x1e] - v[0x2e]) * DAT_005cc31c;
  param_1[0x20] = (v[0x20] - v[0x30]) * DAT_005cc31c;
  param_1[0x21] = (v[0x21] - v[0x31]) * DAT_005cc31c;
  param_1[0x22] = (v[0x22] - v[0x32]) * DAT_005cc31c;
  param_1[0x24] = (v[0x24] - v[0x34]) * DAT_005cc31c;
  param_1[0x25] = (v[0x25] - v[0x35]) * DAT_005cc31c;
  param_1[0x26] = (v[0x26] - v[0x36]) * DAT_005cc31c;
  param_1[0x28] = (v[0x28] - v[0x38]) * DAT_005cc31c;
  param_1[0x29] = (v[0x29] - v[0x39]) * DAT_005cc31c;
  param_1[0x2a] = (v[0x2a] - v[0x3a]) * DAT_005cc31c;
  param_1[0x2c] = (v[0x2c] - v[0x3c]) * DAT_005cc31c;
  param_1[0x2d] = (v[0x2d] - v[0x3d]) * DAT_005cc31c;
  param_1[0x2e] = (v[0x2e] - v[0x3e]) * DAT_005cc31c;
  *param_1 = (v[0] - v[0x30]) - v[0x10];
  param_1[1] = (v[1] - v[0x31]) - v[0x11];
  param_1[2] = (v[2] - v[0x32]) - v[0x12];
  param_1[4] = (v[4] - v[0x34]) - v[0x14];
  param_1[5] = (v[5] - v[0x35]) - v[0x15];
  param_1[6] = (v[6] - v[0x36]) - v[0x16];
  param_1[8] = (v[8] - v[0x38]) - v[0x18];
  param_1[9] = (v[9] - v[0x39]) - v[0x19];
  param_1[10] = (v[10] - v[0x3a]) - v[0x1a];
  param_1[0xc] = (v[0xc] - v[0x3c]) - v[0x1c];
  param_1[0xd] = (v[0xd] - v[0x3d]) - v[0x1d];
  param_1[0xe] = (v[0xe] - v[0x3e]) - v[0x1e];
  param_1[0x10] = v[0x10] - v[0x20];
  param_1[0x11] = v[0x11] - v[0x21];
  param_1[0x12] = v[0x12] - v[0x22];
  param_1[0x14] = v[0x14] - v[0x24];
  param_1[0x15] = v[0x15] - v[0x25];
  param_1[0x16] = v[0x16] - v[0x26];
  param_1[0x18] = v[0x18] - v[0x28];
  param_1[0x19] = v[0x19] - v[0x29];
  param_1[0x1a] = v[0x1a] - v[0x2a];
  param_1[0x1c] = v[0x1c] - v[0x2c];
  param_1[0x1d] = v[0x1d] - v[0x2d];
  param_1[0x1e] = v[0x1e] - v[0x2e];
}
