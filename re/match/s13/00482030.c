// FLAGS: /O2 /GS /Ob1
// h11: 100% (regs 100%). Hand: unsigned params (fild+2^32 fixup), quotient reuses param_2's slot, double temp keeps f on the x87 stack across the switch, case 2/3 store the K-f result first.
/* MATCH-S13 0x00482030: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(float, DAT_005cc35c, 0x005cc35c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
// MATCH: 0x00482030 FUN_00482030
void FUN_00482030(float *param_1, unsigned int param_2, unsigned int param_3)
{
  double f;

  f = (float)param_2 * DAT_005cc35c / (float)param_3;
  param_2 = (param_2 * 4) / param_3;
  f = f - (float)param_2;
  switch (param_2) {
  case 0:
    param_1[0] = f;
    param_1[1] = 0.0f;
    return;
  case 1:
    param_1[1] = f;
    param_1[0] = 1.0f;
    return;
  case 2:
    param_1[0] = DAT_005cc320 - f;
    param_1[1] = 1.0f;
    return;
  case 3:
    param_1[1] = DAT_005cc320 - f;
    param_1[0] = 0.0f;
    return;
  }
}
