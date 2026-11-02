// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00443300: 100% by hand (h7). Raw 28.6%. A local index i = param_2 - 1 (reusing param_2 gives
   dec; jns, 92%). Catmull-Rom over 4 wrapped points of a 64-entry
   {x, z} array (count at +0x200); t clamped to [0, 1]. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
  float x;
  float z;
} Pt_00443300;
typedef struct {
  Pt_00443300 pt[64];
  int count;
} Path_00443300;
// MATCH: 0x00443300 FUN_00443300
void FUN_00443300(Path_00443300 *param_1, int param_2, float param_3, float *param_4)
{
  Pt_00443300 *p0;
  Pt_00443300 *p1;
  Pt_00443300 *p2;
  Pt_00443300 *p3;
  float t;
  int i;
  float t2;
  float t3;

  i = param_2 - 1;
  if (i < 0) {
    i += param_1->count;
  }
  p0 = &param_1->pt[i];
  i++;
  if (i >= param_1->count) {
    i -= param_1->count;
  }
  p1 = &param_1->pt[i];
  i++;
  if (i >= param_1->count) {
    i -= param_1->count;
  }
  p2 = &param_1->pt[i];
  i++;
  if (i >= param_1->count) {
    i -= param_1->count;
  }
  p3 = &param_1->pt[i];
  t = param_3;
  if (t > 1.0f) {
    t = 1.0f;
  } else if (t < 0.0f) {
    t = 0.0f;
  }
  t2 = t * t;
  t3 = t2 * t;
  param_4[0] = 0.5f * (p1->x + p1->x + (p2->x - p0->x) * t +
                       (p0->x + p0->x - p1->x * 5.0f + p2->x * 4.0f - p3->x) * t2 +
                       (p1->x * 3.0f - p0->x - p2->x * 3.0f + p3->x) * t3);
  param_4[1] = 0.5f * (p1->z + p1->z + (p2->z - p0->z) * t +
                       (p0->z + p0->z - p1->z * 5.0f + p2->z * 4.0f - p3->z) * t2 +
                       (p1->z * 3.0f - p0->z - p2->z * 3.0f + p3->z) * t3);
}
