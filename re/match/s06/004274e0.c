// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x004274e0 (124 B): 100.0% exact (regs 100, ops 100), cl 13.00.9176. Raw g2s was 76.9%.
   Fix: the open call written once per case (each case pushes its own string, 0x5cd5e4..0x5cd5ac, and jumps to the
   shared `push 1; push 2; call 0x004cc230` at 0x0042751a); a string pointer chosen in the switch and one call after
   it loads EAX per case instead. Language strings are the .rdata literals named by address. Residual: none. */
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
MGLOBAL(int, DAT_007f0f60, 0x007f0f60);
MGLOBAL(int, DAT_0067d84c, 0x0067d84c);
MGLOBALA(unsigned char, DAT_0066d828, 0x10000, 0x0066d828);
MGLOBALA(char, s_english_dat_005cd5e4, 12, 0x005cd5e4);
MGLOBALA(char, s_french_dat_005cd5d8, 11, 0x005cd5d8);
MGLOBALA(char, s_German_dat_005cd5cc, 11, 0x005cd5cc);
MGLOBALA(char, s_Spanish_dat_005cd5c0, 12, 0x005cd5c0);
MGLOBALA(char, s_Italian_dat_005cd5b4, 12, 0x005cd5b4);
MGLOBALA(char, s_USA_dat_005cd5ac, 8, 0x005cd5ac);
unsigned int FUN_004cbd30(void *p0, void *p1, unsigned int p2);
int FUN_004cc160(void *p0, void *p1);
void *FUN_004cc230(int p0, int p1, const char *p2);

// MATCH: 0x004274e0 FUN_004274e0
int FUN_004274e0(void)
{
  void *f;

  switch (DAT_007f0f60) {
  case 0:
    f = FUN_004cc230(2, 1, s_english_dat_005cd5e4);
    break;
  case 1:
    f = FUN_004cc230(2, 1, s_french_dat_005cd5d8);
    break;
  case 2:
    f = FUN_004cc230(2, 1, s_German_dat_005cd5cc);
    break;
  case 3:
    f = FUN_004cc230(2, 1, s_Spanish_dat_005cd5c0);
    break;
  case 4:
    f = FUN_004cc230(2, 1, s_Italian_dat_005cd5b4);
    break;
  case 5:
    f = FUN_004cc230(2, 1, s_USA_dat_005cd5ac);
    break;
  default:
    return 0;
  }
  if (f != 0) {
    FUN_004cbd30(f, DAT_0066d828, 0x10000);
    FUN_004cc160(f, 0);
    DAT_0067d84c = 1;
    return 1;
  }
  return 0;
}
