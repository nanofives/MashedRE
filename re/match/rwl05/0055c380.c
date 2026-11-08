// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055c380: hand-matched (h3). 85.2% (regs 96.3, ops 100). permuter swapped the [0xc]/[0x15] stores; residue is the allocation of the 0x3e4ccccd constant vs the [+0xc] read-modify-write. class (b). */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
// MATCH: 0x0055c380 FUN_0055c380
void FUN_0055c380(undefined4 *param_1,undefined4 param_2)

{
  param_1[0x13] = 0;
  *(undefined2 *)((int)param_1 + 0x5a) = 0;
  *(undefined2 *)(param_1 + 0x16) = 0;
  param_1[4] = 0;
  param_1[2] = 0;
  param_1[1] = 0;
  param_1[9] = 0;
  param_1[8] = 0;
  param_1[6] = 0;
  param_1[0xe] = 0;
  param_1[0xd] = 0;
  param_1[0x15] = 0x3e4ccccd;
  param_1[0xc] = 0;
  param_1[0x14] = 0x3e4ccccd;
  param_1[3] = param_1[3] | 0x20003;
  param_1[0x17] = param_2;
  param_1[10] = 0x3f800000;
  param_1[5] = 0x3f800000;
  *param_1 = 0x3f800000;
  return;
}
