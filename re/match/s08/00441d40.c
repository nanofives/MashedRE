// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00441d40 (1730 B): best 69.2% (regs 74.2%, ops 94.8%), 445/444 ins, by hand + permuter (raw 23.7%). Shape: dt from the
   int frame counter, target from FUN_00441c80, d = target - pos and the normalised pos - target (FUN_004c3ac0's
   length feeds +0xc0), turn limit min(+0xc0 * 25, 1), yaw angle by the atan code of 0x00441990 (same .rdata
   literals), the wrap / proportional rate update written out twice (yaw then pitch), the 0x00408a50 range test
   for +0xc8, identity + three rotations of the +0x4c matrix, and the eye offset {0, 0.1741f, 0.2713f} through a
   local identity matrix (the target vector is reused for it, as in the original's frame). `t = s` reproduces
   the original's copy of the step before each update. Tried: __inline helpers for the update (not inlined by
   9176, 29-30%), __forceinline (67.3%), operand orders of the formula. Permuter (2480 compiles) reached 77.5%
   only through reorders that change behaviour (normalise before n is set, step copied before it is computed,
   offset added before the transform): rejected; its behaviour-neutral moves are kept (declaration order, v = 0
   before the call, fd4 > r, rate * damp, identity before the flags).
   Residual (b): the original keeps the wrapped angle on the x87 stack through the rate formula and pops it at
   the join (fld lim; fadd st(1)), 9176 here consumes it (fadd [lim]); frame layout differs by the step copy slot
   (0x80 vs 0x84); the order of the d / n subtractions against the pos copy. */
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
typedef struct { float x, y, z; } V3_00441d40;
typedef struct {
  V3_00441d40 right; unsigned int flags;
  V3_00441d40 up; unsigned int pad1;
  V3_00441d40 at; unsigned int pad2;
  V3_00441d40 pos; unsigned int pad3;
} Mtx_00441d40;
typedef struct {
  char pad0[8];
  int mode;
  char padc[0x3c - 0xc];
  V3_00441d40 pos;
  int pad48;
  Mtx_00441d40 mtx;
  char pad8c[0xac - 0x8c];
  float pitch;
  float yaw;
  float roll;
  float pitch_rate;
  float yaw_rate;
  float fc0;
  float fc4;
  float fc8;
  int padcc;
  float fd0;
  float fd4;
} Cam_00441d40;
typedef struct {
  char pad0[0x20];
  int car;
} Tgt_00441d40;
MGLOBAL(int, DAT_007f1008, 0x007f1008);
MGLOBALA(float, DAT_006146fc, 3, 0x006146fc);
#ifdef __cplusplus
extern "C" {
#endif
double atan(double p0);
#ifdef __cplusplus
}
#endif
#pragma intrinsic(atan)
float FUN_00408a50(int p0);
void FUN_00441c80(V3_00441d40 *p0, Tgt_00441d40 *p1);
float FUN_004c3ac0(V3_00441d40 *p0);
float FUN_004c3b30(float p0);
int FUN_004c3df0(V3_00441d40 *p0, V3_00441d40 *p1, int p2, Mtx_00441d40 *p3);
int FUN_004c4d20(Mtx_00441d40 *p0, float *p1, float p2, int p3);
void FUN_004c51a0(Mtx_00441d40 *p0, V3_00441d40 *p1, int p2);
// MATCH: 0x00441d40 FUN_00441d40
void FUN_00441d40(Cam_00441d40 *param_1, Tgt_00441d40 *param_2)
{
  float dt;
  float dz;
  float dx;
  float len;
  float dy;
  V3_00441d40 pos;
  float a;
  float s;
  float v;
  float t;
  float lim;
  float damp;
  float r;
  V3_00441d40 tgt;
  V3_00441d40 n;
  Mtx_00441d40 m;

  dt = (float)DAT_007f1008 * 0.00033333333f;
  FUN_00441c80(&tgt, param_2);
  pos = param_1->pos;
  dx = tgt.x - pos.x;
  dy = tgt.y - pos.y;
  dz = tgt.z - pos.z;
  n.x = pos.x - tgt.x;
  n.y = pos.y - tgt.y;
  n.z = pos.z - tgt.z;
  len = FUN_004c3ac0(&n);
  if (param_1->fc4 > 1000.0f) {
    param_1->fc0 = param_1->fc4 - 1000.0f;
  } else {
    len = 45.0f - len;
    if (len < 7.0f) {
      len = 7.0f;
    }
    param_1->fc0 = len / param_1->fc4;
  }
  lim = param_1->fc0 * 25.0f;
  if (lim > 1.0f) {
    lim = 1.0f;
  }
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
  s = dt * 7.5f;
  t = s;
  a = param_1->yaw - a;
  while (a >= 360.0f) {
    a -= 360.0f;
  }
  while (a < 0.0f) {
    a += 360.0f;
  }
  if (a > 180.0f) {
    a = -(360.0f - a);
  }
  if (a > 0.0f) {
    if (a > lim) {
      param_1->yaw_rate = param_1->yaw_rate - t;
    } else {
      param_1->yaw_rate = (1.0f - dt * 16.0f) * param_1->yaw_rate - (1.0f - (lim - a) / lim) * t;
    }
  }
  if (a < 0.0f) {
    if (a < -lim) {
      param_1->yaw_rate = t + param_1->yaw_rate;
    } else {
      param_1->yaw_rate = (1.0f - (lim + a) / lim) * s + (1.0f - dt * 16.0f) * param_1->yaw_rate;
    }
  }
  if (param_1->mode != 1) {
    param_1->yaw = param_1->yaw_rate + param_1->yaw;
  }
  damp = 1.0f - dt * 4.0f;
  param_1->yaw_rate = damp * param_1->yaw_rate;
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
  t = s;
  a = param_1->pitch - (360.0f - a);
  while (a >= 360.0f) {
    a -= 360.0f;
  }
  while (a < 0.0f) {
    a += 360.0f;
  }
  if (a > 180.0f) {
    a = -(360.0f - a);
  }
  if (a > 0.0f) {
    if (a > lim) {
      param_1->pitch_rate = param_1->pitch_rate - t;
    } else {
      param_1->pitch_rate = (1.0f - dt * 16.0f) * param_1->pitch_rate - (1.0f - (lim - a) / lim) * t;
    }
  }
  if (a < 0.0f) {
    if (a < -lim) {
      param_1->pitch_rate = t + param_1->pitch_rate;
    } else {
      param_1->pitch_rate = (1.0f - (lim + a) / lim) * s + (1.0f - dt * 16.0f) * param_1->pitch_rate;
    }
  }
  s = 100.0f;
  v = 0.0f;
  r = FUN_00408a50(param_2->car);
  if (param_1->fd0 <= param_1->fd4) {
    if (r >= param_1->fd0 && param_1->fd4 > r) {
      goto skip;
    }
  } else if (r >= param_1->fd0 || r < param_1->fd4) {
    goto skip;
  }
  v = s;
skip:
  if (param_1->mode != 1) {
    param_1->pitch = param_1->pitch_rate + param_1->pitch;
  }
  param_1->pitch_rate = param_1->pitch_rate * damp;
  if (v < 0.0f) {
    v = -v;
  }
  param_1->fc8 = v;
  param_1->mtx.right.x = param_1->mtx.up.y = param_1->mtx.at.z = 1.0f;
  param_1->mtx.flags = param_1->mtx.flags | 0x20003;
  param_1->mtx.right.y = param_1->mtx.right.z = param_1->mtx.up.x = 0.0f;
  param_1->mtx.up.z = param_1->mtx.at.x = param_1->mtx.at.y = 0.0f;
  param_1->mtx.pos.x = param_1->mtx.pos.y = param_1->mtx.pos.z = 0.0f;
  FUN_004c4d20(&param_1->mtx, DAT_006146fc, param_1->yaw, 2);
  FUN_004c4d20(&param_1->mtx, &param_1->mtx.right.x, param_1->pitch, 2);
  FUN_004c4d20(&param_1->mtx, &param_1->mtx.at.x, param_1->roll, 2);
  if (param_1->mode != 1) {
    m.flags = m.flags | 0x20003;
    m.right.x = m.up.y = m.at.z = 1.0f;
    m.right.y = m.right.z = m.up.x = 0.0f;
    m.up.z = m.at.x = m.at.y = 0.0f;
    m.pos.x = m.pos.y = m.pos.z = 0.0f;
    FUN_004c4d20(&m, DAT_006146fc, param_1->yaw, 2);
    tgt.x = 0.0f;
    tgt.y = 0.1741f;
    tgt.z = 0.2713f;
    FUN_004c3df0(&tgt, &tgt, 1, &m);
    pos.x = pos.x + tgt.x;
    pos.y = pos.y + tgt.y;
    pos.z = pos.z + tgt.z;
  }
  FUN_004c51a0(&param_1->mtx, &pos, 2);
}
