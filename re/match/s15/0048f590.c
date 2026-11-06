// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h2 0x0048f590: 100% (regs 100%), whole symbol byte-identical. Rewritten by hand from the raw converter (33.8%): a 10-entry array of 0x24-byte records at 0x00769f50 (field names mechanical), for-loop with break, count/oldest tracking. Residual: none. */
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
  float c;
  int age;
  int u20;
} Rec24;
MGLOBALA(Rec24, DAT_00769f50, 10, 0x00769f50);
// MATCH: 0x0048f590 FUN_0048f590
void FUN_0048f590(float *pos, float r)
{
  int i;
  int best = 0;
  int bestAge = 0;
  int count = 0;
  for (i = 0; i < 10; i++) {
    if (DAT_00769f50[i].active == 0) {
      DAT_00769f50[i].x = pos[0];
      DAT_00769f50[i].y = pos[1];
      DAT_00769f50[i].z = pos[2];
      DAT_00769f50[i].r0 = r;
      DAT_00769f50[i].r1 = r;
      DAT_00769f50[i].age = 0;
      DAT_00769f50[i].c = 255.0f;
      DAT_00769f50[i].u20 = 0;
      DAT_00769f50[i].active = 1;
      break;
    }
    count++;
    if (DAT_00769f50[i].age > bestAge) {
      best = i;
      bestAge = DAT_00769f50[i].age;
    }
  }
  if (count == 10) {
    DAT_00769f50[best].x = pos[0];
    DAT_00769f50[best].y = pos[1];
    DAT_00769f50[best].z = pos[2];
    DAT_00769f50[best].r0 = r;
    DAT_00769f50[best].r1 = r;
    DAT_00769f50[best].age = 0;
    DAT_00769f50[best].c = 255.0f;
    DAT_00769f50[best].u20 = 0;
    DAT_00769f50[best].active = 1;
  }
}
