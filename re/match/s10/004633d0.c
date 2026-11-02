// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x004633d0 (h6): 100.0% (regs 100%), whole symbol byte-identical (442 B). Raw 41.8%.
   The .rdata floats 0x005cc320/0x005ce990/0x005ce6b8/0x005ccd6c/0x005cc358 are float literals (1, 10000,
   22050, 20, 5): as globals the compiler loads them first, as literals it loads param_2 first like the
   original. FUN_004a2c48 is __ftol (an int cast). The x87 shape needs the pitch computed from
   (1.0f - param_2) BEFORE the separate t = 1.0f - param_2 (CSE keeps t on the stack, fmul st(1)).
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
MGLOBAL(int, DAT_006059ac, 0x006059ac);
MGLOBAL(int, DAT_00690524, 0x00690524);
MGLOBAL(int, DAT_00690528, 0x00690528);
MGLOBAL(int, DAT_00605918, 0x00605918);
MGLOBAL(int, DAT_00605a40, 0x00605a40);
MGLOBALA(S10Voice, DAT_0068f640, 1, 0x0068f640);
void FUN_0045e0f0(int p0, float p1);
void FUN_005a66d0(int p0, int p1);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);
// MATCH: 0x004633d0 FUN_004633d0
void FUN_004633d0(int param_1, float param_2)
{
  int ch;
  float t;
  float pitch;

  if (param_1 == 1) {
    ch = DAT_006059ac;
    if ((*(unsigned char *)(DAT_0068f640[ch].h + 0x34) & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[ch].h, 1);
      *(float *)(DAT_0068f640[ch].h + 0x3c) = 22050.0f;
      if (*(int *)(DAT_0068f640[ch].h + 0x30) != 0) {
        FUN_005a6dc0(*(int *)(DAT_0068f640[ch].h + 0x30), 4, 1, 22050.0f);
      }
      FUN_0045e0f0(ch, 1.0f);
      DAT_00690524 = 0;
      DAT_00690528 = 0;
    }
  }
  else if (param_1 == 2) {
    ch = DAT_00605918;
    if ((*(unsigned char *)(DAT_0068f640[ch].h + 0x34) & 2) == 0 && DAT_00690524 == 1) {
      pitch = 10000.0f * (1.0f - param_2) + 22050.0f;
      t = 1.0f - param_2;
      FUN_005a66d0(DAT_0068f640[ch].h, 1);
      *(float *)(DAT_0068f640[ch].h + 0x3c) = pitch;
      if (*(int *)(DAT_0068f640[ch].h + 0x30) != 0) {
        FUN_005a6dc0(*(int *)(DAT_0068f640[ch].h + 0x30), 4, 1, pitch);
      }
      FUN_0045e0f0(ch, t);
      DAT_00690524 = 0;
    }
    else {
      DAT_00690528++;
      if (DAT_00690528 >= (int)(param_2 * 20.0f + 5.0f)) {
        DAT_00690528 = 0;
        DAT_00690524 = 1;
      }
    }
  }
  else if (param_1 == 3) {
    ch = DAT_00605a40;
    if ((*(unsigned char *)(DAT_0068f640[ch].h + 0x34) & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[ch].h, 1);
      *(float *)(DAT_0068f640[ch].h + 0x3c) = 22050.0f;
      if (*(int *)(DAT_0068f640[ch].h + 0x30) != 0) {
        FUN_005a6dc0(*(int *)(DAT_0068f640[ch].h + 0x30), 4, 1, 22050.0f);
      }
      FUN_0045e0f0(ch, 1.0f);
    }
  }
}
