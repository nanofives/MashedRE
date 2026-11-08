// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x005515a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
MGLOBAL(int, DAT_009124f8, 0x009124f8);
int FUN_00551610(void);
int FUN_00551680(void);
int FUN_005516c0(void);
int FUN_005516e0(void);
int FUN_00551770(void);
int FUN_005518f0(void);
int FUN_00551bf0(void);
int FUN_00551840(void);
typedef int (*PFV)(void);
int FUN_004c2d90(int p0, int p1, PFV p2, PFV p3);
int FUN_004e80c0(int p0, int p1, PFV p2, PFV p3, PFV p4);
int FUN_004e80f0(int p0, PFV p1, PFV p2, PFV p3);
// MATCH: 0x005515a0 FUN_005515a0

int FUN_005515a0(void)
{
  int iVar1;
  iVar1 = FUN_004c2d90(0,0x135,FUN_00551610,FUN_00551680);
  if (iVar1 >= 0) {
    DAT_009124f8 = FUN_004e80c0(0x28,0x135,FUN_005516c0,FUN_005516e0,FUN_00551770);
    iVar1 = FUN_004e80f0(0x135,FUN_005518f0,FUN_00551840,FUN_00551bf0);
    if ((iVar1 >= 0) && (DAT_009124f8 >= 0)) {
      return 1;
    }
  }
  return 0;
}
