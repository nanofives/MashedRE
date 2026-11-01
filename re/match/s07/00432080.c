// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x00432080 (417 B): 100% (regs 100, ops 100), taken from the MATCH-PILOT hand match
   re/match/pilot/a1_00432080.c (same body; globals moved to MGLOBAL with their addresses). Pilot notes:
   start (g2s) 36.4%; switch on DAT_007f1a0c (cases 1, 2, 0x1000: the dec / dec / sub 0xffe chain at
   0x0043209c..0x004320aa), the 8 checks of case 0x1000 written out (bytes at 0x007f1041 + i*0x4c and
   0x007f1501 + i*0x4c, i = 0..7), case 2 as an int index over the 4 x 16-byte table at 0x007f1a14, and
   `if (DAT_0067eca4 == 0) { switch ... }` instead of an early return 0. Residual: none. */
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
typedef struct { int id; int a, b, c; } MEnt;
MGLOBAL(int, DAT_0067eca4, 0x0067eca4);
MGLOBAL(int, DAT_0067f19c, 0x0067f19c);
MGLOBAL(int, DAT_007f1a0c, 0x007f1a0c);
MGLOBALA(MEnt, DAT_007f1a14, 4, 0x007f1a14);
MGLOBALA(char, DAT_007f1041, 0x260, 0x007f1041);
MGLOBALA(char, DAT_007f1501, 0x260, 0x007f1501);
MGLOBAL(int, DAT_0067ecac, 0x0067ecac);
MGLOBAL(int, DAT_0067e844, 0x0067e844);
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
MGLOBAL(int, DAT_0067e914, 0x0067e914);
int FUN_0040e470(int p0);
void FUN_0042d3a0(void);

#define OK(i) (DAT_007f1041[(i) * 0x4c] != 0 && DAT_007f1501[(i) * 0x4c] == 0)

// MATCH: 0x00432080 FUN_00432080
int FUN_00432080(int param_1)
{
  int i;
  DAT_0067f19c = 0;
  if (DAT_0067eca4 == 0) {
  switch (DAT_007f1a0c) {
  case 1:
    if (DAT_007f1a14[0].id != -1 && OK(DAT_007f1a14[0].id))
      DAT_0067eca4 = 1;
    break;
  case 2:
    for (i = 0; i < 4; i++) {
      if (FUN_0040e470(i) == 1 && DAT_007f1a14[i].id != -1 && OK(DAT_007f1a14[i].id))
        DAT_0067eca4 = 1;
    }
    break;
  case 0x1000:
    if (OK(0)) DAT_0067eca4 = 1;
    if (OK(1)) DAT_0067eca4 = 1;
    if (OK(2)) DAT_0067eca4 = 1;
    if (OK(3)) DAT_0067eca4 = 1;
    if (OK(4)) DAT_0067eca4 = 1;
    if (OK(5)) DAT_0067eca4 = 1;
    if (OK(6)) DAT_0067eca4 = 1;
    if (OK(7)) DAT_0067eca4 = 1;
    break;
  default:
    return 0;
  }
  if (DAT_0067eca4 == 1) {
    FUN_0042d3a0();
    DAT_0067ecac = param_1;
    DAT_0067e844 = 0;
    DAT_0067e9f8 = 0;
    DAT_0067e914 = 0;
    DAT_0067f19c = 2;
    return 1;
  }
  }
  return 0;
}
