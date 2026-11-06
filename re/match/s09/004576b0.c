// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x004576b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
MGLOBAL(undefined4, DAT_0068a254, 0x0068a254);
MGLOBAL(undefined1, DAT_0068a300, 0x0068a300);
// MATCH: 0x004576b0 FUN_004576b0
undefined4 FUN_004576b0(void)

{
  int iVar1;
  int *piVar2;
  
  iVar1 = 0;
  do {
    if (*(int *)(&DAT_0068a300 + iVar1 * 4) != 0) {
      return 1;
    }
    iVar1 = iVar1 + 1;
  } while (iVar1 < 2);
  if (iVar1 < 4) {
    piVar2 = (int *)&DAT_0068a254 + iVar1 * 4;
    do {
      if (*piVar2 != 0) {
        return 1;
      }
      piVar2 = piVar2 + 4;
    } while ((int)piVar2 < 0x68a294);
  }
  return 0;
}
