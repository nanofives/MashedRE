// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00464e10 (h8): 100% by hand (raw 57.5%). What moved it: the additive/multiplicative constants of the
   two y-based volumes as literals (2.5f, 0.125f, 0.7f, 0.25f) so the local loads first, the clamp bounds
   and 1.0 / 0.8 / 0.01 as the globals, + 0.05f as a literal (as
   the global, cl loads it first and relocheck reports the swapped pair); FUN_00472650 is a float (lo, hi) random returning float, its
   (int) casts are __ftol (0x004a2c48); DAT_00690570 = 0.0f written before n = 0 (the other order stores the
   zero register, 99.4%). Same-file shape as 0x004652b0. */
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
typedef struct { float x, y, z; } MatchV3_464e10;
typedef struct { char *p; float f; } MatchRec0068f640;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(char *, DAT_0088e670, 0x0088e670);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cc9bc, 0x005cc9bc);
MGLOBAL(float, DAT_005cc328, 0x005cc328);
MGLOBAL(int, DAT_0068f618, 0x0068f618);
MGLOBAL(float, DAT_00690570, 0x00690570);
MGLOBAL(MatchV3_464e10, DAT_00690480, 0x00690480);
MGLOBAL(int, DAT_006900d0, 0x006900d0);
MGLOBAL(int, DAT_0068fccc, 0x0068fccc);
MGLOBAL(int, DAT_00690574, 0x00690574);
MGLOBALA(MatchRec0068f640, DAT_0068f640, 1, 0x0068f640);
float FUN_00408ad0(int i);
void FUN_004417e0(MatchV3_464e10 *out);
void FUN_0045e0f0(int p0, float p1);
int FUN_0046c7b0(int i);
float FUN_00472650(float lo, float hi);
void FUN_005459c0(int a, int b, float c, MatchV3_464e10 *v, int d);
void FUN_005a66d0(char *p, int a);
void FUN_005a6d60(int h, int a, int b, MatchV3_464e10 *v);
void FUN_005a6dc0(int h, int a, int b, float f);
// MATCH: 0x00464e10 FUN_00464e10
void FUN_00464e10(void)
{
  int ch;
  int i;
  int n;
  float v;
  float vol;
  MatchV3_464e10 loc;
  MatchV3_464e10 me;

  if (DAT_0068f4c8 == 0 || DAT_008aa254 <= 1) {
    FUN_004417e0(&me);
    ch = *(int *)(DAT_0088e670 + 0x6b0);
    if ((*(unsigned char *)(DAT_0068f640[ch].p + 0x34) & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[ch].p, 1);
    }
    *(float *)(DAT_0068f640[ch].p + 0x3c) = 22050.0f;
    if (*(int *)(DAT_0068f640[ch].p + 0x30) != 0) {
      FUN_005a6dc0(*(int *)(DAT_0068f640[ch].p + 0x30), 4, 1, 22050.0f);
    }
    vol = (me.y + 2.5f) * 0.125f;
    if (vol < DAT_005d757c) {
      vol = 0.0f;
    }
    else if (vol > DAT_005cc320) {
      vol = 1.0f;
    }
    FUN_0045e0f0(ch, vol);
    ch = *(int *)(DAT_0088e670 + 0x470);
    if ((*(unsigned char *)(DAT_0068f640[ch].p + 0x34) & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[ch].p, 1);
      *(float *)(DAT_0068f640[ch].p + 0x3c) = 22050.0f;
      if (*(int *)(DAT_0068f640[ch].p + 0x30) != 0) {
        FUN_005a6dc0(*(int *)(DAT_0068f640[ch].p + 0x30), 4, 1, 22050.0f);
      }
    }
    v = (me.y + 0.7f) * 0.125f - 0.25f;
    if (v < DAT_005d757c) {
      v = DAT_005d757c;
    }
    else if (v > DAT_005cc320) {
      v = DAT_005cc320;
    }
    FUN_0045e0f0(ch, DAT_005cc320 - v);
    ch = *(int *)(DAT_0088e670 + 0x500);
    if ((*(unsigned char *)(DAT_0068f640[ch].p + 0x34) & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[ch].p, 1);
      *(float *)(DAT_0068f640[ch].p + 0x3c) = 22050.0f;
      if (*(int *)(DAT_0068f640[ch].p + 0x30) != 0) {
        FUN_005a6dc0(*(int *)(DAT_0068f640[ch].p + 0x30), 4, 1, 22050.0f);
      }
    }
    FUN_0045e0f0(ch, DAT_005cc9bc - FUN_00472650(0.0f, 0.1f));
    if (DAT_0068f618 != 0) {
      DAT_00690570 = 0.0f;
      n = 0;
      for (i = 0; i < 4; i++) {
        if (FUN_0046c7b0(i) != 0) {
          DAT_00690570 = FUN_00408ad0(i) * DAT_005cc328 + DAT_00690570;
          n++;
        }
      }
      if (n != 0) {
        DAT_00690570 = DAT_00690570 / (float)n;
      }
      if (DAT_00690570 > DAT_005cc320) {
        DAT_00690570 = 1.0f;
      }
      else if (DAT_00690570 < DAT_005d757c) {
        DAT_00690570 = 0.0f;
      }
      DAT_00690570 = DAT_00690570 + 0.05f;
      if (DAT_00690570 > DAT_005cc320) {
        DAT_00690570 = DAT_00690570 - DAT_005cc320;
      }
      FUN_005459c0(DAT_0068f618, 10, DAT_00690570, &DAT_00690480, 0);
      *(MatchV3_464e10 *)(DAT_0068f640[ch].p + 0x58) = DAT_00690480;
      if (*(int *)(DAT_0068f640[ch].p + 0x30) != 0) {
        FUN_005a6d60(*(int *)(DAT_0068f640[ch].p + 0x30), 0, 1, &DAT_00690480);
      }
      FUN_0045e0f0(ch, 1.0f);
    }
    ch = *(int *)(DAT_0088e670 + 0x590);
    DAT_006900d0++;
    if (DAT_006900d0 >= DAT_0068fccc) {
      ch += (int)FUN_00472650(0.0f, 2.0f);
      FUN_005a66d0(DAT_0068f640[ch].p, 1);
      *(float *)(DAT_0068f640[ch].p + 0x3c) = 22050.0f;
      if (*(int *)(DAT_0068f640[ch].p + 0x30) != 0) {
        FUN_005a6dc0(*(int *)(DAT_0068f640[ch].p + 0x30), 4, 1, 22050.0f);
      }
      FUN_0045e0f0(ch, DAT_005cc320 - FUN_00472650(0.0f, 0.2f));
      DAT_006900d0 = 0;
      DAT_0068fccc = (int)FUN_00472650(1000.0f, 3000.0f);
    }
    if (DAT_00690574 == 500) {
      ch = *(int *)(DAT_0088e670 + 0x740);
      FUN_005a66d0(DAT_0068f640[ch].p, 1);
      *(float *)(DAT_0068f640[ch].p + 0x3c) = 22050.0f;
      if (*(int *)(DAT_0068f640[ch].p + 0x30) != 0) {
        FUN_005a6dc0(*(int *)(DAT_0068f640[ch].p + 0x30), 4, 1, 22050.0f);
      }
      FUN_0045e0f0(ch, 1.0f);
      loc.x = -23.0f;
      loc.y = 0.0f;
      loc.z = -53.0f;
      *(MatchV3_464e10 *)(DAT_0068f640[ch].p + 0x58) = loc;
      if (*(int *)(DAT_0068f640[ch].p + 0x30) != 0) {
        FUN_005a6d60(*(int *)(DAT_0068f640[ch].p + 0x30), 0, 1, &loc);
      }
      DAT_00690574 = 0;
    }
    DAT_00690574++;
  }
}
