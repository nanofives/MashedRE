// FLAGS: /O2 /GS /Ob1
/* 0x0048a850: 100% (regs 100%, whole symbol byte-identical 457 B). Raw converter 35.8%. By hand: Particle34
   pool struct (512 x 0x34 at 0x00715230, bitmask 0x007151f0, ring index 0x0071fa34), V3 struct copies,
   2-arg FUN_00472650(lo, hi), DAT_005d757c is the 0.0f literal (not a variable), 7th parameter p7 unread
   (callers pass 1.0f). The alpha needs ((y - 0.007f) * 76.9230728f) * 128.0f with the parentheses: without
   them cl folds 76.92 * 128 into one constant (94.2%). Float literals checked against 0x005cd0e8 (0.003),
   0x005cf48c (0.007), 0x005cf488, 0x005cc9d0 (128), 0x005cd6d4 (64), 0x005cc31c (3.0). */
/* MATCH-S14 0x0048a850: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs14_20261102/results.tsv. */
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
typedef struct { uchar r, g, b, a; } Rgba;
typedef struct { float x, y, z; } V3;
typedef struct {
  V3 pos;        /* +0x00 */
  V3 vel;        /* +0x0c */
  Rgba color;    /* +0x18 */
  float life;    /* +0x1c */
  int f20;       /* +0x20 */
  float size;    /* +0x24 */
  V3 accel;      /* +0x28 */
} Particle34;    /* 0x34 */
MGLOBAL(int, DAT_0071fa34, 0x0071fa34);
MGLOBALA(uchar, DAT_007151f0, 64, 0x007151f0);
MGLOBALA(Particle34, DAT_00715230, 512, 0x00715230);
float FUN_00472650(float lo, float hi);
float FUN_004c39b0(float *out, float *in);
// MATCH: 0x0048a850 FUN_0048a850
void FUN_0048a850(V3 *pos, int count, V3 *vel, Rgba *color, V3 *accel, float size, float p7)
{
  Particle34 *p;
  int idx;

  for (; count != 0; count--) {
    idx = DAT_0071fa34;
    p = &DAT_00715230[idx];
    DAT_007151f0[idx >> 3] |= (uchar)(1 << (idx & 7));
    p->f20 = 0;
    if (size > 0.0f) {
      p->size = size;
    } else {
      p->size = 0.25f;
    }
    p->life = FUN_00472650(-1.5f, 1.5f) + 3.0f;
    p->pos = *pos;
    if (vel != 0) {
      p->vel = *vel;
    } else {
      p->vel.x = FUN_00472650(-1.0f, 1.0f);
      p->vel.y = FUN_00472650(0.0f, 1.0f);
      p->vel.z = FUN_00472650(-1.0f, 1.0f);
      FUN_004c39b0(&p->vel.x, &p->vel.x);
      p->vel.x = p->vel.x * 0.003f;
      p->vel.y = p->vel.y * 0.003f;
      p->vel.z = p->vel.z * 0.003f;
      p->vel.y = FUN_00472650(0.007f, 0.013f);
    }
    if (color != 0) {
      p->color = *color;
    } else {
      p->color.r = 0xff;
      p->color.g = 0x84;
      p->color.b = 0x21;
      if (vel == 0) {
        p->color.a = (uchar)(((p->vel.y - 0.007f) * 76.9230728f) * 128.0f + 64.0f);
      } else {
        p->color.a = 0xff;
      }
    }
    if (accel != 0) {
      p->accel = *accel;
    } else {
      p->accel.x = 0.0f;
      p->accel.y = -0.0001f;
      p->accel.z = 0.0f;
    }
    DAT_0071fa34++;
    if (DAT_0071fa34 >= 0x200) {
      DAT_0071fa34 = 0;
    }
  }
}
