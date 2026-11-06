// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045cc80 (h2): 100% (regs 100%). Raw 52.6%. A switch with two cases carrying identical bodies
   (merged by the compiler) gives sub/je/dec/jne; an `||` test gives cmp/cmp and a shared case gives a range check.
   Ring index walk with a done flag tested at the loop top and the count in a do-while. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
MGLOBAL(int, DAT_007e9de0, 0x007e9de0);
MGLOBAL(int, DAT_007f1030, 0x007f1030);
MGLOBALA(int, DAT_007e9de4, 256, 0x007e9de4);
MGLOBALA(int, DAT_007ea1e4, 256, 0x007ea1e4);
MGLOBALA(int, DAT_007ea5e4, 256, 0x007ea5e4);
MGLOBALA(int, DAT_007ea9e4, 2048, 0x007ea9e4);
void FUN_0040e350(void);
void FUN_0045cb20(int param_1, int param_2, int param_3);
// MATCH: 0x0045cc80 FUN_0045cc80
void FUN_0045cc80(void)
{
  int idx;
  int frame;
  int count;
  int done;
  int a;
  int b;

  FUN_0040e350();
  idx = DAT_007e9de0 - 1;
  if (idx < 0)
    idx = 0xff;
  frame = DAT_007f1030;
  count = 0x100;
  done = 0;
  do {
    if (done)
      break;
    if (DAT_007ea1e4[idx] == frame) {
      a = DAT_007ea5e4[idx] - 0xff00;
      b = DAT_007ea9e4[idx * 8] - 0xff00;
      switch (DAT_007e9de4[idx]) {
      case 0x3ff03:
        FUN_0045cb20(0x13, a, b);
        FUN_0045cb20(0x13, b, a);
        break;
      case 0x3ff04:
        FUN_0045cb20(0x13, a, b);
        FUN_0045cb20(0x13, b, a);
        break;
      }
    } else {
      done = 1;
    }
    idx--;
    if (idx < 0)
      idx = 0xff;
    count--;
  } while (count > 0);
}
