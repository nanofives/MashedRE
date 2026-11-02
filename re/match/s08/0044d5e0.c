// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044d5e0: 100% by hand (raw 48.7%). Ghidra dropped the struct copy: the param is a 3-float
   struct copied whole into a local, then .y -= DAT_005cc9c0. The loop is an int index 0..7 (strength-reduced to
   a pointer, hence the signed jl against 0x684690) over an 8 x 0x48 array of six 3-float vertices at 0x006848dc
   (the compiler keeps &v[1] = 0x006848e8 in edi). */
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
typedef struct { float x, y, z; } V3_0044d5e0;
typedef struct { V3_0044d5e0 v[6]; } Quad_0044d5e0;
MGLOBAL(int, DAT_00684b34, 0x00684b34);
MGLOBAL(float, DAT_005cc9c0, 0x005cc9c0);
MGLOBALA(int, DAT_00684670, 8, 0x00684670);
MGLOBALA(Quad_0044d5e0, DAT_006848dc, 8, 0x006848dc);
MGLOBAL(V3_0044d5e0, DAT_00684b1c, 0x00684b1c);
int FUN_004c18c0(V3_0044d5e0 *p0, V3_0044d5e0 *p1);
int FUN_00547230(V3_0044d5e0 *p0, V3_0044d5e0 *p1, V3_0044d5e0 *p2, V3_0044d5e0 *p3, V3_0044d5e0 *p4, float *p5);
// MATCH: 0x0044d5e0 FUN_0044d5e0
int FUN_0044d5e0(V3_0044d5e0 *param_1)
{
  float out;
  V3_0044d5e0 a;
  V3_0044d5e0 b;
  int i;

  b.x = 0.0f;
  b.y = 0.0f;
  b.z = 0.0f;
  out = 0.0f;
  if (DAT_00684b34 != 0) {
    a = *param_1;
    a.y = a.y - DAT_005cc9c0;
    b.y = 0.2f;
    if (FUN_004c18c0(&DAT_00684b1c, param_1) != 0) {
      for (i = 0; i < 8; i++) {
        if (DAT_00684670[i] == 0) {
          if (FUN_00547230(&a, &b, &DAT_006848dc[i].v[0], &DAT_006848dc[i].v[1], &DAT_006848dc[i].v[2], &out) != 0) {
            return 0;
          }
          if (FUN_00547230(&a, &b, &DAT_006848dc[i].v[3], &DAT_006848dc[i].v[4], &DAT_006848dc[i].v[5], &out) != 0) {
            return 0;
          }
        }
      }
      return 1;
    }
  }
  return 0;
}
