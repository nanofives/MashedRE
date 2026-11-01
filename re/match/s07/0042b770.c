// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042b770: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs07_20261101/results.tsv. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
MGLOBAL(int, DAT_007f1a0c, 0x007f1a0c);
MGLOBAL(undefined1, DAT_007f1049, 0x007f1049);
MGLOBAL(undefined1, DAT_007f1509, 0x007f1509);
MGLOBAL(char, DAT_007f1095, 0x007f1095);
MGLOBAL(char, DAT_007f1555, 0x007f1555);
MGLOBAL(char, DAT_007f10e1, 0x007f10e1);
MGLOBAL(char, DAT_007f15a1, 0x007f15a1);
MGLOBAL(char, DAT_007f112d, 0x007f112d);
MGLOBAL(char, DAT_007f15ed, 0x007f15ed);
MGLOBAL(char, DAT_007f1179, 0x007f1179);
MGLOBAL(char, DAT_007f1639, 0x007f1639);
MGLOBAL(char, DAT_007f11c5, 0x007f11c5);
MGLOBAL(char, DAT_007f1685, 0x007f1685);
MGLOBAL(char, DAT_007f1211, 0x007f1211);
MGLOBAL(char, DAT_007f16d1, 0x007f16d1);
MGLOBAL(char, DAT_007f125d, 0x007f125d);
MGLOBAL(char, DAT_007f171d, 0x007f171d);
MGLOBAL(undefined4, DAT_007f1a14, 0x007f1a14);
undefined4 FUN_0040e470(int p0);
// MATCH: 0x0042b770 FUN_0042b770
undefined4 FUN_0042b770(void)

{
  int iVar1;
  undefined4 uVar2;
  int *piVar3;
  int iVar4;
  
  uVar2 = 0;
  if (DAT_007f1a0c == 0x1000) {
    if ((DAT_007f1049 != '\0') && (DAT_007f1509 == '\0')) {
      uVar2 = 1;
    }
    if ((DAT_007f1095 != '\0') && (DAT_007f1555 == '\0')) {
      uVar2 = 1;
    }
    if ((DAT_007f10e1 != '\0') && (DAT_007f15a1 == '\0')) {
      uVar2 = 1;
    }
    if ((DAT_007f112d != '\0') && (DAT_007f15ed == '\0')) {
      uVar2 = 1;
    }
    if ((DAT_007f1179 != '\0') && (DAT_007f1639 == '\0')) {
      uVar2 = 1;
    }
    if ((DAT_007f11c5 != '\0') && (DAT_007f1685 == '\0')) {
      uVar2 = 1;
    }
    if ((DAT_007f1211 != '\0') && (DAT_007f16d1 == '\0')) {
      uVar2 = 1;
    }
    if ((DAT_007f125d != '\0') && (DAT_007f171d == '\0')) {
      return 1;
    }
  }
  else {
    iVar4 = 0;
    piVar3 = (int *)&DAT_007f1a14;
    do {
      if ((((*piVar3 != -1) && (iVar1 = FUN_0040e470(iVar4), iVar1 != 2)) &&
          ((&DAT_007f1049)[*piVar3 * 0x4c] != '\0')) && ((&DAT_007f1509)[*piVar3 * 0x4c] == '\0')) {
        uVar2 = 1;
      }
      piVar3 = piVar3 + 4;
      iVar4 = iVar4 + 1;
    } while ((int)piVar3 < 0x7f1a54);
  }
  return uVar2;
}
