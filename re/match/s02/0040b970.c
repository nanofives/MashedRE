// FLAGS: /O2 /GS /Ob1
/* MATCH-S02 0x0040b970: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
MGLOBAL(undefined4, DAT_008a94e0, 0x008a94e0);
// MATCH: 0x0040b970 FUN_0040b970
void FUN_0040b970(undefined4 *param_1)



{

  int iVar1;

  int iVar2;

  

  iVar1 = (int)&DAT_008a94e0 - (int)param_1;

  iVar2 = 4;

  do {

    *param_1 = 0;

    if (*(int *)(iVar1 + (int)param_1) == 0) {

      *param_1 = 1;

    }

    param_1 = param_1 + 1;

    iVar2 = iVar2 + -1;

  } while (iVar2 != 0);

  return;

}
