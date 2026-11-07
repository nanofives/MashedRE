// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b53b0 (h3): hand, best 45.4% (regs 45.4%). Bounding sphere: centre from FUN_004b4550, radius =
   FUN_004c3b30(max squared distance over the n vertices). DAT_005d757c holds 0.0f (.rdata). Frame, call setup,
   remainder loop and tail match. Residual (a): the original unrolls the max loop x4 (cmp n,4 / ebx=3 / ecx+=0x30),
   cl 13.00.9176 unrolls it x8 in every form tried: indexed / pointer-walk / while / for-pointer loops, struct temp vs
   scalar temps, dot as inline helper, Sub+Dot helpers, ternary max, both subtraction directions x 6 dot orders,
   unsigned index, /G5 /G6 /G7 /Ox /Ob2 /Op, permuter (no gain). Same x4-vs-x8 split recorded for 0x00401690 and
   0x00475ab0 in earlier slices. Also the x87 operand order of the x/y subtractions (original fld v; fsubr s). */
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
typedef struct { float x, y, z; } V3_4b53b0;
typedef struct { V3_4b53b0 center; float radius; } Sphere_4b53b0;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
void FUN_004b4550(Sphere_4b53b0 *s, const V3_4b53b0 *v, int n);
float FUN_004c3b30(float x);
// MATCH: 0x004b53b0 FUN_004b53b0
void FUN_004b53b0(Sphere_4b53b0 *param_1, const V3_4b53b0 *param_2, int param_3)
{
  int i;
  float maxd;
  float d;
  V3_4b53b0 t;

  FUN_004b4550(param_1, param_2, param_3);
  maxd = DAT_005d757c;
  for (i = 0; i < param_3; i++) {
    float dx = param_2[i].x - param_1->center.x;
    float dy = param_2[i].y - param_1->center.y;
    float dz = param_2[i].z - param_1->center.z;
    d = dx * dx + dy * dy + dz * dz;
    if (d > maxd) {
      maxd = d;
    }
  }
  param_1->radius = FUN_004c3b30(maxd);
}
