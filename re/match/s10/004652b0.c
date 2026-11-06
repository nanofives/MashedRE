// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x004652b0 (h8): 100% by hand (raw 55.9%), relocheck OK. What moved it: the additive/multiplicative
   .rdata constants written as literals (2.5f, 0.125f, 0.25f, 27.0f, 30.0f, 0.6f: as globals cl loads the
   constant first, fld [c] / fadd [me.y]); the two products of the third channel stay globals (literals fold
   0.025f * 0.4f into one constant). cl swaps the 0x005cc9a4 / 0x005ccac0 product, so the source names them in
   the reverse of the emitted order (x * cac0 * c9a4 gives fmul [c9a4]; fmul [cac0]). The me.x test is
   if (me.x > -2.8) { if (me.z > -27) .. } else .. (the else-if form lays out the other way, 96.9%). */
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
typedef struct { float x, y, z; } MatchV3_4652b0;
typedef struct { char *p; float f; } MatchRec0068f640;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(char *, DAT_0088e670, 0x0088e670);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(int, DAT_0068f618, 0x0068f618);
MGLOBAL(double, DAT_005ce9d8, 0x005ce9d8);
MGLOBAL(float, DAT_005cc9a4, 0x005cc9a4);
MGLOBAL(float, DAT_005ccac0, 0x005ccac0);
MGLOBAL(float, DAT_005cc9f0, 0x005cc9f0);
MGLOBAL(float, DAT_005ce9d4, 0x005ce9d4);
MGLOBAL(float, DAT_005ce9d0, 0x005ce9d0);
MGLOBAL(float, DAT_00690578, 0x00690578);
MGLOBAL(MatchV3_4652b0, DAT_00690480, 0x00690480);
MGLOBALA(MatchRec0068f640, DAT_0068f640, 1, 0x0068f640);
void FUN_004417e0(MatchV3_4652b0 *out);
void FUN_0045e0f0(int p0, float p1);
void FUN_005459c0(int a, int b, float c, MatchV3_4652b0 *v, int d);
void FUN_005a66d0(char *p, int a);
void FUN_005a6d60(int h, int a, int b, MatchV3_4652b0 *v);
void FUN_005a6dc0(int h, int a, int b, float f);
// MATCH: 0x004652b0 FUN_004652b0
void FUN_004652b0(void)
{
  int ch;
  float v;
  float vol;
  MatchV3_4652b0 me;

  if (DAT_0068f4c8 == 0 || DAT_008aa254 <= 1) {
    FUN_004417e0(&me);
    ch = *(int *)(DAT_0088e670 + 0x470);
    if ((*(unsigned char *)(DAT_0068f640[ch].p + 0x34) & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[ch].p, 1);
      *(float *)(DAT_0068f640[ch].p + 0x3c) = 22050.0f;
      if (*(int *)(DAT_0068f640[ch].p + 0x30) != 0) {
        FUN_005a6dc0(*(int *)(DAT_0068f640[ch].p + 0x30), 4, 1, 22050.0f);
      }
    }
    v = (me.y + 2.5f) * 0.125f - 0.25f;
    if (v < DAT_005d757c) {
      v = DAT_005d757c;
    }
    else if (v > DAT_005cc320) {
      v = DAT_005cc320;
    }
    FUN_0045e0f0(ch, DAT_005cc320 - v);
    ch = *(int *)(DAT_0088e670 + 0x590);
    if ((*(unsigned char *)(DAT_0068f640[ch].p + 0x34) & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[ch].p, 1);
      *(float *)(DAT_0068f640[ch].p + 0x3c) = 22050.0f;
      if (*(int *)(DAT_0068f640[ch].p + 0x30) != 0) {
        FUN_005a6dc0(*(int *)(DAT_0068f640[ch].p + 0x30), 4, 1, 22050.0f);
      }
    }
    vol = (me.y + 2.5f) * 0.125f;
    if (vol < DAT_005d757c) {
      vol = 0.0f;
    }
    else if (vol > DAT_005cc320) {
      vol = 1.0f;
    }
    FUN_0045e0f0(ch, vol);
    ch = *(int *)(DAT_0088e670 + 0x500);
    if ((*(unsigned char *)(DAT_0068f640[ch].p + 0x34) & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[ch].p, 1);
      *(float *)(DAT_0068f640[ch].p + 0x3c) = 22050.0f;
      if (*(int *)(DAT_0068f640[ch].p + 0x30) != 0) {
        FUN_005a6dc0(*(int *)(DAT_0068f640[ch].p + 0x30), 4, 1, 22050.0f);
      }
    }
    if (DAT_0068f618 != 0) {
      if (me.x > DAT_005ce9d8) {
        if (me.z > DAT_005ce9d4) {
          DAT_00690578 = DAT_005ce9d0 - (me.z + 27.0f) * DAT_005cc9f0 * DAT_005ce9d0;
        }
      }
      else {
        DAT_00690578 = (me.z + 30.0f) * DAT_005ccac0 * DAT_005cc9a4 + 0.6f;
      }
      if (DAT_00690578 > DAT_005cc320) {
        DAT_00690578 = 1.0f;
      }
      else if (DAT_00690578 < DAT_005d757c) {
        DAT_00690578 = 0.0f;
      }
      FUN_005459c0(DAT_0068f618, 10, DAT_00690578, &DAT_00690480, 0);
      *(MatchV3_4652b0 *)(DAT_0068f640[ch].p + 0x58) = DAT_00690480;
      if (*(int *)(DAT_0068f640[ch].p + 0x30) != 0) {
        FUN_005a6d60(*(int *)(DAT_0068f640[ch].p + 0x30), 0, 1, &DAT_00690480);
      }
    }
    FUN_0045e0f0(ch, 1.0f);
  }
}
