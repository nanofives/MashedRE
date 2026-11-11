// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x00538600: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
#ifndef __cplusplus
#define false 0
#endif
MGLOBAL(int, DAT_00912480, 0x00912480);
MGLOBAL(int, DAT_00912484, 0x00912484);
MGLOBAL(int, DAT_00912488, 0x00912488);
typedef int (*FNP)(void);
int FUN_005386b0(void);
int FUN_00538780(void);
int FUN_00538800(void);
int FUN_00538830(void);
int FUN_00538870(void);
int FUN_00538890(void);
int FUN_005388f0(void);
int FUN_00538960(void);
int FUN_00538980(void);
int FUN_005389c0(void);
int FUN_004e5cd0(int p0, int p1, FNP p2, FNP p3, int p4);
int FUN_004e5d00(int p0, FNP p1, FNP p2, FNP p3);
int FUN_004f0910(int p0, int p1, FNP p2, FNP p3, int p4);
int FUN_004f0940(int p0, FNP p1, FNP p2, FNP p3);
// MATCH: 0x00538600 FUN_00538600
int FUN_00538600(void)

{
  int iVar1;

  DAT_00912480 = 0;
  DAT_00912484 = FUN_004e5cd0(0x2c,0x12a,FUN_005386b0,FUN_00538780,0);
  if (DAT_00912484 < 0) {
    return 0;
  }
  iVar1 = FUN_004e5d00(0x12a,FUN_005389c0,FUN_00538980,FUN_00538960);
  if (iVar1 < 0) {
    return 0;
  }
  DAT_00912488 = FUN_004f0910(0x68,0x12a,FUN_00538800,FUN_00538830,0);
  if (DAT_00912488 < 0) {
    return 0;
  }
  iVar1 = FUN_004f0940(0x12a,FUN_005388f0,FUN_00538890,FUN_00538870);
  return iVar1 >= 0;
}
