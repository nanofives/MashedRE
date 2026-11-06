// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x004540c0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs09_20261102/results.tsv. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
MGLOBAL(undefined4, DAT_006882f4, 0x006882f4);
MGLOBAL(undefined4, DAT_006882f0, 0x006882f0);
MGLOBAL(undefined4, DAT_00688058, 0x00688058);
MGLOBAL(undefined4, DAT_00688260, 0x00688260);
undefined4 FUN_004c0c20(int p0);
int FUN_004c1210(int p0);
undefined4 FUN_004e6920(int p0);
undefined4 FUN_004e6e00(int p0);
// MATCH: 0x004540c0 FUN_004540c0
void FUN_004540c0(void)

{
  undefined4 *puVar1;
  
  puVar1 = &DAT_00688260;
  do {
    FUN_004e6e00(*puVar1);
    puVar1 = puVar1 + 0xb;
  } while ((int)puVar1 < 0x688310);
  puVar1 = &DAT_00688058;
  do {
    FUN_004c1210(*puVar1);
    FUN_004c0c20(puVar1[-1]);
    FUN_004c0c20(*puVar1);
    FUN_004e6920(puVar1[-2]);
    puVar1 = puVar1 + 0x11;
  } while ((int)puVar1 < 0x688278);
  FUN_004e6e00(DAT_006882f4);
  FUN_004e6e00(DAT_006882f0);
  return;
}
