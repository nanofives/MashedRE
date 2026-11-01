// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 h4 0x0042ee40: 100% (regs 100%), whole 244 B symbol incl. the jump table BYTE-IDENTICAL. Raw converter
   dropped the string arguments (52.2%). By hand: switch on DAT_0067e9fc (2..10), tail calls FUN_0040bb90(<string>),
   strings read at 0x005cd7c0..0x005cd81c. Residual: none. */
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
MGLOBAL(int, DAT_0067e9fc, 0x0067e9fc);
MGLOBAL(int, DAT_0067f17c, 0x0067f17c);
MGLOBALA(char, s_TimeTrial_005cd81c, 10, 0x005cd81c);
MGLOBALA(char, s_BronzeMedal_005cd810, 12, 0x005cd810);
MGLOBALA(char, s_SilverMedal_005cd804, 12, 0x005cd804);
MGLOBALA(char, s_GoldMedal_005cd7f8, 10, 0x005cd7f8);
MGLOBALA(char, s_BronzeCup_005cd7ec, 10, 0x005cd7ec);
MGLOBALA(char, s_SilverCup_005cd7e0, 10, 0x005cd7e0);
MGLOBALA(char, s_GoldCup_005cd7d8, 8, 0x005cd7d8);
MGLOBALA(char, s_MultiPlayer_005cd7cc, 12, 0x005cd7cc);
MGLOBALA(char, s_QuickRace_005cd7c0, 10, 0x005cd7c0);
void *FUN_0040bb90(char *a1);

// MATCH: 0x0042ee40 FUN_0042ee40
void *FUN_0042ee40(int param_1)
{
  switch (DAT_0067e9fc) {
  case 2:
    return FUN_0040bb90(s_TimeTrial_005cd81c);
  case 3:
  case 4:
  case 5:
    if (DAT_0067f17c > 9) {
      if (param_1 == 0)
        return FUN_0040bb90(s_BronzeMedal_005cd810);
      if (param_1 == 1)
        return FUN_0040bb90(s_SilverMedal_005cd804);
      if (param_1 == 2)
        return FUN_0040bb90(s_GoldMedal_005cd7f8);
    }
    if (param_1 >= 1000)
      param_1 -= 1000;
    if (param_1 == 0)
      return FUN_0040bb90(s_BronzeCup_005cd7ec);
    if (param_1 == 1)
      return FUN_0040bb90(s_SilverCup_005cd7e0);
    if (param_1 == 2)
      return FUN_0040bb90(s_GoldCup_005cd7d8);
    break;
  case 6:
  case 7:
  case 8:
  case 9:
    if (param_1 == 0)
      return FUN_0040bb90(s_MultiPlayer_005cd7cc);
    break;
  case 10:
    return FUN_0040bb90(s_QuickRace_005cd7c0);
  }
  return 0;
}
