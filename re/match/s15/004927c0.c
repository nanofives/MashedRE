// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x004927c0 (h7, by hand): 100.0% (regs 100.0%). Raw 26.3%. What moved it: the 12-entry step table
   as an initialised local int array, one flag r (2 = any listed byte set with its partner clear, 1 = the step's
   pair), switch cases in source order 0,1,3,2 (the original's code order), r == 1 / else r == 2 tail,
   DAT_007f0a40 filled by a 156-iteration int loop (rep stosd). Residual: none. */
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
typedef unsigned int undefined4;
MGLOBAL(int, DAT_00771980, 0x00771980);
MGLOBAL(char, DAT_007f1046, 0x007f1046);
MGLOBAL(char, DAT_007f1506, 0x007f1506);
MGLOBAL(char, DAT_007f10de, 0x007f10de);
MGLOBAL(char, DAT_007f159e, 0x007f159e);
MGLOBAL(char, DAT_007f1047, 0x007f1047);
MGLOBAL(char, DAT_007f1507, 0x007f1507);
MGLOBAL(char, DAT_007f10df, 0x007f10df);
MGLOBAL(char, DAT_007f159f, 0x007f159f);
MGLOBAL(char, DAT_007f1044, 0x007f1044);
MGLOBAL(char, DAT_007f1504, 0x007f1504);
MGLOBAL(char, DAT_007f10dc, 0x007f10dc);
MGLOBAL(char, DAT_007f159c, 0x007f159c);
MGLOBAL(char, DAT_007f1045, 0x007f1045);
MGLOBAL(char, DAT_007f1505, 0x007f1505);
MGLOBAL(char, DAT_007f10dd, 0x007f10dd);
MGLOBAL(char, DAT_007f159d, 0x007f159d);
MGLOBAL(int, DAT_007f0f2c, 0x007f0f2c);
MGLOBALA(int, DAT_007f0a40, 156, 0x007f0a40);
// MATCH: 0x004927c0 FUN_004927c0
void FUN_004927c0(void)
{
  int seq[12] = { 0, 2, 1, 3, 0, 3, 1, 2, 0, 1, 0, 1 };
  int r;
  int i;

  if (DAT_00771980 < 0 || DAT_00771980 > 11) {
    DAT_00771980 = 0;
  }
  r = 0;
  if (DAT_007f1046 && !DAT_007f1506) r = 2;
  if (DAT_007f10de && !DAT_007f159e) r = 2;
  if (DAT_007f1047 && !DAT_007f1507) r = 2;
  if (DAT_007f10df && !DAT_007f159f) r = 2;
  if (DAT_007f1044 && !DAT_007f1504) r = 2;
  if (DAT_007f10dc && !DAT_007f159c) r = 2;
  if (DAT_007f1045 && !DAT_007f1505) r = 2;
  if (DAT_007f10dd && !DAT_007f159d) r = 2;
  switch (seq[DAT_00771980]) {
  case 0:
    if (DAT_007f1046 && !DAT_007f1506) r = 1;
    if (DAT_007f10de && !DAT_007f159e) r = 1;
    break;
  case 1:
    if (DAT_007f1047 && !DAT_007f1507) r = 1;
    if (DAT_007f10df && !DAT_007f159f) r = 1;
    break;
  case 3:
    if (DAT_007f1044 && !DAT_007f1504) r = 1;
    if (DAT_007f10dc && !DAT_007f159c) r = 1;
    break;
  case 2:
    if (DAT_007f1045 && !DAT_007f1505) r = 1;
    if (DAT_007f10dd && !DAT_007f159d) r = 1;
    break;
  }
  if (r == 1) {
    DAT_00771980++;
  } else if (r == 2) {
    DAT_00771980 = 0;
  }
  if (DAT_00771980 == 12) {
    for (i = 0; i < 156; i++) {
      DAT_007f0a40[i] = 2;
    }
    DAT_007f0f2c = 1;
  }
}
