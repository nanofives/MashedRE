// FLAGS: /O2 /GS /Ob1
/* MATCH-S02 0x0040bd80: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
void FUN_00413f20(void);
void FUN_00421080(void);
void FUN_0045b350(void);
void FUN_00475a60(void);
void FUN_00477730(void);
void FUN_00486270(void);
void FUN_00486f90(void);
void FUN_00487df0(void);
void FUN_0048a5d0(void);
void FUN_0048ade0(void);
void FUN_0048bb70(void);
void FUN_0048f260(void);
void FUN_0048f680(void);
void FUN_0048f740(void);
void FUN_004904d0(void);
void thunk_FUN_00412190(void);
void thunk_FUN_00475ef0(void);
// MATCH: 0x0040bd80 FUN_0040bd80
void FUN_0040bd80(void)



{

  thunk_FUN_00412190();

  FUN_004904d0();

  FUN_0048f260();

  FUN_0048f680();

  FUN_0048f740();

  FUN_00475a60();

  thunk_FUN_00475ef0();

  FUN_00477730();

  FUN_0048ade0();

  FUN_0045b350();

  FUN_00487df0();

  FUN_00486f90();

  FUN_00486270();

  FUN_0048a5d0();

  FUN_0048bb70();

  FUN_00413f20();

  FUN_00421080();

  return;

}
