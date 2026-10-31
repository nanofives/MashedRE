// FLAGS: /O2 /GS /Ob1
/* MATCH-S02 0x0040df20: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
MGLOBAL(undefined4, DAT_0063b9a8, 0x0063b9a8);
MGLOBAL(undefined4, DAT_0063b928, 0x0063b928);
MGLOBAL(undefined1, DAT_0063b9ac, 0x0063b9ac);
void FUN_004266b0(undefined4 p0);
int FUN_004c1480(int p0, undefined4 p1, undefined4 p2);
int FUN_004c1c10(int p0, int p1);
int FUN_004c1c80(int p0, undefined4 * p1);
// MATCH: 0x0040df20 FUN_0040df20
void FUN_0040df20(int param_1)



{

  undefined4 uVar1;

  

  FUN_004266b0(param_1);

  uVar1 = *(undefined4 *)(param_1 + 4);

  FUN_004c1c10(param_1,DAT_0063b9a8);

  FUN_004c1c80(param_1,&DAT_0063b9ac);

  FUN_004c1480(uVar1,&DAT_0063b928,0);

  return;

}
