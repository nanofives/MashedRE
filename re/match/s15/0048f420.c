// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h2 0x0048f420: 100% (regs 100%), whole symbol byte-identical. Hand rewrite of the raw converter (44.1%): 40-entry array of 0x20-byte records at 0x00766a00 (field names mechanical), FUN_00472650 as cdecl float(float,float) (caller pops 0x10 for two calls), y stored before the kind branch, count declared before bestAge (99.0% -> 100%). Residual: none. */
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
  int age;
  int u1c;
} Rec20;
MGLOBALA(Rec20, DAT_00766a00, 40, 0x00766a00);
float FUN_00472650(float lo, float hi);
// MATCH: 0x0048f420 FUN_0048f420
void FUN_0048f420(float *pos, float r, int kind)
{
  int i;
  int best = 0;
  int count = 0;
  int bestAge = 0;
  for (i = 0; i < 40; i++) {
    if (DAT_00766a00[i].active == 0) {
      DAT_00766a00[i].y = pos[1];
      if (kind) {
        DAT_00766a00[i].x = FUN_00472650(-0.1f, 0.1f) + pos[0];
        DAT_00766a00[i].z = FUN_00472650(-0.1f, 0.1f) + pos[2];
      } else {
        DAT_00766a00[i].x = pos[0];
        DAT_00766a00[i].z = pos[2];
      }
      DAT_00766a00[i].r0 = r;
      DAT_00766a00[i].r1 = r;
      DAT_00766a00[i].age = 0;
      DAT_00766a00[i].u1c = 0;
      if (kind) {
        DAT_00766a00[i].active = kind;
      } else {
        DAT_00766a00[i].active = 1;
      }
      break;
    }
    count++;
    if (DAT_00766a00[i].age > bestAge) {
      best = i;
      bestAge = DAT_00766a00[i].age;
    }
  }
  if (count == 40) {
    DAT_00766a00[best].y = pos[1];
    if (kind) {
      DAT_00766a00[best].x = FUN_00472650(-0.1f, 0.1f) + pos[0];
      DAT_00766a00[best].z = FUN_00472650(-0.1f, 0.1f) + pos[2];
    } else {
      DAT_00766a00[best].x = pos[0];
      DAT_00766a00[best].z = pos[2];
    }
    DAT_00766a00[best].r0 = r;
    DAT_00766a00[best].r1 = r;
    DAT_00766a00[best].age = 0;
    DAT_00766a00[best].u1c = 0;
    if (kind) {
      DAT_00766a00[best].active = kind;
    } else {
      DAT_00766a00[best].active = 1;
    }
  }
}
