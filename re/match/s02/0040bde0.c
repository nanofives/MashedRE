// FLAGS: /O2 /GS /Ob1
/* MATCH-S02 0x0040bde0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(int, DAT_0063b8f4, 0x0063b8f4);
MGLOBAL(undefined1, DAT_0063b8f0, 0x0063b8f0);
void FUN_00412100(void);
void FUN_00413f50(void);
void FUN_00421560(void);
void FUN_00457610(void);
void FUN_00475d30(void);
void FUN_00475e50(void);
void FUN_00477810(void);
void FUN_00486220(void);
void FUN_00486f50(void);
void FUN_00487de0(void);
void FUN_0048ad50(void);
void FUN_0048baf0(void);
void FUN_0048e7b0(void);
void FUN_0048fce0(void);
void FUN_0048fd10(void);
void FUN_0048fd40(void);
void FUN_0048fd70(void);
void FUN_00490490(void);
MGLOBAL(int, _DAT_0063b8f0, 0x0063b8f0);
// MATCH: 0x0040bde0 FUN_0040bde0
/* [C2 2026-05-17] No args, void return. Single-guard fixed-sequence dispatcher: */



void FUN_0040bde0(void)



{

  if (DAT_0063b8f4 != 0) {

    FUN_0048ad50();

    FUN_00487de0();

    FUN_0048e7b0();

    FUN_00457610();

    FUN_00475d30();

    FUN_00486f50();

    FUN_00486220();

    FUN_0048fce0();

    FUN_0048fd40();

    FUN_0048fd10();

    FUN_00475e50();

    FUN_00477810();

    FUN_0048fd70();

    FUN_00490490();

    FUN_0048baf0();

    FUN_00413f50();

    FUN_00421560();

    FUN_00412100();

    _DAT_0063b8f0 = 0;

  }

  return;

}
