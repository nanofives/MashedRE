// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x0041ffb0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs05_20261031/results.tsv. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
MGLOBAL(undefined4, DAT_0063e49c, 0x0063e49c);
MGLOBAL(undefined4, DAT_0063e4a0, 0x0063e4a0);
MGLOBAL(int, DAT_00636acc, 0x00636acc);
MGLOBAL(undefined4, DAT_0063e490, 0x0063e490);
MGLOBAL(undefined4, DAT_0063e494, 0x0063e494);
MGLOBAL(undefined4, DAT_0063d854, 0x0063d854);
MGLOBAL(undefined4, DAT_0063d858, 0x0063d858);
MGLOBAL(undefined4, DAT_0063d840, 0x0063d840);
void FUN_0041ec00(int p0);
void FUN_004768c0(int p0);
void FUN_004b4880(int p0);
undefined4 FUN_004c0c20(int p0);
// MATCH: 0x0041ffb0 FUN_0041ffb0
void FUN_0041ffb0(void)

{
  int iVar1;
  undefined4 *puVar2;
  
  iVar1 = 0;
  do {
    FUN_0041ec00(iVar1);
    iVar1 = iVar1 + 1;
  } while (iVar1 < 4);
  puVar2 = &DAT_0063d840;
  do {
    FUN_004c0c20(*puVar2);
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  } while ((int)puVar2 < 0x63d850);
  FUN_004768c0(DAT_0063e49c);
  DAT_0063e49c = 0;
  FUN_004768c0(DAT_0063e4a0);
  DAT_0063e4a0 = 0;
  if (DAT_00636acc == 0) {
    FUN_004b4880(DAT_0063e490);
    DAT_0063e490 = 0;
    DAT_0063d854 = 0;
    FUN_004b4880(DAT_0063e494);
    DAT_0063e494 = 0;
    DAT_0063d858 = 0;
  }
  return;
}
