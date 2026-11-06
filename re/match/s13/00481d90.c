// FLAGS: /O2 /GS /Ob1
// h11: 100% (regs 100%). Hand: guard as early returns, FUN_0055b650 takes 1 arg (tail jmp), globals array 200.
/* MATCH-S13 0x00481d90: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBALA(int, DAT_006c71d8, 200, 0x006c71d8);
void FUN_0047d080(int p0, int p1);
void FUN_0055ac00(int p0, int p1, int p2);
int FUN_0055ac50(int p0, int p1, int p2);
int FUN_0055ad30(int p0, int p1);
void FUN_0055b650(int p0);
int FUN_0057c210(int p0);
// MATCH: 0x00481d90 FUN_00481d90
void FUN_00481d90(int param_1)
{
  int iVar1;
  int iVar2;

  if (param_1 < 0) return;
  if (param_1 >= 200) return;
  iVar1 = DAT_006c71d8[param_1];
  if (iVar1 == 0) return;
  FUN_0047d080(param_1, 1);
  iVar1 = FUN_0057c210(iVar1);
  if (iVar1 == 0) return;
  FUN_0055ac50(*(int *)(iVar1 + 0x24), iVar1, 1);
  FUN_0055ac00(*(int *)(iVar1 + 0x24), iVar1, 1);
  FUN_0055ad30(*(int *)(iVar1 + 0x24), iVar1);
  iVar2 = *(int *)(*(int *)(iVar1 + 0x10) + 4);
  if (iVar2 != 0) {
    FUN_0055b650(iVar2);
  }
}
