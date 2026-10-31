// FLAGS: /O2 /GS /Ob1
/* MATCH-S02 0x0040ce00: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
void FUN_0040c5b0(void);
void FUN_0040ca00(void);
void FUN_00412620(void);
void FUN_00413cb0(void);
void FUN_004212b0(void);
void FUN_00475c10(void);
void FUN_00476440(void);
void FUN_004777d0(void);
void FUN_00481a30(void);
void FUN_00485ef0(void);
void FUN_00486830(void);
void FUN_00487df0(void);
void FUN_0048a780(void);
void FUN_0048ab70(void);
void FUN_0048b960(void);
void FUN_0048e5f0(void);
void FUN_0048f8a0(void);
void FUN_0048f9f0(void);
void FUN_0048fb10(void);
void FUN_00490380(void);
void FUN_00490500(void);
int thunk_FUN_00425ef0(void);
MGLOBAL(int, _DAT_0063b8f0, 0x0063b8f0);
// MATCH: 0x0040ce00 FUN_0040ce00
/* [C1 2026-06-01] `void FUN_0040ce00(void)` — gated per-frame dispatcher. If `DAT_0063b8f4 == 0`,

   returns immediately.… */



void FUN_0040ce00(void)



{

  if (DAT_0063b8f4 != 0) {

    _DAT_0063b8f0 = _DAT_0063b8f0 + 1;

    FUN_00487df0();

    FUN_0040c5b0();

    FUN_0048ab70();

    FUN_0048e5f0();

    FUN_0040ca00();

    thunk_FUN_00425ef0();

    FUN_00485ef0();

    FUN_00486830();

    FUN_0048f8a0();

    FUN_0048fb10();

    FUN_0048f9f0();

    FUN_00490500();

    FUN_00490380();

    FUN_00475c10();

    FUN_00476440();

    FUN_004777d0();

    FUN_0048b960();

    FUN_00413cb0();

    FUN_004212b0();

    FUN_0048a780();

    FUN_00481a30();

    FUN_00412620();

    return;

  }

  return;

}
