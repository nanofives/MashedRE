// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x00530050: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(int, DAT_007dc540, 0x007dc540);
MGLOBAL(int, DAT_007dc544, 0x007dc544);
MGLOBAL(int, DAT_007dc508, 0x007dc508);
MGLOBAL(uint, DAT_007dc548, 0x007dc548);
// MATCH: 0x00530050 FUN_00530050
undefined4 FUN_00530050(int param_1)

{
  int iVar1;
  
  if ((((DAT_007dc540 != 0) && (DAT_007dc544 != 0)) &&
      (iVar1 = *(int *)(DAT_007dc508 + *(int *)(param_1 + 0x18)), iVar1 != 0)) &&
     (*(uint *)(iVar1 + 0x1c) <= DAT_007dc548)) {
    return 1;
  }
  return 0;
}
