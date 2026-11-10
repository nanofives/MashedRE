// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-FRESH4 0x004177b0 (first hand source; raw g2s 23.8%): 1339 B whole symbol BYTE-IDENTICAL
   (Ghidra body 1338 B stops before the ret), relocheck OK (93 checked, __ftol unchecked). Reference: the hand
   port mashedmod/src/mashed_re/Ai/AiPreTick.cpp (read only). Closed by, in order:
   - the per-car records and DAT_0089a368 as one block at 0x0089a360 (cars at +0x160, stride 0x74, as
     re/match/s03/h4_00417180.c): cl then re-reads 0x0089a368 after the zeroing loops (72.6 -> 88.4%);
   - the mode-4 and mode-9 `DAT_0089a368 = 2; return;` written as two statements: cl promotes the constant 2
     to EBP like the original (69.9 -> 72.6%);
   - parentheses around `(10.0f - level) * 0.1f` keep two fmuls (without them cl folds 0.1f * 1/6);
   - a named float for (float)FUN_00407a20(v) and for the FUN_00408ad0 result (call order and the
     DAT_007f0fd0 load slot);
   - the table entry as an int local compared `n >= rnd` (the int copy to [esp+0xc] before the fild);
   - band 3 nested under `if (d > 20.0f)`, and band 4's repeated `d > 20` read from the .rdata float at
     0x005ccd6c as a global: with the literal cl folds the two compares into one and pops d early
     (0x00417bbc / 0x00417be2). DAT_005ccd6c holds 20.0f, the same value as the literal.
   Literals read from the anchored image: 0.01f 0x005cc328, 3.0f 0x005cc31c, -1.0f 0x005cc33c, 1.5f 0x005cc348,
   0.75f 0x005cc950, 0.025f 0x005cc9a4, 2.0f 0x005cc574, 10.0f 0x005cc55c, 0.1f 0x005cc56c, 1/6 0x005cc8f4,
   1.0f 0x005cc320, 1/3 0x005ccac8, 0.5f 0x005cc32c, 0.05f 0x005cc9a0, 0.15f 0x005cc8f0, 0.25f 0x005cc564,
   1/3000 0x005cc948, 12.0f 0x005cc354, 22.0f 0x005cd0f8, 42.0f 0x005cd0f4, 60.0f 0x005cc728, 62.0f 0x005cd0f0.
   The band variable is read uninitialised on one path (`mov edx,[esp+0x10]` at 0x00417b6c), as in the original.
   Residual: none. */
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
typedef struct { float x, y, z; } RwV3d;
typedef struct {
  int f0;
  int count;
  int timer;
  int state;
  int sub;
  char pad0[0x8];
  int latch;
  int target;
  char pad1[0x24];
  int rtimer;
  char pad2[0x28];
} AiCar;
typedef struct {
  float level;
  int i364;
  int mode;
  int modetime;
  float stamp;
  int band;
  char pad[0x148];
  AiCar car[4];
} AiBlock;
MGLOBAL(AiBlock, DAT_0089a360, 0x0089a360);
MGLOBAL(float, DAT_0089a870, 0x0089a870);
MGLOBAL(float, DAT_0089a874, 0x0089a874);
MGLOBAL(float, DAT_0089a878, 0x0089a878);
MGLOBAL(float, DAT_0089a87c, 0x0089a87c);
MGLOBALA(float, DAT_0089a880, 4, 0x0089a880);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(float, DAT_005ccd6c, 0x005ccd6c);
MGLOBAL(int, DAT_007f0ff8, 0x007f0ff8);
MGLOBAL(int, DAT_007f1008, 0x007f1008);
typedef int Row5[5];
MGLOBALA(Row5, DAT_005f30a0, 11, 0x005f30a0);
MGLOBALA(Row5, DAT_005f3180, 11, 0x005f3180);
int FUN_00443080(void);
float FUN_0046dd80(void);
void FUN_0046dd90(int car, float f);
int FUN_00407a20(int car);
float FUN_00408ad0(int car);
float FUN_00417730(int car);
int FUN_00454a30(int car);
int FUN_0040e470(int car);
int FUN_0040e350(void);
float FUN_00472650(float lo, float hi);

// MATCH: 0x004177b0 FUN_004177b0
void FUN_004177b0(void)
{
  int v;
  int found;
  int any;
  int row;
  int band;
  int flag;
  float fv;
  float mult;
  float ang;
  float t;
  float d;
  float p;
  int m;
  float r;
  int n;
  float a20f;

  if (FUN_00443080() != 0) {
    DAT_0089a360.mode = 2;
  }
  for (v = 0; v < 4; v++) {
    FUN_0046dd90(v, FUN_0046dd80());
    a20f = (float)FUN_00407a20(v);
    r = FUN_00408ad0(v);
    DAT_0089a880[v] = r * 0.01f + a20f;
    if ((DAT_007f0fd0 == 4 || DAT_007f0fd0 == 9 || DAT_007f0fd0 == 7 || DAT_007f0fd0 == 8) &&
        DAT_0089a880[v] >= 3.0f) {
      fv = (float)v;
      found = 0;
      if (DAT_0089a870 == fv) {
        found = 1;
      }
      if (DAT_0089a874 == fv) {
        found = 1;
      }
      if (DAT_0089a878 == fv) {
        found = 1;
      }
      if (DAT_0089a87c == fv) {
        found = 1;
      }
      if (found == 0) {
        if (DAT_0089a870 == -1.0f) {
          DAT_0089a870 = fv;
        }
        else if (DAT_0089a874 == -1.0f) {
          DAT_0089a874 = fv;
        }
        else if (DAT_0089a878 == -1.0f) {
          DAT_0089a878 = fv;
        }
        else if (DAT_0089a87c == -1.0f) {
          DAT_0089a87c = fv;
        }
      }
    }
  }
  if (DAT_007f0fd0 == 9) {
    mult = 1.0f;
    ang = FUN_00417730(1);
    if (ang < 1.5f) {
      mult = 0.95f;
    }
    if (ang < 0.75f) {
      mult = 0.9f;
    }
    if (ang < 0.025f) {
      mult = 0.25f;
    }
    if (ang > 2.0f) {
      mult = 1.15f;
    }
    FUN_0046dd90(1, FUN_0046dd80() * mult);
  }
  if (DAT_007f0fd0 == 4) {
    for (v = 1; v < 4; v++) {
      mult = 1.0f;
      ang = FUN_00417730(v);
      if (ang > 0.75f) {
        mult = ((10.0f - DAT_0089a360.level) * 0.1f) * 0.16666667f + 1.0f;
      }
      if (ang > 2.0f) {
        mult = ((10.0f - DAT_0089a360.level) * 0.1f) * 0.33333334f + mult;
      }
      if (ang < 0.5f) {
        mult = mult - 0.15f;
      }
      if (ang < 0.05f) {
        mult = mult - 0.25f;
      }
      FUN_0046dd90(v, FUN_0046dd80() * mult);
    }
  }
  any = 0;
  for (v = 0; v < 4; v++) {
    if (FUN_00454a30(v) != 0) {
      any = 1;
    }
  }
  if (any != 0) {
    for (v = 0; v < 4; v++) {
      if (FUN_0040e470(v) == 2) {
        FUN_0046dd90(v, FUN_0046dd80() * 2.0f);
      }
    }
  }
  if (DAT_007f0fd0 == 4) {
    DAT_0089a360.mode = 2;
    return;
  }
  if (DAT_007f0fd0 == 9) {
    DAT_0089a360.mode = 2;
    return;
  }
  if (DAT_007f0fd0 == 8) {
    DAT_0089a360.mode = 0;
    return;
  }
  if (FUN_0040e350() != 6) {
    return;
  }
  if (DAT_0089a360.mode == 2) {
    DAT_0089a360.modetime += DAT_007f1008;
    if (DAT_0089a360.modetime > 180000) {
      DAT_0089a360.mode = 1;
      DAT_0089a360.modetime = 0;
    }
  }
  row = (int)DAT_0089a360.level;
  flag = 0;
  t = (float)DAT_007f0ff8 * 0.00033333333f;
  d = t - DAT_0089a360.stamp;
  if (d > 1.0f && t < 2.0f) {
    band = 0;
    flag = 1;
  }
  if (d > 10.0f) {
    if (t < 12.0f) {
      band = 1;
      flag = 1;
    }
    if (t > 20.0f && t < 22.0f) {
      band = 2;
      flag = 1;
    }
    if (d > 20.0f) {
      if (t < 42.0f) {
        band = 3;
        flag = 1;
      }
      if (d > DAT_005ccd6c && t > 60.0f && t < 62.0f) {
        band = 4;
        flag = 1;
      }
    }
  }
  if (flag != 0) {
    DAT_0089a360.stamp = t;
    DAT_0089a360.band = band;
    for (v = 0; v < 4; v++) {
      DAT_0089a360.car[v].timer = 0;
      DAT_0089a360.car[v].count = 0;
    }
  }
  if (DAT_0089a360.mode != 0) {
    return;
  }
  if (flag == 0) {
    return;
  }
  n = DAT_005f30a0[row][band];
  if (n >= FUN_00472650(0.0f, 100.0f)) {
    DAT_0089a360.mode = 1;
    for (v = 0; v < 4; v++) {
      DAT_0089a360.car[v].f0 = 0;
    }
    return;
  }
  n = DAT_005f3180[row][band];
  if (n >= FUN_00472650(0.0f, 100.0f)) {
    DAT_0089a360.mode = 2;
  }
}
