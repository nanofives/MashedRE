// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00433240: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
MGLOBAL(int, DAT_0067eca4, 0x0067eca4);
MGLOBAL(undefined4, DAT_0067ecb0, 0x0067ecb0);
MGLOBAL(undefined4, DAT_0067e844, 0x0067e844);
MGLOBAL(undefined4, DAT_0067e9f8, 0x0067e9f8);
MGLOBAL(undefined4, DAT_0067e914, 0x0067e914);
MGLOBAL(undefined4, DAT_0067ecac, 0x0067ecac);
MGLOBAL(undefined4, DAT_0067ea84, 0x0067ea84);
void FUN_00402f80(void);
void FUN_0042d3a0(void);
// MATCH: 0x00433240 FUN_00433240
void FUN_00433240(undefined4 param_1)

{
  if (DAT_0067eca4 == 0) {
    DAT_0067eca4 = 1;
    DAT_0067ecb0 = 0x21;
    FUN_00402f80();
    FUN_0042d3a0();
    DAT_0067e844 = 0;
    DAT_0067e9f8 = 0;
    DAT_0067e914 = 0;
    DAT_0067ecac = param_1;
    DAT_0067ea84 = 1;
  }
  return;
}
