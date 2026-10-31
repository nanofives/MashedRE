// FLAGS: /O2 /GS /Ob1
/* MATCH-S02 0x0040b7b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs02_20261031/results.tsv. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
MGLOBAL(undefined4, DAT_008a9530, 0x008a9530);
MGLOBAL(undefined1, DAT_008a9540, 0x008a9540);
MGLOBAL(undefined1, DAT_008a9550, 0x008a9550);
MGLOBAL(undefined1, DAT_008a9560, 0x008a9560);
// MATCH: 0x0040b7b0 FUN_0040b7b0
undefined4 FUN_0040b7b0(int param_1,undefined4 param_2)



{

  switch(param_2) {

  case 0:

    return (&DAT_008a9530)[param_1];

  case 1:

    return *(undefined4 *)(&DAT_008a9560 + param_1 * 4);

  case 2:

    return *(undefined4 *)(&DAT_008a9540 + param_1 * 4);

  case 3:

    return *(undefined4 *)(&DAT_008a9550 + param_1 * 4);

  default:

    return 0;

  }

}
