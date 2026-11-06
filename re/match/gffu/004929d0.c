// FLAGS: /O2 /GS /Ob1
/* MATCH-GFFU 0x004929d0 (790 B): 95.7% (regs 95.7%, ops 96.2%), 235/235 instructions. Raw g2s.py output scored
   72.8%. Source form: the switch cases in the original block order (jump table 0x00492ce8: case 1 -> 0x004929ef,
   2 -> 0x00492a67, 3 -> 0x00492a80, 7 -> 0x00492c04, 5 -> 0x00492c25, 4 -> 0x00492c46, 6 -> 0x00492cc9), case 1
   as a switch on 0x0042b930 (dec eax / sub eax,3 at 0x00492a1f), case 4 as two calls of 0x0042b940 with 3 and 0
   (push 3 / push esi at 0x00492c6e..0x00492c74), the place-count loop with i = 0 before the 0x0040e340 call
   (0x00492b82) and `score > next` (cmp ebx,eax / jle at 0x00492bc7), the boolean callees 0x0042b8d0 / 0x0042b8e0 /
   0x0042b8f0 as int (test eax,eax). match_permute.py (3 rounds, 710 compiles) found case 4's `!= 0` order.
   Residual (10 instructions): the original saves esi after the jump-table range check (push esi at 0x004929e8,
   the default path 0x00492cdd skips the pop), this compile saves it before the call of 0x004927c0, so each return
   pops it earlier. No other instruction differs. */
/* ---- MATCH-GFFU preamble (same text in every re/match/gffu source; not part of any function body) ----
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
MGLOBAL(int, DAT_00771968, 0x00771968);
MGLOBAL(int, DAT_0077196c, 0x0077196c);
MGLOBAL(int, DAT_00771970, 0x00771970);
MGLOBAL(int, DAT_00771974, 0x00771974);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBALA(int, DAT_007f1a18, 16, 0x007f1a18);
void FUN_0040b430(int p0, int p1);
void FUN_0040b540(int *p0);
int FUN_0040b6d0(int p0);
void FUN_0040b6e0(int p0);
void FUN_0040b700(int p0, int p1);
void FUN_0040d440(void);
int FUN_0040e340(void);
int FUN_0040e350(void);
int FUN_0040e450(void);
void FUN_0040e460(int p0);
int FUN_0042b8d0(void);
int FUN_0042b8e0(void);
int FUN_0042b8f0(void);
int FUN_0042b910(void);
int FUN_0042b930(void);
void FUN_0042b940(int p0);
int FUN_0042c1c0(void);
void FUN_0042c1d0(void);
int FUN_0042c220(void);
void FUN_0042c280(void);
int FUN_0042c2d0(void);
int FUN_0042c2e0(void);
void FUN_0042c2f0(int p0);
int FUN_0042f500(void);
int FUN_0042f6a0(void);
int FUN_00432080(int p0);
void FUN_004331a0(int p0);
void FUN_00448700(int p0, int p1);
void FUN_004927c0(void);
int FUN_005c9d00(void);
// MATCH: 0x004929d0 FUN_004929d0
int FUN_004929d0(void)
{
  int i;
  int score;
  int run;
  int place;
  int team;
  int order[4];

  FUN_004927c0();
  switch (DAT_00771968) {
  case 1:
    if (FUN_0042c2e0() != 0) {
      FUN_0042b940(FUN_0042c2d0());
      FUN_0042c2f0(0);
    }
    switch (FUN_0042b930()) {
    case 1:
      if (FUN_0042b8d0() != 0) {
        DAT_00771968 = 2;
        FUN_0042b940(0);
        return 1;
      }
      break;
    case 4:
      DAT_00771968 = 4;
      FUN_0042b940(0);
      return 1;
    }
    break;
  case 2:
    FUN_0040d440();
    DAT_00771968 = 3;
    return 1;
  case 3:
    FUN_0042c1d0();
    if (FUN_0042c220() != 0) {
      FUN_0042c280();
    }
    if (FUN_0042c1c0() != 0) {
      DAT_00771968 = 7;
    }
    i = FUN_0042f6a0();
    if (i != 0xb && (i = FUN_0040e350(), i != 0xb) && i != 9) {
      if (FUN_00432080(FUN_005c9d00()) != 0) {
        DAT_00771968 = 5;
      }
    }
    if (FUN_0040e450() != 0) {
      FUN_004331a0(FUN_0042b910());
      DAT_00771968 = 5;
      DAT_00771974 = 1;
      FUN_0040b6e0(1);
      FUN_0040b540(order);
      place = FUN_0040e340() - 1;
      if (FUN_0042f500() == 0 || DAT_007f0fd0 == 2) {
        run = 0;
        i = 0;
        if (0 < FUN_0040e340()) {
          do {
            FUN_0040b430(order[i], place);
            FUN_0040b700(order[i], place);
            score = FUN_0040b6d0(order[i]);
            if (i < 3) {
              if (score > FUN_0040b6d0(order[i + 1])) {
                place = place + (-1 - run);
                run = 0;
              } else {
                run = run + 1;
              }
            }
            i = i + 1;
          } while (i < FUN_0040e340());
        }
      } else {
        team = DAT_007f1a18[order[0] * 4];
        i = 0;
        do {
          if (team == DAT_007f1a18[order[i] * 4]) {
            FUN_0040b430(order[i], 1);
            FUN_0040b700(order[i], 1);
          } else {
            FUN_0040b700(order[i], 0);
          }
          i = i + 1;
        } while (i < 4);
      }
      FUN_00448700(1, order[0]);
      FUN_0040e460(0);
      return 1;
    }
    break;
  case 7:
    if (FUN_0042c1c0() == 0) {
      DAT_00771968 = 3;
      return 1;
    }
    break;
  case 5:
    if (FUN_0042b8e0() != 0) {
      DAT_00771968 = 4;
      return 1;
    }
    break;
  case 4:
    if (DAT_0077196c != 0 || DAT_00771970 != 0 || DAT_00771974 != 0) {
      DAT_00771968 = 1;
      if (DAT_00771974 != 0) {
        FUN_0042b940(3);
      } else {
        FUN_0042b940(0);
      }
      DAT_0077196c = 0;
      DAT_00771970 = 0;
      DAT_00771974 = 0;
    }
    if (FUN_0042c2e0() != 0) {
      FUN_0042b940(FUN_0042c2d0());
      FUN_0042c2f0(0);
    }
    if (FUN_0042b8f0() != 0) {
      DAT_00771968 = 6;
      return 1;
    }
    break;
  case 6:
    if (FUN_0042b8d0() != 0) {
      DAT_00771968 = 3;
    }
    break;
  }
  return 1;
}
