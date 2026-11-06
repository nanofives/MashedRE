// FLAGS: /O2 /GS /Ob1
// h11: 100% (regs 100%). Hand: the conditional third arg is two calls in an if/else (cross-jumped into one call), float compare as bool arg.
/* MATCH-S13 0x004826d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(float, DAT_005d757c, 0x005d757c);
int FUN_00481e00(int p0, float p1, int p2);
int FUN_00482140(int p0, int p1);
void FUN_00563810(int p0);
// MATCH: 0x004826d0 FUN_004826d0
int FUN_004826d0(int param_1, int param_2)
{
  int iVar1;
  int iVar2;

  if (param_2 != 0) {
    iVar1 = FUN_00481e00(param_1, 1.0f, param_1);
  } else {
    iVar1 = FUN_00481e00(param_1, 1.0f, 0);
  }
  iVar2 = FUN_00482140(iVar1, *(float *)(param_1 + 0x4c) > DAT_005d757c);
  FUN_00563810(iVar1);
  return iVar2;
}
