// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00429310 (h9, by hand): 98.3% (regs 98.3%, ops 98.9%), 175/175 ins.
   Raw converter 50.8%. What moved it: FUN_00408ad0 takes 1 arg and returns float, FUN_0042b8c0 returns short,
   the .rdata floats are source literals (0.0f 0x005d757c, 0.025f 0x005cc9a4, 10.0f 0x005cc55c, 1/3000
   0x005cc948), the sector bracket is one || condition (jump threading builds the 0x0042954a shortcut), and
   secs/mins/hund are block-scoped inside the `== 11` block so they overlap t and the (float) temps in a 12-byte
   frame (76% -> 98.3%). Residual: in the last FUN_00430b00 call the original loads hund (`mov ecx,[esp+0x48]`)
   before the deferred `add esp,0x48`, ours after it. Tried: 6 declaration orders, function- vs block-scoped
   vars, split if, >10 vs >=11, explicit return, permuter 223 variants: no change. Class (b) (not proven
   compiler-version). */
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
MGLOBAL(float, DAT_008991b4, 0x008991b4);
MGLOBAL(int, DAT_008991b8, 0x008991b8);
MGLOBAL(int, DAT_008991bc, 0x008991bc);
MGLOBAL(int, DAT_0067d98c, 0x0067d98c);
MGLOBAL(int, DAT_0067d994, 0x0067d994);
MGLOBAL(float, DAT_0067d99c, 0x0067d99c);
MGLOBAL(int, DAT_007f0ff4, 0x007f0ff4);
MGLOBALA(char, s_time_trial_time_is__f_005cd6ac, 23, 0x005cd6ac);
MGLOBALA(char, s_gametimer_is__f_005cd698, 17, 0x005cd698);
float FUN_00408ad0(int p0);
int FUN_0040dc70(void);
void FUN_0040dcb0(void);
int FUN_0040e350(void);
void FUN_0040e360(int p0);
void FUN_00411350(int p0, float *p1, int *p2, int *p3);
void FUN_00411580(int p0, float *p1, int *p2, int *p3);
void FUN_004115c0(int p0, float *p1, int *p2, int *p3);
void FUN_00411870(int p0);
short FUN_0042b8c0(void);
int FUN_0042f6a0(void);
void FUN_00430b00(int p0, int p1, float p2, int p3, int p4);
void FUN_004987b0(const char *fmt, ...);
// MATCH: 0x00429310 FUN_00429310
/* [C1 2026-06-01] Per-frame time-trial tick. Advances a frame counter + a timer, drives sector
   progression (1..11). */
void FUN_00429310(void)
{
  float t;

  if (FUN_0040dc70() != 0) {
    FUN_0040dcb0();
  }
  DAT_008991b8++;
  if (DAT_008991b4 != 0.0f) {
    DAT_008991b4 = DAT_008991b4 + (float)FUN_0042b8c0() * 0.025f;
    if (DAT_008991b4 >= 0.0f) {
      DAT_008991b4 = 0.0f;
    }
  }
  if (FUN_0042f6a0() != 2) {
    return;
  }
  t = FUN_00408ad0(0);
  if (DAT_008991bc == 0) {
    return;
  }
  if (((float)(DAT_008991bc * 10) < t && (float)(DAT_008991bc * 10 + 10) > t) ||
      (DAT_008991bc == 10 && t < 10.0f)) {
    DAT_008991bc++;
  }
  if (DAT_008991bc == 11 && FUN_0040e350() == 6) {
    float secs;
    int mins;
    int hund;

    FUN_004987b0(s_time_trial_time_is__f_005cd6ac, (double)((float)DAT_0067d994 + DAT_0067d99c));
    FUN_004987b0(s_gametimer_is__f_005cd698, (double)((float)DAT_007f0ff4 * 0.00033333333f));
    FUN_004115c0(0, &secs, &mins, &hund);
    FUN_00430b00(0, 0, secs, mins, hund);
    FUN_004115c0(1, &secs, &mins, &hund);
    FUN_00430b00(1, 0, secs, mins, hund);
    secs = 0.99f;
    mins = 0x3b;
    hund = 9;
    FUN_00411580(0, &secs, &mins, &hund);
    FUN_00430b00(0, 1, secs, mins, hund);
    secs = 0.99f;
    mins = 0x3b;
    hund = 9;
    FUN_00411580(1, &secs, &mins, &hund);
    FUN_00430b00(1, 1, secs, mins, hund);
    FUN_00411870(1);
    FUN_0040e360(7);
  }
  if (DAT_008991bc >= 11) {
    return;
  }
  FUN_00411350(DAT_007f0ff4, &DAT_0067d99c, &DAT_0067d994, &DAT_0067d98c);
  if (DAT_0067d98c >= 10) {
    DAT_0067d99c = 0.99f;
    DAT_0067d994 = 0x3b;
    DAT_0067d98c = 9;
  }
}
