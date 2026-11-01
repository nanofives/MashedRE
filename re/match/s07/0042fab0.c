// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 h4 0x0042fab0: 100% (regs 100%), whole 192 B symbol incl. the jump table BYTE-IDENTICAL. Raw converter
   dropped the string argument (20.0%). By hand: switch 0..9, each case a tail call FUN_0040bb90(<string>), the strings
   read at 0x005cd838..0x005cd898. Residual: none. */
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
MGLOBALA(char, s_NFLRed_005cd898, 7, 0x005cd898);
MGLOBALA(char, s_NFLBluejay_005cd88c, 11, 0x005cd88c);
MGLOBALA(char, s_NFLMelon_005cd880, 9, 0x005cd880);
MGLOBALA(char, s_NFLGold_005cd878, 8, 0x005cd878);
MGLOBALA(char, s_NFLPink_005cd870, 8, 0x005cd870);
MGLOBALA(char, s_NFLShadow_005cd864, 10, 0x005cd864);
MGLOBALA(char, s_NFLCopter_005cd858, 10, 0x005cd858);
MGLOBALA(char, s_NFLBomb_005cd850, 8, 0x005cd850);
MGLOBALA(char, s_NFLFugitive_005cd844, 12, 0x005cd844);
MGLOBALA(char, s_NFLSurvival_005cd838, 12, 0x005cd838);
void FUN_0040bb90(char *a1);

// MATCH: 0x0042fab0 FUN_0042fab0
void FUN_0042fab0(int param_1)
{
  switch (param_1) {
  case 0:
    FUN_0040bb90(s_NFLRed_005cd898);
    return;
  case 1:
    FUN_0040bb90(s_NFLBluejay_005cd88c);
    return;
  case 2:
    FUN_0040bb90(s_NFLMelon_005cd880);
    return;
  case 3:
    FUN_0040bb90(s_NFLGold_005cd878);
    return;
  case 4:
    FUN_0040bb90(s_NFLPink_005cd870);
    return;
  case 5:
    FUN_0040bb90(s_NFLShadow_005cd864);
    return;
  case 6:
    FUN_0040bb90(s_NFLCopter_005cd858);
    return;
  case 7:
    FUN_0040bb90(s_NFLBomb_005cd850);
    return;
  case 8:
    FUN_0040bb90(s_NFLFugitive_005cd844);
    return;
  case 9:
    FUN_0040bb90(s_NFLSurvival_005cd838);
    return;
  }
}
