// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d53b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs21_20261107/results.tsv. */
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
typedef int code();
MGLOBAL(uint, DAT_007d6c14, 0x007d6c14);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(uint, DAT_007d6c18, 0x007d6c18);
MGLOBAL(undefined4, DAT_007d4720, 0x007d4720);
MGLOBAL(undefined4, DAT_007d4724, 0x007d4724);
MGLOBAL(undefined4, DAT_007d5168, 0x007d5168);
MGLOBAL(undefined4, DAT_007d54b0, 0x007d54b0);
MGLOBAL(undefined4, DAT_007d57f8, 0x007d57f8);
MGLOBAL(undefined4, DAT_007d57fc, 0x007d57fc);
MGLOBAL(undefined4, DAT_007d5e88, 0x007d5e88);
MGLOBAL(undefined4, DAT_007d62a8, 0x007d62a8);
MGLOBAL(undefined4, DAT_007d62ac, 0x007d62ac);
// MATCH: 0x004d53b0 FUN_004d53b0
void FUN_004d53b0(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  uint uVar7;
  
  if (DAT_007d6c14 != 0) {
    uVar7 = 0;
    do {
      iVar1 = (&DAT_007d5168)[uVar7];
      iVar2 = (&DAT_007d57f8)[iVar1 * 2];
      iVar3 = (&DAT_007d54b0)[iVar1];
      (&DAT_007d57fc)[iVar1 * 2] = 0;
      piVar6 = DAT_007d4110;
      if (iVar3 != iVar2) {
        (&DAT_007d54b0)[iVar1] = iVar2;
        (**(code **)(*piVar6 + 0xe4))(piVar6,iVar1,iVar2);
      }
      uVar7 = uVar7 + 1;
    } while (uVar7 < DAT_007d6c14);
    DAT_007d6c14 = 0;
  }
  if (DAT_007d6c18 != 0) {
    uVar7 = 0;
    do {
      iVar2 = (&DAT_007d62a8)[uVar7 * 2];
      iVar3 = (&DAT_007d62ac)[uVar7 * 2];
      iVar1 = iVar2 + iVar3 + iVar2 * 0x20;
      iVar4 = (&DAT_007d4720)[iVar1 * 2];
      iVar5 = (&DAT_007d5e88)[iVar1];
      (&DAT_007d4724)[iVar1 * 2] = 0;
      if (iVar5 != iVar4) {
        (&DAT_007d5e88)[iVar1] = iVar4;
        (**(code **)(*DAT_007d4110 + 0x10c))(DAT_007d4110,iVar2,iVar3,iVar4);
      }
      uVar7 = uVar7 + 1;
    } while (uVar7 < DAT_007d6c18);
    DAT_007d6c18 = 0;
  }
  return;
}
