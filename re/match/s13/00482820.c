// FLAGS: /O2 /GS /Ob1
// h11: 100% (regs 100%). Hand: callbacks declared as functions (Ghidra typed them as byte globals), callee arities from the pushes.
/* MATCH-S13 0x00482820: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs13_20261102/results.tsv. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef int (*CB)(int, int);
int FUN_004827a0(int p0, int p1);
int FUN_004827c0(int p0, int p1);
int FUN_00482800(int p0, int p1);
int FUN_0055af80(int p0, CB p1, int p2);
int FUN_0055afc0(int p0, CB p1, int p2);
int FUN_0055de20(int p0, CB p1, int p2);
int FUN_0055dec0(int p0);
// MATCH: 0x00482820 FUN_00482820
void FUN_00482820(int param_1)
{
  int iVar1;

  iVar1 = FUN_0055dec0(param_1);
  FUN_0055afc0(iVar1, FUN_004827a0, iVar1);
  FUN_0055af80(iVar1, FUN_004827c0, iVar1);
  FUN_0055de20(param_1, FUN_00482800, param_1);
}
