// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00424100: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(undefined4, DAT_007f1a18, 0x007f1a18);
MGLOBAL(int, DAT_007f1a14, 0x007f1a14);
MGLOBAL(undefined4, DAT_008999a0, 0x008999a0);
MGLOBAL(int, DAT_007f1a28, 0x007f1a28);
MGLOBAL(int, DAT_007f1a24, 0x007f1a24);
MGLOBAL(int, DAT_00899ad8, 0x00899ad8);
MGLOBAL(int, DAT_007f1a38, 0x007f1a38);
MGLOBAL(int, DAT_007f1a34, 0x007f1a34);
MGLOBAL(int, DAT_00899c10, 0x00899c10);
MGLOBAL(int, DAT_007f1a48, 0x007f1a48);
MGLOBAL(int, DAT_007f1a44, 0x007f1a44);
MGLOBAL(int, DAT_00899d48, 0x00899d48);
undefined4 FUN_0042f500(void);
// MATCH: 0x00424100 FUN_00424100
int FUN_00424100(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = FUN_0042f500();
  if (iVar1 != 0) {
    iVar1 = (&DAT_007f1a18)[param_1 * 4];
    iVar2 = (&DAT_008999a0)[param_1 * 0x4e];
    if (((DAT_007f1a18 == iVar1) && (param_1 != 0)) && (DAT_007f1a14 != -1)) {
      iVar2 = iVar2 + DAT_008999a0;
    }
    if (((DAT_007f1a28 == iVar1) && (param_1 != 1)) && (DAT_007f1a24 != -1)) {
      iVar2 = iVar2 + DAT_00899ad8;
    }
    if (((DAT_007f1a38 == iVar1) && (param_1 != 2)) && (DAT_007f1a34 != -1)) {
      iVar2 = iVar2 + DAT_00899c10;
    }
    if (((DAT_007f1a48 == iVar1) && (param_1 != 3)) && (DAT_007f1a44 != -1)) {
      iVar2 = iVar2 + DAT_00899d48;
    }
    return iVar2;
  }
  return (&DAT_008999a0)[param_1 * 0x4e];
}
