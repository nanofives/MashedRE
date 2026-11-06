// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h2 0x0048f290: 100% (regs 100%), whole symbol byte-identical. Hand rewrite (raw 40.6%): 100-entry array of 0x24-byte records at 0x00770718 (field names mechanical), byte min-search on +0x1f, FUN_00472650 cdecl float(float,float), the float->byte store goes through _ftol (0x004a2c48, compiler-emitted). Store order that matches: f18 before r0/r1, f20 before active = 1 (92% -> 100%). Residual: none. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef struct {
  int active;
  float x, y, z;
  float r0, r1;
  float f18;
  unsigned char c0, c1, c2, c3;
  float f20;
} Rec24b;
MGLOBALA(Rec24b, DAT_00770718, 100, 0x00770718);
MGLOBAL(float, DAT_005cc9d0, 0x005cc9d0);
MGLOBAL(float, DAT_005cc950, 0x005cc950);
float FUN_00472650(float lo, float hi);
// MATCH: 0x0048f290 FUN_0048f290
void FUN_0048f290(float *pos, float r, float f18, unsigned char *rgb)
{
  int i;
  int best = 0;
  int count = 0;
  unsigned char bestA = 0xff;
  for (i = 0; i < 100; i++) {
    if (DAT_00770718[i].active == 0) {
      DAT_00770718[i].x = pos[0];
      DAT_00770718[i].y = pos[1];
      DAT_00770718[i].z = pos[2];
      DAT_00770718[i].f18 = f18;
      DAT_00770718[i].r0 = r;
      DAT_00770718[i].r1 = r;
      DAT_00770718[i].c0 = rgb[0];
      DAT_00770718[i].c1 = rgb[1];
      DAT_00770718[i].c2 = rgb[2];
      DAT_00770718[i].c3 = (unsigned char)(FUN_00472650(0.0f, 128.0f) + DAT_005cc9d0);
      DAT_00770718[i].f20 = FUN_00472650(0.0f, 0.25f) + DAT_005cc950;
      DAT_00770718[i].active = 1;
      break;
    }
    count++;
    if (DAT_00770718[i].c3 < bestA) {
      best = i;
      bestA = DAT_00770718[i].c3;
    }
  }
  if (count == 100) {
    DAT_00770718[best].x = pos[0];
    DAT_00770718[best].y = pos[1];
    DAT_00770718[best].z = pos[2];
    DAT_00770718[best].f18 = f18;
    DAT_00770718[best].r0 = r;
    DAT_00770718[best].r1 = r;
    DAT_00770718[best].c0 = rgb[0];
    DAT_00770718[best].c1 = rgb[1];
    DAT_00770718[best].c2 = rgb[2];
    DAT_00770718[best].c3 = (unsigned char)(FUN_00472650(0.0f, 128.0f) + DAT_005cc9d0);
    DAT_00770718[best].f20 = FUN_00472650(0.0f, 0.25f) + DAT_005cc950;
    DAT_00770718[best].active = 1;
  }
}
