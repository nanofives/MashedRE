// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00462f50 (h6): 100.0% (regs 100%), whole symbol byte-identical (504 B incl. the jump table;
   Ghidra's 482 B stops before it). Raw 23.8%. Written as a 5-case switch that only sets two play flags
   (esi/edi) and the state, then `if (a == 1)` / `if (b == 1)` play blocks after it; the compiler threads the
   case exits into the play blocks and merges their tails, which is the original's goto web.
   Residual: none. */
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
typedef struct { int h; int pad; } S10Voice;
MGLOBAL(int, DAT_006904e4, 0x006904e4);
MGLOBAL(float, DAT_005ccdf4, 0x005ccdf4);
MGLOBAL(float, DAT_005cd230, 0x005cd230);
MGLOBAL(float, DAT_005cd234, 0x005cd234);
MGLOBAL(float, DAT_005cd238, 0x005cd238);
MGLOBAL(int, DAT_00605478, 0x00605478);
MGLOBAL(int, DAT_00604f44, 0x00604f44);
MGLOBALA(S10Voice, DAT_0068f640, 1, 0x0068f640);
int FUN_0040e350(void);
void FUN_0041da90(float *p0);
int FUN_0042f6a0(void);
void FUN_0045e0f0(int p0, float p1);
void FUN_005a66d0(int p0, int p1);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);
// MATCH: 0x00462f50 FUN_00462f50
void FUN_00462f50(void)
{
  float v;
  int a;
  int b;
  int ch;

  if (FUN_0040e350() != 5) {
    return;
  }
  if (FUN_0042f6a0() == 0xb) {
    return;
  }
  a = 0;
  b = 0;
  FUN_0041da90(&v);
  switch (DAT_006904e4) {
  case 0:
    if (v >= DAT_005cd238) {
      DAT_006904e4 = 1;
      a = 1;
    }
    break;
  case 1:
    if (v >= DAT_005cd234) {
      a = 1;
      DAT_006904e4 = 2;
    }
    if (v < DAT_005cd238) {
      DAT_006904e4 = 0;
    }
    break;
  case 2:
    if (v >= DAT_005cd230) {
      a = 1;
      DAT_006904e4 = 3;
    }
    if (v < DAT_005cd234) {
      DAT_006904e4 = 1;
    }
    break;
  case 3:
    if (v >= DAT_005ccdf4) {
      b = 1;
      DAT_006904e4 = 4;
    }
    if (v < DAT_005cd230) {
      DAT_006904e4 = 2;
    }
    break;
  case 4:
    if (v < DAT_005ccdf4) {
      DAT_006904e4 = 0;
    }
    break;
  default:
    DAT_006904e4 = 0;
    break;
  }
  if (a == 1) {
    ch = DAT_00604f44;
    if ((*(unsigned char *)(DAT_0068f640[ch].h + 0x34) & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[ch].h, 1);
      *(float *)(DAT_0068f640[ch].h + 0x3c) = 22050.0f;
      if (*(int *)(DAT_0068f640[ch].h + 0x30) != 0) {
        FUN_005a6dc0(*(int *)(DAT_0068f640[ch].h + 0x30), 4, 1, 22050.0f);
      }
      FUN_0045e0f0(ch, 0.6f);
    }
  }
  if (b == 1) {
    ch = DAT_00605478;
    if ((*(unsigned char *)(DAT_0068f640[ch].h + 0x34) & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[ch].h, 1);
      *(float *)(DAT_0068f640[ch].h + 0x3c) = 22050.0f;
      if (*(int *)(DAT_0068f640[ch].h + 0x30) != 0) {
        FUN_005a6dc0(*(int *)(DAT_0068f640[ch].h + 0x30), 4, 1, 22050.0f);
      }
      FUN_0045e0f0(ch, 0.6f);
    }
  }
}
