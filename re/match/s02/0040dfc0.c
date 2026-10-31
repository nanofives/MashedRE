// FLAGS: /O2 /GS /Ob1
/* 0x0040dfc0: 100% (regs 100%), whole obj symbol byte-identical (424 B incl. the 4 jump/index tables).
   Raw g2s 58.0%. What moved it: (1) the DAT_0063ba8c == 7 branch written first (if > 6 { if != 7 return; ... })
   as in Ghidra's order, 35.7% -> 72.9%; (2) in that branch every case returns and FUN_0041d870 sits after the
   switch as the shared break target, with case 6/10 = `if (f500 == 0 || mode == 2) { ccc0; return; } break;`
   -> 98.8%; (3) default spelled `if (f500 != 0) break; ccc0(); return;` so the compiler does not cross-jump it
   into case 6/10's jne -> 100%. The 5/6 branch reaches the shared tail through goto done from the inner switch.
   No residual. */
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
MGLOBAL(int, DAT_0063ba8c, 0x0063ba8c);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);

void FUN_00403160(void);
void FUN_0041a3e0(void);
void FUN_0041b630(void);
void FUN_0041c0c0(void);
void FUN_0041c300(void);
void FUN_0041ccc0(void);
void FUN_0041d870(void);
void FUN_0041db80(void);
void FUN_0041ded0(int param_1);
void FUN_0041e850(void);
int FUN_00426ba0(void);
int FUN_0042f500(void);
int FUN_0042f6a0(void);

// MATCH: 0x0040dfc0 FUN_0040dfc0
void FUN_0040dfc0(void)
{
  if (FUN_00426ba0() == 0) {
    return;
  }
  if (DAT_0063ba8c < 5) {
    return;
  }
  if (DAT_0063ba8c > 6) {
    if (DAT_0063ba8c != 7) {
      return;
    }
    switch (FUN_0042f6a0()) {
    case 2:
      FUN_0041e850();
      return;
    case 6:
    case 10:
      if (FUN_0042f500() == 0 || DAT_007f0fd0 == 2) {
        FUN_0041ccc0();
        return;
      }
      break;
    case 3:
    case 4:
    case 5:
      switch (DAT_007f0fd0) {
      case 5:
        FUN_0041c300();
        return;
      case 8:
        FUN_0041ded0(0);
        return;
      case 4:
      case 7:
      case 9:
        FUN_0041ded0(1);
        return;
      case 10:
        FUN_0041a3e0();
        return;
      }
      /* fall through */
    default:
      if (FUN_0042f500() != 0) {
        break;
      }
      FUN_0041ccc0();
      return;
    }
    FUN_0041d870();
    return;
  }
  switch (FUN_0042f6a0()) {
  case 2:
    FUN_0041e850();
    break;
  case 6:
  case 10:
    if (FUN_0042f500() != 0 && DAT_007f0fd0 != 2) {
      FUN_0041c0c0();
    } else {
      FUN_0041b630();
    }
    break;
  case 3:
  case 4:
  case 5:
    switch (DAT_007f0fd0) {
    case 5:
      FUN_0041c300();
      goto done;
    case 4:
    case 7:
    case 9:
      FUN_0041ded0(1);
      goto done;
    case 8:
      FUN_0041ded0(0);
      goto done;
    case 10:
      FUN_0041a3e0();
      goto done;
    }
    /* fall through */
  default:
    if (FUN_0042f500() != 0) {
      FUN_0041c0c0();
    } else {
      FUN_0041b630();
    }
    break;
  case 11:
    FUN_00403160();
    break;
  }
done:
  if (FUN_0042f6a0() != 11) {
    FUN_0041db80();
  }
}
