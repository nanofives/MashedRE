// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00425bc0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(undefined4, DAT_00644150, 0x00644150);
MGLOBAL(undefined4, DAT_00899260, 0x00899260);
undefined4 FUN_004b3d80(undefined4 p0, undefined4 p1, undefined4 p2);
void FUN_004c5c80(undefined4 p0);
// MATCH: 0x00425bc0 FUN_00425bc0
void FUN_00425bc0(void)

{
  int iVar1;
  undefined4 *puVar2;
  
  DAT_00644150 = FUN_004b3d80("ToastArt/common/perm/Copters/Copters.txd",0,0);
  FUN_004c5c80(0);
  puVar2 = &DAT_00899260;
  for (iVar1 = 0x98; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  }
  return;
}
