// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x0056cf90: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl06_20261108/results.tsv. */
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
typedef unsigned int undefined4;
// MATCH: 0x0056cf90 FUN_0056cf90
void FUN_0056cf90(undefined4 *param_1,int param_2,int param_3)

{
  int iVar1;
  
  iVar1 = param_3 * 0x20 + param_2;
  *param_1 = *(undefined4 *)(param_3 * 0x20 + param_2);
  param_1[1] = *(undefined4 *)((param_3 + 2) * 0x20 + param_2);
  param_1[2] = *(undefined4 *)((param_3 + 4) * 0x20 + param_2);
  param_1[3] = *(undefined4 *)((param_3 + 6) * 0x20 + param_2);
  param_1[4] = *(undefined4 *)(iVar1 + 4);
  param_1[5] = *(undefined4 *)(iVar1 + 0x44);
  param_1[6] = *(undefined4 *)(iVar1 + 0x84);
  param_1[7] = *(undefined4 *)(iVar1 + 0xc4);
  param_1[8] = *(undefined4 *)(iVar1 + 8);
  param_1[9] = *(undefined4 *)(iVar1 + 0x48);
  param_1[10] = *(undefined4 *)(iVar1 + 0x88);
  param_1[0xb] = *(undefined4 *)(iVar1 + 200);
  param_1[0xc] = *(undefined4 *)(iVar1 + 0x10);
  param_1[0xd] = *(undefined4 *)(iVar1 + 0x50);
  param_1[0xe] = *(undefined4 *)(iVar1 + 0x90);
  param_1[0xf] = *(undefined4 *)(iVar1 + 0xd0);
  param_1[0x10] = *(undefined4 *)(iVar1 + 0x14);
  param_1[0x11] = *(undefined4 *)(iVar1 + 0x54);
  param_1[0x12] = *(undefined4 *)(iVar1 + 0x94);
  param_1[0x13] = *(undefined4 *)(iVar1 + 0xd4);
  param_1[0x14] = *(undefined4 *)(iVar1 + 0x18);
  param_1[0x15] = *(undefined4 *)(iVar1 + 0x58);
  param_1[0x16] = *(undefined4 *)(iVar1 + 0x98);
  param_1[0x17] = *(undefined4 *)(iVar1 + 0xd8);
  return;
}
