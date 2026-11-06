// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045c720: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs10_20261102/results.tsv. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
MGLOBAL(undefined1, DAT_005fd190, 0x005fd190);
MGLOBAL(undefined1, DAT_005fe268, 0x005fe268);
MGLOBAL(undefined1, DAT_005ff340, 0x005ff340);
MGLOBAL(undefined1, DAT_00600418, 0x00600418);
MGLOBAL(undefined1, DAT_006014f0, 0x006014f0);
MGLOBAL(undefined1, DAT_006025c8, 0x006025c8);
MGLOBAL(undefined4, DAT_007f1a1c, 0x007f1a1c);
MGLOBAL(undefined4, DAT_0088f680, 0x0088f680);
// MATCH: 0x0045c720 FUN_0045c720
void FUN_0045c720(int param_1,int param_2)

{
  switch((&DAT_007f1a1c)[param_1 * 4]) {
  case 0:
    (&DAT_0088f680)[param_1] = &DAT_005fd190 + param_2 * 0x2c;
    return;
  case 1:
    (&DAT_0088f680)[param_1] = &DAT_005fe268 + param_2 * 0x2c;
    return;
  case 2:
    (&DAT_0088f680)[param_1] = &DAT_005ff340 + param_2 * 0x2c;
    return;
  case 3:
    (&DAT_0088f680)[param_1] = &DAT_00600418 + param_2 * 0x2c;
    return;
  case 4:
    (&DAT_0088f680)[param_1] = &DAT_006014f0 + param_2 * 0x2c;
    return;
  case 5:
    (&DAT_0088f680)[param_1] = &DAT_006025c8 + param_2 * 0x2c;
    return;
  default:
    (&DAT_0088f680)[param_1] = 0;
    return;
  }
}
