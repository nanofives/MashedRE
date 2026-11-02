// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00441990: 100% by hand (raw 26.1%). Same angle code as 0x00442e00 / 0x004233e0: intrinsic atan
   (fld1/fpatan), 57.2958f promoted to the double at 0x005ccae0, negate-then-subtract as two statements (fchs;
   fsubr), `x < 0 ? 270 : 90` arms; the other constants are .rdata literals (0.0 0x005d757c, 90 0x005ccad0,
   180 0x005cd09c, 270 0x005cd324, 360 0x005ccac4). The horizontal length reuses dz (the original stores it in
   dz's stack slot); a separate variable takes a fifth slot. */
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
typedef struct { float x, y, z; } V3_00441990;
typedef struct {
  char pad0[0x34];
  float pitch;
  float yaw;
  float roll;
  V3_00441990 at;
  char pad4c[0x58 - 0x4c];
  float fov;
  char pad5c[0x84 - 0x5c];
  int frame;
} Cam_00441990;
MGLOBAL(float, DAT_007f1a58, 0x007f1a58);
MGLOBAL(float, DAT_007f1a5c, 0x007f1a5c);
#ifdef __cplusplus
extern "C" {
#endif
double atan(double p0);
#ifdef __cplusplus
}
#endif
#pragma intrinsic(atan)
int FUN_004c1c10(int p0, int p1);
float FUN_004c3ac0(V3_00441990 *p0);
float FUN_004c3b30(float p0);
void FUN_00441760(Cam_00441990 *p0);
// MATCH: 0x00441990 FUN_00441990
void FUN_00441990(Cam_00441990 *param_1)
{
  float dx;
  float dy;
  float dz;
  float a;
  V3_00441990 v;

  dx = DAT_007f1a58;
  dz = DAT_007f1a5c;
  param_1->at.x = dx;
  param_1->at.y = 15.0f;
  param_1->at.z = dz;
  FUN_004c1c10(param_1->frame, 2);
  dx = dx - param_1->at.x;
  param_1->fov = 4.0f;
  dy = -param_1->at.y;
  dz = dz - param_1->at.z;
  v.x = dx;
  v.y = dy;
  v.z = dz;
  FUN_004c3ac0(&v);
  if (dz != 0.0f) {
    a = -(atan(dx / dz) * 57.2958f);
    a = 180.0f - a;
    if (dz > 0.0f) {
      a += 180.0f;
    }
  } else if (dx < 0.0f) {
    a = 270.0f;
  } else {
    a = 90.0f;
  }
  param_1->yaw = a;
  dz = dz * dz + dx * dx;
  if (dz != 0.0f && (dz = FUN_004c3b30(dz)) != 0.0f) {
    a = -(atan(dy / dz) * 57.2958f);
    a = 180.0f - a;
    if (dz > 0.0f) {
      a += 180.0f;
    }
  } else if (dy < 0.0f) {
    a = 270.0f;
  } else {
    a = 90.0f;
  }
  param_1->roll = 0.0f;
  param_1->pitch = 360.0f - a;
  FUN_00441760(param_1);
}
