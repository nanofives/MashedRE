// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00427620: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs06_20261101/results.tsv. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
MGLOBAL(undefined4, DAT_0067d838, 0x0067d838);
MGLOBAL(undefined4, DAT_0067d844, 0x0067d844);
MGLOBAL(undefined4, DAT_0067d848, 0x0067d848);
MGLOBAL(undefined4, DAT_0067d83c, 0x0067d83c);
MGLOBAL(undefined4, DAT_0067d840, 0x0067d840);
undefined4 FUN_004c5930(int p0);
void FUN_00552b90(void);
undefined4 FUN_00555830(int p0);
undefined4 FUN_00556cd0(void);
undefined4 FUN_00556e40(int p0);
// MATCH: 0x00427620 FUN_00427620

void FUN_00427620(void)

{
  int iVar1;
  
  FUN_00555830(DAT_0067d838);
  DAT_0067d838 = 0;
  FUN_00556e40(DAT_0067d844);
  FUN_00556e40(DAT_0067d848);
  FUN_00556e40(DAT_0067d83c);
  FUN_00556e40(DAT_0067d840);
  iVar1 = FUN_00556cd0();
  if (iVar1 != 0) {
    FUN_004c5930(iVar1);
  }
  FUN_00552b90();
  return;
}
