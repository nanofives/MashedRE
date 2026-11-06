// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045d0e0 (h2): 100% (regs 100%). Raw 62.2%. Moves: counter as a plain `DAT_0068d53c[param_1]++`
   (the compiler keeps the address in EDI); if/else with the long branch first
   (`FUN_0040e470(..) >= 2 || FUN_00431b70() != 2`) and the byte-flag branch in the else arm ending in
   `if (..== 0) return 1;` that falls through to the shared `return 0` (a `return 0; return 1;` pair becomes
   sete). Permuter found the branch swap (98.8% -> 100%). */
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
typedef struct { int pad00[3]; int state; } Obj;
typedef struct { int pad00[5]; float f14; } Ent28;
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(float, DAT_005ce658, 0x005ce658);
MGLOBALA(int, DAT_0068d53c, 4, 0x0068d53c);
MGLOBALA(char, DAT_007f1077, 1, 0x007f1077);
MGLOBALA(char, DAT_007f1537, 1, 0x007f1537);
MGLOBALA(int, DAT_007f1a14, 16, 0x007f1a14);
MGLOBALA(int, DAT_0088f0c0, 4, 0x0088f0c0);
MGLOBALA(Obj *, DAT_0088f680, 4, 0x0088f680);
MGLOBALA(Ent28 *, DAT_0088f6a0, 4, 0x0088f6a0);
int FUN_0040e470(int param_1);
int FUN_00431b70(void);
int FUN_0045cab0(int param_1);
int FUN_0045dff0(int param_1);
// MATCH: 0x0045d0e0 FUN_0045d0e0
int FUN_0045d0e0(int param_1)
{
  int mode;
  int i;

  mode = DAT_007f0fd0;
  DAT_0068d53c[param_1]++;
  if (FUN_00431b70() != 0) {
    if (FUN_0040e470(param_1) >= 2 || FUN_00431b70() != 2) {
      for (i = 0; i < DAT_008aa254; i++) {
        if (i != param_1 && DAT_0088f680[i] != 0 && DAT_0088f680[i]->state == 1)
          return 0;
      }
      if (mode == 4 && FUN_0045dff0(param_1) == 0)
        return 0;
      if (DAT_0088f0c0[param_1] == 1) {
        if (DAT_0088f6a0[param_1]->f14 > DAT_005ce658)
          return 1;
        if (DAT_0068d53c[param_1] > 1000 && FUN_0045cab0(param_1) != 0) {
          DAT_0068d53c[param_1] = 0;
          return 1;
        }
      }
    } else {
      if (DAT_007f1077[DAT_007f1a14[param_1 * 4] * 0x4c] == 0)
        return 0;
      if (DAT_007f1537[DAT_007f1a14[param_1 * 4] * 0x4c] == 0)
        return 1;
    }
  }
  return 0;
}
