// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046f6c0 (h5): best 44.3% (regs 53.4%, ops 90.0%), 885/893 ins. Raw 5.8%.
   Static callee: ctx in EAX (push ebx / mov ebx, eax), car in EDI (never written), flag on the stack. Real
   callers 0x004704c0, 0x004709a0 (matched in their own files with a stand-in for this function) and 0x00470c70;
   a stand-in caller sits at the end (#ifndef MATCH_PORT). This file also holds the static callee 0x0046c5f0
   (a in EAX, c in ECX, b in EDX, out in ESI; copied from pilot a5_0046c5f0.c, 100% here too).
   Decomp corrections: the 3-wheel branch picks the three wheel offsets (0,1,2) / (0,1,3) / (0,3,2) / (1,3,2)
   after copying the missing wheel from a neighbour (Ghidra dropped the indices); the 0x00470040 test is
   `wheel.tag == 0xff020202` (int); local_ac is (int)car[0x9e0] via ftol; acos is the CRT stack entry 0x004a3384.
   .rdata float constants as literals (0x005cea60 0.2778, 0x005ce18c 0.02, 0x005ceaa0 -0.005, 0x005cc34c -2,
   0x005cea98/90 -0.1/0.1 double, 0x005cea88 0.999, 0x005cc98c 57.29578, 0x005cd088 2.5, 0x005cc9fc 1000,
   0x005cc340 0.7, 0x005cc55c 10, 0x005cc990 1e-5, 0x005cc574 2, 0x005cd61c -20, 0x005cea84 -0.0625).
   Moved the score: the 4-wheel classification loop as do/while with `goto next` (31.6 -> 40.5: the for-loop form
   was fully unrolled, 1104 ins), the grounded counts as four explicit blocks reading len once (43.7), len read
   once at the loop top and `ground` set after the counts (44.3).
   Residual (b), ran out of budget: stack-slot assignment differs throughout (the original keeps the matrix
   pointer in a stack slot and EBP = 0, ours keeps the matrix in EBP), the classification loop shares one k*12
   induction for off[] and the 0x0088e620 table where ours uses a table pointer, and x87 order in the sums.
   The permuter reached 47.3% in three rounds with statement swaps (not checked for semantics, not kept). */
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
typedef struct V3_0046f6c0 { float x, y, z; } V3_0046f6c0;
typedef struct Mtx_0046f6c0 { float m[12]; V3_0046f6c0 pos; float pad; } Mtx_0046f6c0;
typedef struct Mat_0046f6c0 { float m[16]; } Mat_0046f6c0;
/* one wheel record, car + 0x16c + k * 0xc4 */
typedef struct Wheel_0046f6c0 {
  float x, y, z;
  char p0[0x28 - 0xc];
  float len;
  int state;
  char p1[0x80 - 0x30];
  int id;
  int tag;
  char p2[0x94 - 0x88];
  V3_0046f6c0 n;
  char p3[0xc4 - 0xa0];
} Wheel_0046f6c0;
typedef struct Query_0046f6c0 { V3_0046f6c0 p; float r; int a, b; int n; } Query_0046f6c0;
MGLOBAL(int, DAT_0088e650, 0x0088e650);
MGLOBALA(int, DAT_0088e5e0, 4, 0x0088e5e0);
MGLOBALA(V3_0046f6c0, DAT_00881560, 4, 0x00881560);
MGLOBAL(V3_0046f6c0, DAT_0088e600, 0x0088e600);
MGLOBALA(V3_0046f6c0, DAT_0088e620, 4, 0x0088e620);
MGLOBAL(int, DAT_0088e60c, 0x0088e60c);
MGLOBAL(int, DAT_0088e654, 0x0088e654);
MGLOBAL(int, DAT_00828320, 0x00828320);
MGLOBAL(V3_0046f6c0, DAT_006146fc, 0x006146fc);
float FUN_004c3b30(float x);
void FUN_004c3d90(V3_0046f6c0 *out, V3_0046f6c0 *in, int n, Mtx_0046f6c0 *m);
int FUN_00468b80();
void FUN_00538c80(int w, Query_0046f6c0 *q, int (*cb)(), int *res);
int FUN_0046cc40(char *car, int *res);
double FUN_004a3384(double x);
float FUN_004c39b0(V3_0046f6c0 *out, V3_0046f6c0 *in);
void *FUN_004c4d20(Mat_0046f6c0 *m, V3_0046f6c0 *axis, float angle, int combine);
void *FUN_004c52f0(Mtx_0046f6c0 *m, Mat_0046f6c0 *t, int combine);

#define F_0046f6c0(o) (*(float *)(car + (o)))
#define I_0046f6c0(o) (*(int *)(car + (o)))
#define W_0046f6c0(k) ((Wheel_0046f6c0 *)(car + 0x16c + (k) * 0xc4))

/* 0x0046c5f0: plane normal of three points; a in EAX, c in ECX, b in EDX, out in ESI (pilot a5_0046c5f0.c,
   100%; this file holds a copy as the static callee) */
// MATCH: 0x0046c5f0 FUN_0046c5f0
static void FUN_0046c5f0(float *a, float *c, float *b, float *out)
{
    float ax = a[0] - c[0], ay = a[1] - c[1], az = a[2] - c[2];
    float bx = a[0] - b[0], by = a[1] - b[1], bz = a[2] - b[2];
    float nx = bz * ay - by * az;
    float ny = bx * az - bz * ax;
    float nz = by * ax - bx * ay;
    float l = nx * nx + ny * ny + nz * nz;
    if (l >= 0.0f) {
        l = FUN_004c3b30(l);
        if (l != 0.0f) {
            l = 1.0f / l;
            out[0] = nx * l;
            out[1] = ny * l;
            out[2] = l * nz;
        }
    }
}

// MATCH: 0x0046f6c0 FUN_0046f6c0
static void FUN_0046f6c0(int flag, int *ctx, char *car)
{
  Mtx_0046f6c0 *m;
  Wheel_0046f6c0 *w;
  int k;
  int a;
  int b;
  int ground;
  int count;
  int prevcnt;
  int i0, i1, i2;
  int idx;
  float f;
  float sx;
  float inv;
  float best;
  float ang;
  V3_0046f6c0 z;
  V3_0046f6c0 S;
  V3_0046f6c0 N;
  V3_0046f6c0 axis;
  V3_0046f6c0 save;
  V3_0046f6c0 u;
  V3_0046f6c0 c;
  V3_0046f6c0 off[4];
  float ax[4];
  float ay[4];
  float az[4];
  Query_0046f6c0 q;
  Mat_0046f6c0 rot;

  m = (Mtx_0046f6c0 *)(car + I_0046f6c0(0x9ac) * 0x40 + 0x928);
  DAT_0088e650 = 0;
  z.x = 0.0f;
  z.y = 0.0f;
  z.z = 0.0f;
  prevcnt = (int)F_0046f6c0(0x9e0);
  for (k = 0, w = W_0046f6c0(0); k < 4; k++, w++) {
    DAT_0088e5e0[k] = 0;
    f = w->x * 0.2778f;
    w->id = -1;
    w->len = 10.0f;
    DAT_00881560[k].y = 0.0f;
    DAT_00881560[k].x = f;
    DAT_00881560[k].z = w->z * 0.2778f;
  }
  FUN_004c3d90(&DAT_0088e600, &z, 1, m);
  FUN_004c3d90(DAT_0088e620, DAT_00881560, 4, m);
  FUN_0046c5f0(&DAT_0088e620[0].x, &DAT_0088e620[3].x, &DAT_0088e620[2].x, (float *)(car + 0x9c8));
  q.n = 3;
  q.p = m->pos;
  q.r = F_0046f6c0(0x4a4);
  DAT_0088e60c = 0;
  DAT_0088e654 = (int)ctx;
  FUN_00538c80(ctx[2], &q, FUN_00468b80, &DAT_00828320);
  FUN_0046cc40(car, &DAT_00828320);

  a = 0;
  b = 0;
  if (W_0046f6c0(0)->state == 1) {
    f = W_0046f6c0(0)->len;
    if (f >= 0.02f)
      a = 1;
    if (f <= -0.005f)
      b = 1;
  }
  if (W_0046f6c0(1)->state == 1) {
    f = W_0046f6c0(1)->len;
    if (f >= 0.02f)
      a++;
    if (f <= -0.005f)
      b++;
  }
  if (W_0046f6c0(2)->state == 1) {
    f = W_0046f6c0(2)->len;
    if (f >= 0.02f)
      a++;
    if (f <= -0.005f)
      b++;
  }
  if (W_0046f6c0(3)->state == 1) {
    f = W_0046f6c0(3)->len;
    if (f >= 0.02f)
      a++;
    if (f <= -0.005f)
      b++;
  }
  ground = 0;
  if (a != 0 && b == 0)
    ground = 1;

  k = 0;
  w = W_0046f6c0(0);
  do {
    f = w->len;
    if (w->state != 0) {
      if ((f > 0.02f && ground) || w->id == -1) {
        w->state = 0;
        goto next;
      }
      w->state = 1;
    } else {
      if (!(f > -2.0f) || !(f <= 0.0f))
        goto next;
      w->state = 2;
      f = (F_0046f6c0(0x9dc) * F_0046f6c0(0x9b8) + (F_0046f6c0(0x9d8) * F_0046f6c0(0x9b4) + F_0046f6c0(0x9d4) * F_0046f6c0(0x9b0))) / F_0046f6c0(0x9e4);
      if (f != 0.0f && f > -0.1 && f < 0.1)
        w->state = 2;
    }
    ax[k] = w->n.x * w->len;
    off[k].x = DAT_0088e620[k].x - ax[k];
    ay[k] = w->n.y * w->len;
    off[k].y = DAT_0088e620[k].y - ay[k];
    az[k] = w->n.z * w->len;
    off[k].z = DAT_0088e620[k].z - az[k];
  next:
    k++;
    w++;
  } while (k < 4);

  count = 0;
  sx = 0.0f;
  S.y = 0.0f;
  S.z = 0.0f;
  if (W_0046f6c0(0)->state == 1) {
    S.y = ay[0];
    sx = ax[0];
    count = 1;
    S.z = az[0];
  }
  if (W_0046f6c0(1)->state == 1) {
    sx += ax[1];
    count++;
    S.y = ay[1] + S.y;
    S.z = az[1] + S.z;
  }
  if (W_0046f6c0(2)->state == 1) {
    sx += ax[2];
    count++;
    S.y = ay[2] + S.y;
    S.z = az[2] + S.z;
  }
  if (W_0046f6c0(3)->state == 1) {
    sx += ax[3];
    count++;
    S.y = ay[3] + S.y;
    S.z = az[3] + S.z;
  }
  if (count == 4) {
    idx = 0;
    best = W_0046f6c0(0)->len;
    if (best < 0.0f)
      best = -best;
    f = W_0046f6c0(1)->len;
    if (f < 0.0f)
      f = -f;
    if (best < f) {
      idx = 1;
      best = f;
    }
    f = W_0046f6c0(2)->len;
    if (f < 0.0f)
      f = -f;
    if (best < f) {
      idx = 2;
      best = f;
    }
    f = W_0046f6c0(3)->len;
    if (f < 0.0f)
      f = -f;
    if (best < f)
      idx = 3;
    sx -= ax[idx];
    count = 3;
    S.y -= ay[idx];
    S.z -= az[idx];
    W_0046f6c0(idx)->state = 0;
  } else if (count == 0) {
    goto tail;
  }
  inv = 1.0f / count;
  m->pos.x = m->pos.x - inv * sx;
  m->pos.y = m->pos.y - inv * S.y;
  m->pos.z = m->pos.z - inv * S.z;
  if (count == 3) {
    if (W_0046f6c0(0)->state == 1) {
      if (W_0046f6c0(1)->state == 1) {
        if (W_0046f6c0(2)->state == 1) {
          W_0046f6c0(3)->n = W_0046f6c0(2)->n;
          W_0046f6c0(3)->state = 1;
          W_0046f6c0(3)->len = W_0046f6c0(2)->len;
          W_0046f6c0(3)->id = W_0046f6c0(2)->id;
          i0 = 0; i1 = 1; i2 = 2;
        } else {
          W_0046f6c0(2)->n = W_0046f6c0(3)->n;
          W_0046f6c0(2)->id = W_0046f6c0(3)->id;
          W_0046f6c0(2)->state = 1;
          W_0046f6c0(2)->len = W_0046f6c0(3)->len;
          i0 = 0; i1 = 1; i2 = 3;
        }
      } else {
        W_0046f6c0(1)->state = 1;
        W_0046f6c0(1)->n = W_0046f6c0(0)->n;
        W_0046f6c0(1)->len = W_0046f6c0(0)->len;
        W_0046f6c0(1)->id = W_0046f6c0(0)->id;
        i0 = 0; i1 = 3; i2 = 2;
      }
    } else {
      W_0046f6c0(0)->n = W_0046f6c0(1)->n;
      W_0046f6c0(0)->state = 1;
      W_0046f6c0(0)->len = W_0046f6c0(1)->len;
      W_0046f6c0(0)->id = W_0046f6c0(1)->id;
      i0 = 1; i1 = 3; i2 = 2;
    }
    FUN_0046c5f0(&off[i0].x, &off[i1].x, &off[i2].x, &N.x);
  } else if (count < 3) {
    a = 0;
    N.z = 0.0f;
    N.y = 0.0f;
    N.x = 0.0f;
    if (W_0046f6c0(0)->state != 0) {
      N = W_0046f6c0(0)->n;
      a = 1;
    }
    if (W_0046f6c0(1)->state != 0) {
      a++;
      N.x = N.x + W_0046f6c0(1)->n.x;
      N.y = N.y + W_0046f6c0(1)->n.y;
      N.z = N.z + W_0046f6c0(1)->n.z;
    }
    if (W_0046f6c0(2)->state != 0) {
      a++;
      N.x = N.x + W_0046f6c0(2)->n.x;
      N.y = N.y + W_0046f6c0(2)->n.y;
      N.z = N.z + W_0046f6c0(2)->n.z;
    }
    if (W_0046f6c0(3)->state != 0) {
      a++;
      N.x = N.x + W_0046f6c0(3)->n.x;
      N.y = N.y + W_0046f6c0(3)->n.y;
      N.z = N.z + W_0046f6c0(3)->n.z;
    }
    if (a != 0) {
      f = 1.0f / a;
      N.x *= f;
      N.y *= f;
      N.z *= f;
      FUN_004c39b0(&N, &N);
    }
    goto tail;
  }
  axis.x = N.z * F_0046f6c0(0x9cc) - N.y * F_0046f6c0(0x9d0);
  axis.y = N.x * F_0046f6c0(0x9d0) - N.z * F_0046f6c0(0x9c8);
  axis.z = N.y * F_0046f6c0(0x9c8) - N.x * F_0046f6c0(0x9cc);
  f = N.x * F_0046f6c0(0x9c8) + N.y * F_0046f6c0(0x9cc) + N.z * F_0046f6c0(0x9d0);
  if ((axis.x != 0.0f || axis.y != 0.0f || axis.z != 0.0f) && (f < 0.999f || prevcnt < 3)) {
    if (f < -1.0f)
      f = -1.0f;
    else if (f > 1.0f)
      f = 1.0f;
    ang = (float)(FUN_004a3384(f) * 57.29578f);
    if (flag != 0 && ang > 2.5f)
      ang = 2.5f;
    save = m->pos;
    FUN_004c4d20(&rot, &axis, ang, 0);
    FUN_004c52f0(m, &rot, 2);
    m->pos = save;
  }
tail:
  for (k = 0, w = W_0046f6c0(0); k < 4; k++, w++) {
    if (w->state != 1)
      continue;
    if (w->tag == (int)0xff020202) {
      w->state = 0;
      c.x = w->n.x * 1000.0f;
      c.y = w->n.y * 1000.0f;
      c.z = w->n.z * 1000.0f;
      F_0046f6c0(0x9b0) = c.x + F_0046f6c0(0x9b0);
      F_0046f6c0(0x9b4) = c.y + F_0046f6c0(0x9b4);
      F_0046f6c0(0x9b8) = c.z + F_0046f6c0(0x9b8);
    }
    if (w->n.y < 0.7f) {
      w->state = 0;
      u.x = w->n.x * 10.0f;
      u.y = w->n.y * 10.0f;
      u.z = w->n.z * 10.0f;
      F_0046f6c0(0x9b0) = u.x + F_0046f6c0(0x9b0);
      F_0046f6c0(0x9b4) = u.y + F_0046f6c0(0x9b4);
      F_0046f6c0(0x9b8) = u.z + F_0046f6c0(0x9b8);
      FUN_004c39b0(&u, (V3_0046f6c0 *)(car + 0x9b0));
      f = F_0046f6c0(0x9e4) * 0.00001f;
      c.x = (u.z * w->n.y - u.y * w->n.z) * f;
      c.y = (u.x * w->n.z - u.z * w->n.x) * f;
      c.z = (u.y * w->n.x - u.x * w->n.y) * f;
      F_0046f6c0(0x144) = c.x + F_0046f6c0(0x144);
      F_0046f6c0(0x148) = c.y + F_0046f6c0(0x148);
      F_0046f6c0(0x14c) = c.z + F_0046f6c0(0x14c);
    }
  }
  f = (float)count;
  F_0046f6c0(0x9e0) = f;
  if (I_0046f6c0(0x9f0) == 0 && f != 0.0f && f <= 2.0f) {
    a = 0;
    S.z = 0.0f;
    S.y = 0.0f;
    sx = 0.0f;
    if (W_0046f6c0(0)->state != 1) {
      a = 1;
      sx = F_0046f6c0(0x9f8);
      S.y = F_0046f6c0(0x9fc);
      S.z = F_0046f6c0(0xa00);
    }
    if (W_0046f6c0(1)->state != 1) {
      sx += F_0046f6c0(0xa04);
      a++;
      S.y = F_0046f6c0(0xa08) + S.y;
      S.z = F_0046f6c0(0xa0c) + S.z;
    }
    if (W_0046f6c0(2)->state != 1) {
      sx += F_0046f6c0(0xa10);
      a++;
      S.y = F_0046f6c0(0xa14) + S.y;
      S.z = F_0046f6c0(0xa18) + S.z;
    }
    if (W_0046f6c0(3)->state != 1) {
      sx += F_0046f6c0(0xa1c);
      a++;
      S.y = F_0046f6c0(0xa20) + S.y;
      S.z = F_0046f6c0(0xa24) + S.z;
    }
    inv = 1.0f / a;
    S.x = F_0046f6c0(0x958) - inv * sx;
    S.y = F_0046f6c0(0x95c) - inv * S.y;
    S.z = F_0046f6c0(0x960) - inv * S.z;
    FUN_004c39b0(&S, &S);
    S.x *= -20.0f;
    S.y = 0.0f;
    S.z *= -20.0f;
    F_0046f6c0(0x9b0) = S.x + F_0046f6c0(0x9b0);
    F_0046f6c0(0x9b4) = S.y + F_0046f6c0(0x9b4);
    F_0046f6c0(0x9b8) = S.z + F_0046f6c0(0x9b8);
    FUN_004c39b0(&S, &S);
    c.x = S.y * DAT_006146fc.z - S.z * DAT_006146fc.y;
    c.y = S.z * DAT_006146fc.x - S.x * DAT_006146fc.z;
    c.z = S.x * DAT_006146fc.y - S.y * DAT_006146fc.x;
    FUN_004c39b0(&c, &c);
    c.x *= -0.0625f;
    c.y *= -0.0625f;
    c.z *= -0.0625f;
    F_0046f6c0(0x144) = c.x + F_0046f6c0(0x144);
    F_0046f6c0(0x148) = c.y + F_0046f6c0(0x148);
    F_0046f6c0(0x14c) = c.z + F_0046f6c0(0x14c);
  }
  I_0046f6c0(0x9e0) = 0;
  if (W_0046f6c0(0)->state != 0)
    F_0046f6c0(0x9e0) = 1.0f;
  if (W_0046f6c0(1)->state != 0)
    F_0046f6c0(0x9e0) = F_0046f6c0(0x9e0) + 1.0f;
  if (W_0046f6c0(2)->state != 0)
    F_0046f6c0(0x9e0) = F_0046f6c0(0x9e0) + 1.0f;
  if (W_0046f6c0(3)->state != 0)
    F_0046f6c0(0x9e0) = F_0046f6c0(0x9e0) + 1.0f;
}

#undef F_0046f6c0
#undef I_0046f6c0
#undef W_0046f6c0

#ifndef MATCH_PORT
/* stand-in caller (not the real TU): the real callers 0x004704c0 / 0x004709a0 / 0x00470c70 pass ctx in EAX and
   car in EDI with the flag on the stack */
void StandIn_0046f6c0(char *cars, int n, int *ctx)
{
  int i;
  for (i = 0; i < n; i++)
    FUN_0046f6c0(i & 1, ctx, cars + i * 0xd04);
}
#endif
