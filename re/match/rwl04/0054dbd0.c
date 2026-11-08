// FLAGS: /O1
/* h2 note: 78.9% raw form, /O1 flag. cxx ok. Residue fsub [ebp-8] vs fsub [eax+0xdc] (field vs temp spill) and leave frame. class (b). */
/* MATCH-RWL04 0x0054dbd0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(undefined1, DAT_005cc31c, 0x005cc31c);
MGLOBAL(int, _DAT_005cc31c, 0x005cc31c);
// MATCH: 0x0054dbd0 FUN_0054dbd0
/* [C1 2026-05-19] 2-param `(out_dst*, src_control_net*)`. Identical structure to FUN_0054d4d0 but
   for 4-component (RGBA) source. */

void FUN_0054dbd0(float *param_1,float *param_2)

{
  param_1[0x3c] = *param_2;
  param_1[0x3d] = param_2[1];
  param_1[0x3e] = param_2[2];
  param_1[0x3f] = param_2[3];
  param_1[0x38] = (param_2[4] - *param_2) * _DAT_005cc31c;
  param_1[0x39] = (param_2[5] - param_2[1]) * _DAT_005cc31c;
  param_1[0x3a] = (param_2[6] - param_2[2]) * _DAT_005cc31c;
  param_1[0x3b] = (param_2[7] - param_2[3]) * _DAT_005cc31c;
  param_1[0x34] = (param_2[8] - param_2[4]) * _DAT_005cc31c;
  param_1[0x35] = (param_2[9] - param_2[5]) * _DAT_005cc31c;
  param_1[0x36] = (param_2[10] - param_2[6]) * _DAT_005cc31c;
  param_1[0x37] = (param_2[0xb] - param_2[7]) * _DAT_005cc31c;
  param_1[0x30] = (param_2[0xc] - *param_2) - param_1[0x34];
  param_1[0x31] = (param_2[0xd] - param_2[1]) - param_1[0x35];
  param_1[0x32] = (param_2[0xe] - param_2[2]) - param_1[0x36];
  param_1[0x33] = (param_2[0xf] - param_2[3]) - param_1[0x37];
  param_1[0x34] = param_1[0x34] - param_1[0x38];
  param_1[0x35] = param_1[0x35] - param_1[0x39];
  param_1[0x36] = param_1[0x36] - param_1[0x3a];
  param_1[0x37] = param_1[0x37] - param_1[0x3b];
  param_1[0x2c] = param_2[0x10];
  param_1[0x2d] = param_2[0x11];
  param_1[0x2e] = param_2[0x12];
  param_1[0x2f] = param_2[0x13];
  param_1[0x28] = (param_2[0x14] - param_2[0x10]) * _DAT_005cc31c;
  param_1[0x29] = (param_2[0x15] - param_2[0x11]) * _DAT_005cc31c;
  param_1[0x2a] = (param_2[0x16] - param_2[0x12]) * _DAT_005cc31c;
  param_1[0x2b] = (param_2[0x17] - param_2[0x13]) * _DAT_005cc31c;
  param_1[0x24] = (param_2[0x18] - param_2[0x14]) * _DAT_005cc31c;
  param_1[0x25] = (param_2[0x19] - param_2[0x15]) * _DAT_005cc31c;
  param_1[0x26] = (param_2[0x1a] - param_2[0x16]) * _DAT_005cc31c;
  param_1[0x27] = (param_2[0x1b] - param_2[0x17]) * _DAT_005cc31c;
  param_1[0x20] = (param_2[0x1c] - param_2[0x10]) - param_1[0x24];
  param_1[0x21] = (param_2[0x1d] - param_2[0x11]) - param_1[0x25];
  param_1[0x22] = (param_2[0x1e] - param_2[0x12]) - param_1[0x26];
  param_1[0x23] = (param_2[0x1f] - param_2[0x13]) - param_1[0x27];
  param_1[0x24] = param_1[0x24] - param_1[0x28];
  param_1[0x25] = param_1[0x25] - param_1[0x29];
  param_1[0x26] = param_1[0x26] - param_1[0x2a];
  param_1[0x27] = param_1[0x27] - param_1[0x2b];
  param_1[0x1c] = param_2[0x20];
  param_1[0x1d] = param_2[0x21];
  param_1[0x1e] = param_2[0x22];
  param_1[0x1f] = param_2[0x23];
  param_1[0x18] = (param_2[0x24] - param_2[0x20]) * _DAT_005cc31c;
  param_1[0x19] = (param_2[0x25] - param_2[0x21]) * _DAT_005cc31c;
  param_1[0x1a] = (param_2[0x26] - param_2[0x22]) * _DAT_005cc31c;
  param_1[0x1b] = (param_2[0x27] - param_2[0x23]) * _DAT_005cc31c;
  param_1[0x14] = (param_2[0x28] - param_2[0x24]) * _DAT_005cc31c;
  param_1[0x15] = (param_2[0x29] - param_2[0x25]) * _DAT_005cc31c;
  param_1[0x16] = (param_2[0x2a] - param_2[0x26]) * _DAT_005cc31c;
  param_1[0x17] = (param_2[0x2b] - param_2[0x27]) * _DAT_005cc31c;
  param_1[0x10] = (param_2[0x2c] - param_2[0x20]) - param_1[0x14];
  param_1[0x11] = (param_2[0x2d] - param_2[0x21]) - param_1[0x15];
  param_1[0x12] = (param_2[0x2e] - param_2[0x22]) - param_1[0x16];
  param_1[0x13] = (param_2[0x2f] - param_2[0x23]) - param_1[0x17];
  param_1[0x14] = param_1[0x14] - param_1[0x18];
  param_1[0x15] = param_1[0x15] - param_1[0x19];
  param_1[0x16] = param_1[0x16] - param_1[0x1a];
  param_1[0x17] = param_1[0x17] - param_1[0x1b];
  param_1[0xc] = param_2[0x30];
  param_1[0xd] = param_2[0x31];
  param_1[0xe] = param_2[0x32];
  param_1[0xf] = param_2[0x33];
  param_1[8] = (param_2[0x34] - param_2[0x30]) * _DAT_005cc31c;
  param_1[9] = (param_2[0x35] - param_2[0x31]) * _DAT_005cc31c;
  param_1[10] = (param_2[0x36] - param_2[0x32]) * _DAT_005cc31c;
  param_1[0xb] = (param_2[0x37] - param_2[0x33]) * _DAT_005cc31c;
  param_1[4] = (param_2[0x38] - param_2[0x34]) * _DAT_005cc31c;
  param_1[5] = (param_2[0x39] - param_2[0x35]) * _DAT_005cc31c;
  param_1[6] = (param_2[0x3a] - param_2[0x36]) * _DAT_005cc31c;
  param_1[7] = (param_2[0x3b] - param_2[0x37]) * _DAT_005cc31c;
  *param_1 = (param_2[0x3c] - param_2[0x30]) - param_1[4];
  param_1[1] = (param_2[0x3d] - param_2[0x31]) - param_1[5];
  param_1[2] = (param_2[0x3e] - param_2[0x32]) - param_1[6];
  param_1[3] = (param_2[0x3f] - param_2[0x33]) - param_1[7];
  param_1[4] = param_1[4] - param_1[8];
  param_1[5] = param_1[5] - param_1[9];
  param_1[6] = param_1[6] - param_1[10];
  param_1[7] = param_1[7] - param_1[0xb];
  param_1[0x10] = (param_1[0x10] - param_1[0x20]) * _DAT_005cc31c;
  param_1[0x11] = (param_1[0x11] - param_1[0x21]) * _DAT_005cc31c;
  param_1[0x12] = (param_1[0x12] - param_1[0x22]) * _DAT_005cc31c;
  param_1[0x13] = (param_1[0x13] - param_1[0x23]) * _DAT_005cc31c;
  param_1[0x14] = (param_1[0x14] - param_1[0x24]) * _DAT_005cc31c;
  param_1[0x15] = (param_1[0x15] - param_1[0x25]) * _DAT_005cc31c;
  param_1[0x16] = (param_1[0x16] - param_1[0x26]) * _DAT_005cc31c;
  param_1[0x17] = (param_1[0x17] - param_1[0x27]) * _DAT_005cc31c;
  param_1[0x18] = (param_1[0x18] - param_1[0x28]) * _DAT_005cc31c;
  param_1[0x19] = (param_1[0x19] - param_1[0x29]) * _DAT_005cc31c;
  param_1[0x1a] = (param_1[0x1a] - param_1[0x2a]) * _DAT_005cc31c;
  param_1[0x1b] = (param_1[0x1b] - param_1[0x2b]) * _DAT_005cc31c;
  param_1[0x1c] = (param_1[0x1c] - param_1[0x2c]) * _DAT_005cc31c;
  param_1[0x1d] = (param_1[0x1d] - param_1[0x2d]) * _DAT_005cc31c;
  param_1[0x1e] = (param_1[0x1e] - param_1[0x2e]) * _DAT_005cc31c;
  param_1[0x1f] = (param_1[0x1f] - param_1[0x2f]) * _DAT_005cc31c;
  param_1[0x20] = (param_1[0x20] - param_1[0x30]) * _DAT_005cc31c;
  param_1[0x21] = (param_1[0x21] - param_1[0x31]) * _DAT_005cc31c;
  param_1[0x22] = (param_1[0x22] - param_1[0x32]) * _DAT_005cc31c;
  param_1[0x23] = (param_1[0x23] - param_1[0x33]) * _DAT_005cc31c;
  param_1[0x24] = (param_1[0x24] - param_1[0x34]) * _DAT_005cc31c;
  param_1[0x25] = (param_1[0x25] - param_1[0x35]) * _DAT_005cc31c;
  param_1[0x26] = (param_1[0x26] - param_1[0x36]) * _DAT_005cc31c;
  param_1[0x27] = (param_1[0x27] - param_1[0x37]) * _DAT_005cc31c;
  param_1[0x28] = (param_1[0x28] - param_1[0x38]) * _DAT_005cc31c;
  param_1[0x29] = (param_1[0x29] - param_1[0x39]) * _DAT_005cc31c;
  param_1[0x2a] = (param_1[0x2a] - param_1[0x3a]) * _DAT_005cc31c;
  param_1[0x2b] = (param_1[0x2b] - param_1[0x3b]) * _DAT_005cc31c;
  param_1[0x2c] = (param_1[0x2c] - param_1[0x3c]) * _DAT_005cc31c;
  param_1[0x2d] = (param_1[0x2d] - param_1[0x3d]) * _DAT_005cc31c;
  param_1[0x2e] = (param_1[0x2e] - param_1[0x3e]) * _DAT_005cc31c;
  param_1[0x2f] = (param_1[0x2f] - param_1[0x3f]) * _DAT_005cc31c;
  *param_1 = (*param_1 - param_1[0x30]) - param_1[0x10];
  param_1[1] = (param_1[1] - param_1[0x31]) - param_1[0x11];
  param_1[2] = (param_1[2] - param_1[0x32]) - param_1[0x12];
  param_1[3] = (param_1[3] - param_1[0x33]) - param_1[0x13];
  param_1[4] = (param_1[4] - param_1[0x34]) - param_1[0x14];
  param_1[5] = (param_1[5] - param_1[0x35]) - param_1[0x15];
  param_1[6] = (param_1[6] - param_1[0x36]) - param_1[0x16];
  param_1[7] = (param_1[7] - param_1[0x37]) - param_1[0x17];
  param_1[8] = (param_1[8] - param_1[0x38]) - param_1[0x18];
  param_1[9] = (param_1[9] - param_1[0x39]) - param_1[0x19];
  param_1[10] = (param_1[10] - param_1[0x3a]) - param_1[0x1a];
  param_1[0xb] = (param_1[0xb] - param_1[0x3b]) - param_1[0x1b];
  param_1[0xc] = (param_1[0xc] - param_1[0x3c]) - param_1[0x1c];
  param_1[0xd] = (param_1[0xd] - param_1[0x3d]) - param_1[0x1d];
  param_1[0xe] = (param_1[0xe] - param_1[0x3e]) - param_1[0x1e];
  param_1[0xf] = (param_1[0xf] - param_1[0x3f]) - param_1[0x1f];
  param_1[0x10] = param_1[0x10] - param_1[0x20];
  param_1[0x11] = param_1[0x11] - param_1[0x21];
  param_1[0x12] = param_1[0x12] - param_1[0x22];
  param_1[0x13] = param_1[0x13] - param_1[0x23];
  param_1[0x14] = param_1[0x14] - param_1[0x24];
  param_1[0x15] = param_1[0x15] - param_1[0x25];
  param_1[0x16] = param_1[0x16] - param_1[0x26];
  param_1[0x17] = param_1[0x17] - param_1[0x27];
  param_1[0x18] = param_1[0x18] - param_1[0x28];
  param_1[0x19] = param_1[0x19] - param_1[0x29];
  param_1[0x1a] = param_1[0x1a] - param_1[0x2a];
  param_1[0x1b] = param_1[0x1b] - param_1[0x2b];
  param_1[0x1c] = param_1[0x1c] - param_1[0x2c];
  param_1[0x1d] = param_1[0x1d] - param_1[0x2d];
  param_1[0x1e] = param_1[0x1e] - param_1[0x2e];
  param_1[0x1f] = param_1[0x1f] - param_1[0x2f];
  return;
}
