// FLAGS: /O2 /GS /Ob1
/* MATCH-S02 0x0040ddb0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined;
typedef unsigned int undefined4;
MGLOBAL(undefined *, PTR_PTR_005f2770, 0x005f2770);
MGLOBAL(int, DAT_0063ba78, 0x0063ba78);
void FUN_004046a0(void);
void __fastcall FUN_00417750();
void FUN_00426c50(void);
void FUN_0045bf30(void);
void FUN_00475a60(void);
void thunk_FUN_00475ef0(void);
// MATCH: 0x0040ddb0 FUN_0040ddb0
void FUN_0040ddb0(void)



{

  FUN_00417750();

  FUN_00426c50(**(undefined4 **)(PTR_PTR_005f2770 + DAT_0063ba78 * 4));

  FUN_00475a60();

  FUN_0045bf30();

  thunk_FUN_00475ef0();

  FUN_004046a0();

  return;

}
