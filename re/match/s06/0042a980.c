// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x0042a980: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(int, DAT_005f6748, 0x005f6748);
MGLOBAL(undefined4, DAT_005f6754, 0x005f6754);
undefined4 FUN_0040ce80(int p0);
// MATCH: 0x0042a980 FUN_0042a980
undefined4 FUN_0042a980(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = FUN_0040ce80(param_1);
  iVar3 = 0;
  iVar1 = DAT_005f6748;
  while( true ) {
    if (iVar1 == -1) {
      return 0;
    }
    if (iVar1 == iVar2) break;
    iVar1 = (&DAT_005f6754)[iVar3];
    iVar3 = iVar3 + 3;
  }
  return *(undefined4 *)(iVar3 * 4 + 0x5f674c);
}
