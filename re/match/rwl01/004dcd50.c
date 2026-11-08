// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004dcd50: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
MGLOBAL(undefined4, DAT_007d70b4, 0x007d70b4);
MGLOBAL(undefined4, DAT_007d70b8, 0x007d70b8);
MGLOBAL(undefined4, DAT_007d70bc, 0x007d70bc);
MGLOBAL(undefined4, DAT_007d70c4, 0x007d70c4);
undefined4 FUN_004cc7f0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
undefined4 FUN_004dc9e0(void);
void FUN_004dcdc0(void);
// MATCH: 0x004dcd50 FUN_004dcd50
undefined4 FUN_004dcd50(void)

{
  FUN_004dcdc0();
  DAT_007d70b4 = FUN_004cc7f0(0x10,0x10,4,0x40411);
  DAT_007d70b8 = FUN_004cc7f0(0x14,100,4,0x40411);
  DAT_007d70bc = FUN_004cc7f0(8,100,4,0x40411);
  DAT_007d70c4 = FUN_004cc7f0(0x14,0x2a,4,0x40411);
  FUN_004dc9e0();
  return 1;
}
