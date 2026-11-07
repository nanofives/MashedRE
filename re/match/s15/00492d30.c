// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00492d30: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs15_20261102/results.tsv.
   MATCH-FIX-492D30: prototypes of FUN_004111c0 (uint) and FUN_00498860 (undefined4) take the one argument every
   call passes (g2s had (void), which C++ rejects); match.py re-run = 100% (verify/matchfix492d30_20261106). */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
MGLOBAL(undefined4, DAT_00771968, 0x00771968);
MGLOBAL(int, DAT_007719cc, 0x007719cc);
MGLOBAL(undefined1, DAT_007719a8, 0x007719a8);
MGLOBAL(int, DAT_007f1000, 0x007f1000);
void FUN_004030d0(void);
void FUN_004111c0(uint p0);
undefined4 FUN_0042b930(void);
void FUN_0043d7c0(void);
void FUN_0043dfd0(void);
void FUN_0045b350(void);
undefined4 FUN_004671a0(int p0);
undefined4 FUN_004950b0(void);
undefined4 FUN_00496920(int p0);
void FUN_00498860(undefined4 p0);
undefined4 FUN_005c9d00(void);
MGLOBAL(int, _DAT_007719a8, 0x007719a8);
// MATCH: 0x00492d30 FUN_00492d30
/* [C2 2026-05-12] 7-case state machine tick. Dispatches on `DAT_00771968`. */

undefined4 FUN_00492d30(void)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar1 = FUN_004950b0();
  FUN_0045b350();
  FUN_005c9d00();
  switch(DAT_00771968) {
  case 1:
    iVar2 = FUN_0042b930();
    if (iVar2 == 0x21) {
      FUN_004030d0();
    }
    break;
  case 2:
    goto switchD_00492d52_caseD_2;
  case 3:
    if (0 < DAT_007f1000) {
      iVar2 = (DAT_007f1000 - 1U) / 0x32 + 1;
      do {
        FUN_004111c0(0x32);
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
    }
    goto switchD_00492d52_caseD_2;
  case 4:
    FUN_0043dfd0();
    iVar2 = FUN_0042b930();
    if (iVar2 == 0x20) {
      FUN_0045b350();
    }
    goto switchD_00492d52_caseD_7;
  case 5:
    break;
  case 6:
    if (0 < DAT_007f1000) {
      iVar2 = (DAT_007f1000 - 1U) / 0x32 + 1;
      do {
        FUN_004111c0(0x32);
        iVar2 = iVar2 + -1;
      } while (iVar2 != 0);
    }
    break;
  case 7:
    goto switchD_00492d52_caseD_7;
  default:
    goto switchD_00492d52_default;
  }
  FUN_0043dfd0();
switchD_00492d52_caseD_7:
  FUN_0043d7c0();
switchD_00492d52_caseD_2:
switchD_00492d52_default:
  iVar2 = FUN_00496920(5);
  if ((iVar2 != 0) && (DAT_007719cc == 0)) {
    uVar3 = FUN_004671a0(0);
    FUN_00498860(uVar3);
    DAT_007719cc = 1;
    iVar2 = FUN_004950b0();
    _DAT_007719a8 = iVar2 - iVar1;
    return 1;
  }
  DAT_007719cc = 0;
  iVar2 = FUN_004950b0();
  _DAT_007719a8 = iVar2 - iVar1;
  return 1;
}
