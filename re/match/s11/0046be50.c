// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046be50: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs11_20261102/results.tsv. */
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
typedef unsigned int uint;
MGLOBAL(undefined4, DAT_008815c0, 0x008815c0);
MGLOBAL(undefined4, DAT_008815c4, 0x008815c4);
MGLOBAL(undefined4, DAT_008815c8, 0x008815c8);
MGLOBAL(undefined4, DAT_008815cc, 0x008815cc);
MGLOBAL(undefined4, DAT_008815d0, 0x008815d0);
MGLOBAL(undefined4, DAT_008815d4, 0x008815d4);
MGLOBAL(undefined4, DAT_008815d8, 0x008815d8);
MGLOBAL(undefined4, DAT_008815dc, 0x008815dc);
MGLOBAL(undefined4, DAT_008815e0, 0x008815e0);
MGLOBAL(undefined4, DAT_008815e4, 0x008815e4);
MGLOBAL(undefined4, DAT_008815e8, 0x008815e8);
MGLOBAL(undefined4, DAT_008815ec, 0x008815ec);
// MATCH: 0x0046be50 FUN_0046be50
undefined4 FUN_0046be50(uint param_1,undefined4 param_2,uint param_3,undefined4 param_4)

{
  if (param_1 < 0x10) {
    if (100 < param_3) {
      param_3 = 100;
    }
    switch(param_2) {
    case 1:
      (&DAT_008815c0)[param_1 * 0x341] = param_3;
      (&DAT_008815d8)[param_1 * 0x341] = param_4;
      return 1;
    case 2:
      (&DAT_008815c4)[param_1 * 0x341] = param_3;
      (&DAT_008815dc)[param_1 * 0x341] = param_4;
      return 1;
    case 3:
      (&DAT_008815c8)[param_1 * 0x341] = param_3;
      (&DAT_008815e0)[param_1 * 0x341] = param_4;
      return 1;
    case 4:
      (&DAT_008815cc)[param_1 * 0x341] = param_3;
      (&DAT_008815e4)[param_1 * 0x341] = param_4;
      return 1;
    case 5:
      (&DAT_008815d0)[param_1 * 0x341] = param_3;
      (&DAT_008815e8)[param_1 * 0x341] = param_4;
      return 1;
    case 6:
      (&DAT_008815d4)[param_1 * 0x341] = param_3;
      (&DAT_008815ec)[param_1 * 0x341] = param_4;
      return 1;
    }
  }
  return 0;
}
