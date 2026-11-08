// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00549aa0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(int, DAT_007dc728, 0x007dc728);
int FUN_004e8c70(int p0, int p1, uint p2);
void FUN_005495a0(int p0);
// MATCH: 0x00549aa0 FUN_00549aa0
int FUN_00549aa0(int param_1)
{
  uint uVar1;
  int iVar2;
  int a;
  uVar1 = *(uint *)(param_1 + 0x2c);
  a = *(int *)(param_1 + 0x30);
  iVar2 = FUN_004e8c70(a,0,
                       (uVar1 >> 5 & 1) * 64 | (uVar1 >> 4 & 1) * 32 | (uVar1 >> 1 & 1) * 16 |
                       (uVar1 >> 2 & 1) * 8 | (uVar1 >> 3 & 1) * 4 | (uVar1 & 1) * 2 |
                       uVar1 & 0xff0000 | 1);
  *(int *)(DAT_007dc728 + iVar2) = param_1;
  FUN_005495a0(param_1);
  return iVar2;
}
