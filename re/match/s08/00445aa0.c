// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00445aa0: 67.4% (regs 71.6%, ops 90.8%) by hand (h8); raw did not compile (Ghidra typed the record
   as float). Structure first: record as two RwMatrix (+0x0c, pos at +0x3c; +0x4c) + velocity +0x8c + angles
   +0xa4..+0xc8, the target record (int active, x, h, z, x2, y2, z2), sin/atan intrinsics, 57.2958f times a double
   atan result (the original's qword constant is a float-rounded value), FUN_004c3bf0/c60/ac0/b30 returning float.
   Residual (b), out of budget: the original keeps tg in edi and &cam->pos in ebp (ours swap), holds `off` (0 / 7.0)
   in the dead param_2 slot and compares d2 against that slot (ours constant-propagates it, which also re-lays
   the active branch with the FUN_004430a0 loop inline instead of out of line), and spills one more frame slot
   (0x50 vs 0x4c).
   PROV-DIVERGED-18 (2026-11-11): three VALUE differences. (1) dt * 60 * e * (1/7) compiled to
   dt * e * (60/7) (folded literal 0x41092493); the original multiplies left to right through 0x005cc728 (60,
   0x00445d99) and 0x005ce188 (1/7, 0x00445dc0). (2) the sway angle step 0.0030679711f compiled to 0x3b491004; the
   original's 0x005ce184 is 0x3b490e56 (0x00445f4e). (3) the roll step 0.0061359424f compiled to 0x3bc91004; the
   original's 0x005ce17c is 0x3bc90e56 (0x004463d5). All three now read the original's words. */
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
MGLOBAL(const float, DAT_005cc728, 0x005cc728);
MGLOBAL(const float, DAT_005ce188, 0x005ce188);
MGLOBAL(const float, DAT_005ce184, 0x005ce184);
MGLOBAL(const float, DAT_005ce17c, 0x005ce17c);
typedef struct { float x; float y; float z; } V3_00445aa0;
typedef struct { float x; float z; } V2_00445aa0;
typedef struct {
  V3_00445aa0 right; unsigned int flags; V3_00445aa0 up; int pad1; V3_00445aa0 at; int pad2; V3_00445aa0 pos; int pad3;
} RwMatrix_00445aa0;
typedef struct {
  int pad0[3];               /* +0x00 */
  RwMatrix_00445aa0 m1;      /* +0x0c, pos at +0x3c */
  RwMatrix_00445aa0 m2;      /* +0x4c */
  V3_00445aa0 vel;           /* +0x8c */
  int pad98[3];              /* +0x98 */
  float a4;                  /* +0xa4 */
  int a8;                    /* +0xa8 */
  float ac;                  /* +0xac */
  float b0;                  /* +0xb0 */
  float b4;                  /* +0xb4 */
  float b8;                  /* +0xb8 */
  float bc;                  /* +0xbc */
  float c0;                  /* +0xc0 */
  float c4;                  /* +0xc4 */
  float c8;                  /* +0xc8 */
} Cam_00445aa0;
typedef struct { int active; float x; float h; float z; float x2; float y2; float z2; } Tgt_00445aa0;
MGLOBAL(int, DAT_007f1008, 0x007f1008);
MGLOBAL(int, DAT_007f101c, 0x007f101c);
MGLOBALA(char, DAT_007f1042, 0x260, 0x007f1042);
MGLOBALA(float, DAT_006146fc, 3, 0x006146fc);
double sin(double);
double atan(double);
#pragma intrinsic(sin, atan)
void FUN_00441c80(V3_00445aa0 *out, Tgt_00445aa0 *t);
int FUN_00426c00(void);
float FUN_004c3bf0(V2_00445aa0 *v);
float FUN_004c3c60(V2_00445aa0 *out, V2_00445aa0 *in);
float FUN_004c3ac0(V3_00445aa0 *v);
float FUN_004c3b30(float x);
void FUN_004430a0(int p0);
RwMatrix_00445aa0 *FUN_004c4d20(RwMatrix_00445aa0 *m, float *axis, float angle, int op);
RwMatrix_00445aa0 *FUN_004c51a0(RwMatrix_00445aa0 *m, V3_00445aa0 *v, int op);
// MATCH: 0x00445aa0 FUN_00445aa0
void FUN_00445aa0(Cam_00445aa0 *cam, Tgt_00445aa0 *tg)
{
  float dt;
  float lim;
  float dist;
  float acc;
  float damp;
  float dx;
  float dy;
  float lim2;
  float acc5;
  V3_00445aa0 *pd;
  V2_00445aa0 d;
  V2_00445aa0 dir;
  V3_00445aa0 target;
  V3_00445aa0 v;
  float dh;
  float f;
  float k;
  float off;
  float e;
  float d2;
  float s;
  float ex;
  float ey;
  float ez;
  float w;
  float dz;
  float sn;
  float ang;
  float x;
  float r;
  float l;
  float sum;
  float len;
  float roll;
  float m;
  int i;

  dt = (float)DAT_007f1008 * 0.00033333333f;
  FUN_00441c80(&target, tg);
  if (tg->active != 0) {
    target.x = tg->x;
    target.z = tg->z;
    d.x = cam->m1.pos.x - target.x;
    d.z = cam->m1.pos.z - target.z;
    k = FUN_004c3bf0(&d);
    if (k > 4.0f) {
      k = 4.0f;
    }
    cam->a4 = tg->h;
    target.x = target.x + k * 0.33333334f;
    target.z = k * 0.33333334f + target.z;
  } else {
    cam->a4 = 12.0f;
    if (FUN_00426c00() == 0x1a) {
      cam->a4 = 20.0f;
    }
  }
  dh = cam->m1.pos.y - cam->a4;
  if (dh < 0.05f) {
    cam->vel.y = dt * 3.0f + cam->vel.y;
  }
  if (dh > 0.05f) {
    cam->vel.y = cam->vel.y - dt * 3.0f;
  }
  damp = 1.0f - dt;
  f = damp;
  if (dh < 1.0f && dh > -1.0f) {
    f = 1.0f - (dt + dt);
  }
  cam->vel.y = f * cam->vel.y;
  pd = &cam->m1.pos;
  d.x = pd->x - target.x;
  d.z = cam->m1.pos.z - target.z;
  FUN_004c3c60(&dir, &d);
  d2 = FUN_004c3bf0(&d);
  dist = d2;
  if (cam->a8 == 0) {
    off = 0.0f;
    if (tg->active == 0) {
      off = 7.0f;
      if (d2 < off) {
        cam->a8 = 1;
      }
      s = dt * 14.0f;
    } else {
      if (d2 < 0.02f) {
        FUN_004430a0(0);
      }
      for (i = 0; i < 8; i++) {
        if (DAT_007f1042[i * 0x4c] != 0) {
          FUN_004430a0(0);
        }
      }
      s = dt * 6.0f;
    }
    e = d2 - off;
    if (tg->active != 0 && e < 0.75f) {
      e = 0.75f;
    }
    if (d2 < 0.0f) {
      d2 = 0.0f;
    }
    if (e < 0.0f) {
      e = 0.0f;
    } else if (e > 5.0f) {
      e = 5.0f;
    }
    s = s * e * 0.2f;
    cam->vel.x = cam->vel.x - dir.x * s;
    cam->vel.z = cam->vel.z - dir.z * s;
    if (cam->a8 == 0) {
      goto damp_xz;
    }
  }
  if (d2 > 10.0f) {
    cam->a8 = 0;
  }
  if (tg->active != 0) {
    cam->a8 = 0;
  }
  e = 7.0f - d2;
  if (e < 0.0f) {
    e = 0.0f;
  }
  s = dt * DAT_005cc728; s = s * e; s = s * DAT_005ce188;
  cam->vel.x = dir.x * s + cam->vel.x;
  cam->vel.z = dir.z * s + cam->vel.z;
damp_xz:
  f = damp;
  if (d2 < 10.0f && cam->a8 == 0) {
    f = 1.0f - (dt + dt);
  }
  cam->vel.x = f * cam->vel.x;
  cam->vel.z = f * cam->vel.z;
  v = cam->vel;
  v.x = v.x * dt;
  v.y = v.y * dt;
  v.z = v.z * dt;
  FUN_004c51a0(&cam->m1, &v, 2);
  if (tg->active != 0) {
    FUN_00441c80(&target, tg);
    if (dist < 8.0f) {
      ex = target.x - tg->x2;
      ey = target.y - tg->y2;
      ez = target.z - tg->z2;
      w = (8.0f - dist) * 0.125f;
      target.x = target.x - ex * w;
      target.y = target.y - ey * w;
      target.z = target.z - ez * w;
    }
  }
  dx = target.x - pd->x;
  dy = target.y - cam->m1.pos.y;
  dz = target.z - cam->m1.pos.z;
  sn = (float)sin((float)((DAT_007f101c - 0x200) & 0x3ff) * DAT_005ce184);
  if (sn < 0.0f) {
    sn = -sn;
  }
  if (sn < 0.2f) {
    sn = 0.2f - (0.2f - sn) * 0.25f;
  }
  cam->c0 = sn * 0.5f + 0.1f;
  if (tg->active != 0) {
    cam->c0 = 0.6f;
  }
  lim = cam->c0 * 15.0f;
  if (lim > 5.0f) {
    lim = 5.0f;
  }
  if (tg->active != 0 && dist < 4.0f) {
    lim = dist;
  }
  if (dz == 0.0f) {
    if (dx >= 0.0f) {
      ang = 90.0f;
    } else {
      ang = 270.0f;
    }
  } else {
    ang = 180.0f - -(float)(atan(dx / dz) * 57.2958f);
    if (dz > 0.0f) {
      ang = ang + 180.0f;
    }
  }
  acc5 = dt * 5.0f;
  acc = acc5;
  if (tg->active != 0 && dist > 2.0f) {
    acc = dt * 10.0f;
  }
  x = cam->b0 - ang;
  while (x >= 360.0f) {
    x = x - 360.0f;
  }
  while (x < 0.0f) {
    x = x + 360.0f;
  }
  if (x > 180.0f) {
    x = -(360.0f - x);
  }
  if (x > lim) {
    cam->bc = cam->bc - acc;
  } else {
    if (x < 0.0f) {
      x = -x;
    }
    if (x > lim) {
      cam->bc = acc + cam->bc;
    } else {
      cam->bc = cam->bc * 0.5f;
    }
  }
  r = x / lim;
  if (r < 4.0f) {
    r = 100.0f;
  } else if (r < 10.0f) {
    r = (10.0f - r) * 16.666666f;
  } else {
    r = 0.0f;
  }
  v.x = pd->x - target.x;
  v.y = cam->m1.pos.y - target.y;
  v.z = cam->m1.pos.z - target.z;
  l = FUN_004c3ac0(&v) - cam->a4;
  if (l < 0.0f) {
    l = -l;
  }
  m = r - (l + l);
  if (m < 0.0f) {
    m = 0.0f;
  }
  cam->c8 = m;
  f = damp;
  cam->b0 = cam->bc + cam->b0;
  lim2 = lim + lim;
  if (l < lim2) {
    f = 1.0f - dt * 4.0f;
  }
  cam->bc = f * cam->bc;
  sum = dz * dz + dx * dx;
  if (sum == 0.0f || (len = FUN_004c3b30(sum)) == 0.0f) {
    if (dy >= 0.0f) {
      ang = 90.0f;
    } else {
      ang = 270.0f;
    }
  } else {
    ang = 180.0f - -(float)(atan(dy / len) * 57.2958f);
    if (len > 0.0f) {
      ang = ang + 180.0f;
    }
  }
  acc = acc5;
  x = cam->ac - (360.0f - ang);
  while (x >= 360.0f) {
    x = x - 360.0f;
  }
  while (x < 0.0f) {
    x = x + 360.0f;
  }
  if (x > 180.0f) {
    x = -(360.0f - x);
  }
  if (x > lim) {
    cam->b8 = cam->b8 - acc;
  } else if (x < -lim) {
    cam->b8 = acc + cam->b8;
    x = -x;
  } else {
    cam->b8 = cam->b8 * 0.5f;
  }
  cam->ac = cam->b8 + cam->ac;
  f = damp;
  if (x < lim2) {
    f = 1.0f - dt * 4.0f;
  }
  cam->b8 = f * cam->b8;
  roll = (float)sin((float)(DAT_007f101c & 0x3ff) * DAT_005ce17c) * 30.0f;
  if (tg->active != 0 && dist < 4.0f) {
    m = dist - 1.0f;
    if (m < 0.0f) {
      m = 0.0f;
    }
    roll = roll * m * 0.33333334f;
  }
  cam->b4 = roll;
  cam->m2.at.z = 1.0f;
  cam->m2.up.y = 1.0f;
  cam->m2.right.x = 1.0f;
  cam->m2.up.x = 0.0f;
  cam->m2.right.z = 0.0f;
  cam->m2.right.y = 0.0f;
  cam->m2.at.y = 0.0f;
  cam->m2.at.x = 0.0f;
  cam->m2.up.z = 0.0f;
  cam->m2.pos.z = 0.0f;
  cam->m2.pos.y = 0.0f;
  cam->m2.pos.x = 0.0f;
  cam->m2.flags = cam->m2.flags | 0x20003;
  FUN_004c4d20(&cam->m2, DAT_006146fc, cam->b0, 2);
  FUN_004c4d20(&cam->m2, &cam->m2.right.x, cam->ac, 2);
  FUN_004c4d20(&cam->m2, &cam->m2.at.x, cam->b4, 2);
  FUN_004c51a0(&cam->m2, pd, 2);
}
