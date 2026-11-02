// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0043bf30: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs08_20261101/results.tsv. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0067e7a8, 0x0067e7a8);
MGLOBAL(int, DAT_0067e7d8, 0x0067e7d8);
MGLOBAL(int, DAT_0067e7b0, 0x0067e7b0);
MGLOBAL(int, DAT_0067e7f0, 0x0067e7f0);
MGLOBAL(int, DAT_0067e7c8, 0x0067e7c8);
MGLOBAL(int, DAT_0067e7f8, 0x0067e7f8);
MGLOBAL(int, DAT_0067e838, 0x0067e838);
MGLOBAL(int, DAT_0067e830, 0x0067e830);
MGLOBAL(int, DAT_0067e820, 0x0067e820);
MGLOBAL(int, DAT_0067e7e0, 0x0067e7e0);
MGLOBAL(int, DAT_0067e7e8, 0x0067e7e8);
MGLOBAL(int, DAT_0067e810, 0x0067e810);
MGLOBAL(int, DAT_0067e818, 0x0067e818);
MGLOBAL(int, DAT_0067e7b8, 0x0067e7b8);
void FUN_0042f0c0(void);
void FUN_0042fb70(void);
void FUN_0042fe90(void);
void FUN_00430120(void);
void FUN_00430b90(void);
void FUN_00431240(void);
void FUN_004314b0(void);
void FUN_00431710(void);
void FUN_004335f0(void);
void FUN_00434720(void);
void FUN_00439210(void);
void FUN_0043a610(void);
void FUN_0043aa30(void);
void FUN_0043af10(void);
// MATCH: 0x0043bf30 FUN_0043bf30
void FUN_0043bf30(void)

{
  if (DAT_0067e7a8 != 0) {
    FUN_004335f0();
  }
  if (DAT_0067e7d8 != 0) {
    FUN_0043a610();
  }
  if (DAT_0067e7b0 != 0) {
    FUN_0042f0c0();
  }
  if (DAT_0067e7f0 != 0) {
    FUN_0043af10();
  }
  if (DAT_0067e7c8 != 0) {
    FUN_00434720();
  }
  if (DAT_0067e7f8 != 0) {
    FUN_00430b90();
  }
  if (DAT_0067e838 != 0) {
    FUN_00431240();
  }
  if (DAT_0067e830 != 0) {
    FUN_004314b0();
  }
  if (DAT_0067e820 != 0) {
    FUN_00431710();
  }
  if (DAT_0067e7e0 != 0) {
    FUN_0043aa30();
  }
  if (DAT_0067e7e8 != 0) {
    FUN_0042fb70();
  }
  if (DAT_0067e810 != 0) {
    FUN_0042fe90();
  }
  if (DAT_0067e818 != 0) {
    FUN_00430120();
  }
  if (DAT_0067e7b8 != 0) {
    FUN_00439210();
    return;
  }
  return;
}
