// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00495fe0 (h2): FUN_00495fe0 100% (regs 100%). This file also holds its callee FUN_00495870 (0x00495870, the fix_joypad address; matched against the stock bytes) as a static function before it: the JoyDev pointer arrives in ESI (pilot finding 1). FUN_00495870 best 98.4% (regs 98.4%, ops 99.7%): JoyDev 0x448-byte struct (stick/stick2/hist/buttons histories at +0x2c8/+0x348/+0x3c8/+0x408, 8-entry averages at +0x248/+0x288), DIJOYSTATE local, POV switch on a signed long, deadzone read once into a local, squared length through a by-value V2 __inline helper (the int-register copy), field-wise avg[0] store. Residual (b): one fxch st(1) before the y*scale multiply in the normalise branch (x87 operand order); tried x*s/y*s, s*y, y*=s, a separate s local, sqrt then 1/s, inline 1/sqrt per component, two permuter runs (their 98.9% best moves the stick.y store after the length read, which changes behaviour, so it was rejected). */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
double fabs(double x);
double sqrt(double x);
#pragma intrinsic(fabs, sqrt)
typedef struct V2 { float x, y; } V2;
typedef struct JoyDev {
  int *dev;                      /* +0x000 IDirectInputDevice8 */
  undefined4 pad004[0x91];       /* +0x004 */
  V2 avg[8];                     /* +0x248 */
  V2 avg2[8];                    /* +0x288 */
  V2 stick[16];                  /* +0x2c8 */
  V2 stick2[16];                 /* +0x348 */
  undefined4 hist3c8[16];        /* +0x3c8 */
  unsigned int buttons[16];      /* +0x408 */
} JoyDev;
typedef struct DIJOYSTATE {
  long lX, lY, lZ, lRx, lRy, lRz;
  long rglSlider[2];
  long rgdwPOV[4];
  unsigned char rgbButtons[32];
} DIJOYSTATE;
typedef long (__stdcall *ComFn)(int *self);
typedef long (__stdcall *GetDeviceStateFn)(int *self, unsigned long n, void *data);
MGLOBAL(int, DAT_00772fac, 0x00772fac);
MGLOBALA(JoyDev, DAT_00771e88, 4, 0x00771e88);
MGLOBAL(float, DAT_00772fa8, 0x00772fa8);
void FUN_004b6480(void *p0, unsigned int p1);
static __inline float V2LenSq(V2 v)
{
  return v.x * v.x + v.y * v.y;
}
// MATCH: 0x00495870 FUN_00495870
static void FUN_00495870(JoyDev *d)
{
  int *dev;
  int i;
  long hr;
  float x;
  float y;
  float mag;
  float dz;
  DIJOYSTATE st;

  dev = d->dev;
  if ((*(ComFn *)(*dev + 0x64))(dev) < 0) {
    hr = (*(ComFn *)(*dev + 0x1c))(dev);
    while (hr == (long)0x8007001e) {
      hr = (*(ComFn *)(*dev + 0x1c))(dev);
    }
  }
  if ((*(GetDeviceStateFn *)(*dev + 0x24))(dev, 0x50, &st) < 0) {
    return;
  }
  for (i = 15; i >= 1; i--) {
    d->buttons[i] = d->buttons[i - 1];
    d->hist3c8[i] = d->hist3c8[i - 1];
    d->stick[i] = d->stick[i - 1];
    d->stick2[i] = d->stick2[i - 1];
  }
  d->hist3c8[0] = 0;
  d->stick[0].y = 0;
  d->stick[0].x = 0;
  d->stick2[0].y = 0;
  d->stick2[0].x = 0;
  FUN_004b6480(&d->buttons[0], 0x20);
  for (i = 0; i < 16; i++) {
    if (st.rgbButtons[i] & 0x80) {
      ((unsigned char *)&d->buttons[0])[i >> 3] |= (unsigned char)(1 << (i & 7));
    }
  }
  d->stick[0].x = (float)(st.lX >> 12) * 0.13333334f - 1.0f;
  d->stick[0].y = 1.0f - (float)(st.lY >> 12) * 0.13333334f;
  if ((unsigned short)st.rgdwPOV[0] != 0xffff) {
    switch (st.rgdwPOV[0]) {
    case 0:
      d->stick[0].x = 0.0f;
      d->stick[0].y = 1.0f;
      break;
    case 4500:
      d->stick[0].x = 1.0f;
      d->stick[0].y = 1.0f;
      break;
    case 9000:
      d->stick[0].x = 1.0f;
      d->stick[0].y = 0.0f;
      break;
    case 13500:
      d->stick[0].x = 1.0f;
      d->stick[0].y = -1.0f;
      break;
    case 18000:
      d->stick[0].x = 0.0f;
      d->stick[0].y = -1.0f;
      break;
    case 22500:
      d->stick[0].x = -1.0f;
      d->stick[0].y = -1.0f;
      break;
    case 27000:
      d->stick[0].x = -1.0f;
      d->stick[0].y = 0.0f;
      break;
    case 31500:
      d->stick[0].x = -1.0f;
      d->stick[0].y = 1.0f;
      break;
    }
  }
  dz = DAT_00772fa8;
  x = (float)fabs(d->stick[0].x) < dz ? 0.0f : d->stick[0].x;
  d->stick[0].x = x;
  y = (float)fabs(d->stick[0].y) < dz ? 0.0f : d->stick[0].y;
  mag = V2LenSq(d->stick[0]);
  d->stick[0].y = y;
  if (mag > 1.0f) {
    mag = 1.0f / (float)sqrt(mag);
    d->stick[0].x = x * mag;
    d->stick[0].y = mag * y;
  }
  for (i = 7; i > 0; i--) {
    d->avg[i] = d->avg[i - 1];
    d->avg2[i] = d->avg2[i - 1];
  }
  d->avg[0].x = d->stick[0].x;
  d->avg[0].y = d->stick[0].y;
  d->stick[0].x = 0.0f;
  d->stick[0].y = 0.0f;
  d->stick[0] = d->avg[0];
  for (i = 1; i < 8; i++) {
    d->stick[0].x += d->avg[i].x;
    d->stick[0].y += d->avg[i].y;
  }
  d->stick[0].x *= 0.125f;
  d->stick[0].y *= 0.125f;
}

// MATCH: 0x00495fe0 FUN_00495fe0
void FUN_00495fe0(void)
{
  int i;
  JoyDev *d;

  i = 0;
  if (0 < DAT_00772fac) {
    d = DAT_00771e88;
    do {
      FUN_00495870(d);
      i++;
      d++;
    } while (i < DAT_00772fac);
  }
}
