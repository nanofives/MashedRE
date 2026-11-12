// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00442e00: 65.1% (regs 65.7%, ops 94.2%) by hand (h7). Raw does not compile. eye/target are RwV3d
   passed by value; target is overwritten with eye - target for FUN_004c3ac0 (result discarded). sin/atan are the
   x87 intrinsics; the atan term is 180.0f - (float)(-(atan(n / d) * 57.2958f)) (fchs, then a float 180).
   Constants are float literals. The fov product needs its own temp (else the compiler factors param_8).
   Residual (b): the original frame is 8 bytes with dz, |dxz| and its sqrt packed into param_1's dead stack
   slot and the wobble temp in param_8's slot; ours keeps 3 locals (0xc), which shifts every esp offset. The
   pitch wobble keeps (float)DAT_007f101c and the pitch on the x87 stack in the original; ours spills both.
   Tried: inline helpers for the angle/sqrt (not inlined, worse), reusing dz for |dxz|, reusing param_8 for the
   wobble temp, 6 forms of the fov expression, 5 forms of the atan term, declaration order (no effect).
   PROV-DIVERGED-18 (2026-11-11): the sway step 0.004291845f compiled to 0x3b8ca29b; the original's
   0x005ce02c is 0x3b8ca29c (1 ULP, 0x00442fe8). Now reads the original's word. */
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
MGLOBAL(const float, DAT_005ce02c, 0x005ce02c);
typedef struct {
  float x;
  float y;
  float z;
} V3_00442e00;
typedef struct {
  char pad0[0x34];
  float pitch;
  float yaw;
  float roll;
  V3_00442e00 at;
  char pad4c[0x58 - 0x4c];
  float fov;
  char pad5c[0x964 - 0x5c];
  V3_00442e00 target;
  char pad970[0x97c - 0x970];
  V3_00442e00 eye;
} Cam_00442e00;
MGLOBAL(int, DAT_007f101c, 0x007f101c);
#ifdef __cplusplus
extern "C" {
#endif
double sin(double p0);
double atan(double p0);
#ifdef __cplusplus
}
#endif
#pragma intrinsic(sin, atan)
float FUN_004c3ac0(V3_00442e00 *p0);
float FUN_004c3b30(float p0);
float FUN_00472650(float p0, float p1);
void FUN_00441760(Cam_00442e00 *p0);
// MATCH: 0x00442e00 FUN_00442e00
void FUN_00442e00(Cam_00442e00 *param_1, V3_00442e00 eye, V3_00442e00 target, float param_8, float param_9)
{
  float dx;
  float dy;
  float dz;
  float a;
  float w;

  dx = eye.x - target.x;
  dy = eye.y - target.y;
  dz = eye.z - target.z;
  param_1->target = target;
  param_1->eye = eye;
  param_1->at.x = target.x;
  param_1->at.y = target.y;
  param_1->at.z = target.z;
  target.x = dx;
  target.y = dy;
  target.z = dz;
  FUN_004c3ac0(&target);
  w = ((float)sin(DAT_007f101c * 0.05f) + 1.0f) * param_8 * param_9 * 0.05f;
  param_1->fov = w + param_8;
  if (dz != 0.0f) {
    a = 180.0f - (float)(-(atan(dx / dz) * 57.2958f));
    if (dz > 0.0f) {
      a += 180.0f;
    }
  } else if (dx < 0.0f) {
    a = 270.0f;
  } else {
    a = 90.0f;
  }
  param_1->yaw = a;
  dz = dx * dx + dz * dz;
  if (dz != 0.0f && (dz = FUN_004c3b30(dz)) != 0.0f) {
    a = 180.0f - (float)(-(atan(dy / dz) * 57.2958f));
    if (dz > 0.0f) {
      a += 180.0f;
    }
  } else if (dy < 0.0f) {
    a = 270.0f;
  } else {
    a = 90.0f;
  }
  param_1->pitch = 360.0f - a;
  param_8 = (float)sin(0.05f * DAT_007f101c) + (float)sin(DAT_007f101c * 0.012820513f);
  param_1->pitch += (param_8 * 0.5f + (float)sin(DAT_007f101c * DAT_005ce02c)) * 0.5f * param_9;
  param_8 = (float)sin(0.1f * DAT_007f101c) * 0.125f + (float)sin(DAT_007f101c * 0.012048192f);
  param_1->roll = 0.0f;
  param_1->yaw += ((float)sin((FUN_00472650(0.0f, 60.0f) + DAT_007f101c) * 0.016666668f) * 0.0625f + param_8) * param_9;
  FUN_00441760(param_1);
}
