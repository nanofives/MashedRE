// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x004212b0 (685 B): best 69.5% (regs 69.5%, ops 97.0%), 193/197 instructions, by hand.
   Smoke emitter (4 records at DAT_0063e4b8) + updater/submit (128 records at DAT_0063e588). Structure, call order,
   x87 code and both loops match; the frame (0x34) matches once the first loop's vector is at function scope and the
   second loop's locals are block-scoped (they share slots with the first loop's). 1.0f must be a literal (the
   compiler keeps it in memory, the fild-first arm then matches); 0.0f goes through DAT_005d757c.
   Residual (b) source shape: the original walks the first loop with TWO pointer induction variables (EBX =
   &rec.v[0], EDI = &rec.v[1], loop test on EDI) besides i, so it pushes EBX and every later [esp+k] is 4 higher.
   Every spelling tried folds them into one IV: plain v[0]+v[1] / v[1]+v[0], record pointer, float* casts, an
   inlined 2-step sum loop (loads v[0] first), reversed loop, explicit p0/p1 pointer pair. A second MGLOBALA view at
   0x0063e4bc gives 72.6% but is a different access shape (one offset IV, two relocs), not kept. The permuter's
   89.3% (`unsigned i; i <= 3`) is unfaithful (second loop counts down, sar -> shr), not kept.
   Relocheck OK. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct Rec0063e4b8 {
  float v[2];    /* +0x00 */
  int a[2];      /* +0x08 */
  int b[2];      /* +0x10 */
  int id;        /* +0x18 */
  int f7;        /* +0x1c */
  int f8;        /* +0x20 */
} Rec0063e4b8;
MGLOBALA(Rec0063e4b8, DAT_0063e4b8, 4, 0x0063e4b8);
typedef struct Smoke0063e588 {
  float sx;            /* +0x00 */
  float sy;            /* +0x04 */
  float pos[3];        /* +0x08 */
  float ang;           /* +0x14 */
  float spin;          /* +0x18 */
  float age;           /* +0x1c, -1.0f = free */
  float f20;           /* +0x20 */
  unsigned char col[4];/* +0x24 */
  int owner;           /* +0x28 */
} Smoke0063e588;
MGLOBALA(Smoke0063e588, DAT_0063e588, 128, 0x0063e588);
MGLOBAL(int, DAT_0063fb88, 0x0063fb88);
typedef struct V3_004212b0 { float x, y, z; } V3_004212b0;
typedef struct Mat_004212b0 { float m[12]; V3_004212b0 pos; float pad; } Mat_004212b0;
typedef struct RGBA_004212b0 { unsigned char r, g, b, a; } RGBA_004212b0;
typedef struct V2_004212b0 { float x, y; } V2_004212b0;
MGLOBALA(unsigned char, DAT_0063e4a8, 16, 0x0063e4a8);
MGLOBALA(unsigned char, DAT_0063e548, 64, 0x0063e548);
MGLOBAL(const float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_007f100c, 0x007f100c);
double sin(double);
double cos(double);
#pragma intrinsic(sin, cos)
void FUN_0046cb30(V3_004212b0 *out, int i);
float FUN_004c3ac0(V3_004212b0 *v);
int FUN_0040e370(int i);
Mat_004212b0 *FUN_0041f290(int i, int n);
float FUN_00472650(float lo, float hi);
void FUN_00421160(V3_004212b0 *pos, int owner);
void FUN_00474de0(V3_004212b0 *dst, V3_004212b0 *src, float k);
void FUN_004769a0(V3_004212b0 *v);
void FUN_004769d0(V2_004212b0 *v);
void FUN_00476a30(float a);
void FUN_004769f0(RGBA_004212b0 *c);
void FUN_00476d00(void *sys);
void FUN_00476df0(void *sys);
// MATCH: 0x004212b0 FUN_004212b0
void FUN_004212b0(void)
{
  V3_004212b0 tmp;
  int i;
  for (i = 0; i < 4; i++) {
    float amt;
    FUN_0046cb30(&tmp, i);
    FUN_004c3ac0(&tmp);
    amt = (DAT_0063e4b8[i].v[0] + DAT_0063e4b8[i].v[1]) * 0.01f;
    if (!(amt < DAT_005d757c)) {
      if (FUN_0040e370(i) != 0) {
        if (FUN_00472650(0.0f, 1.0f) < amt * 0.5f) {
          V3_004212b0 p = FUN_0041f290(i, 0x33)->pos;
          p.x += FUN_00472650(-0.07f, 0.07f);
          p.y += FUN_00472650(-0.1f, 0.1f);
          p.z += FUN_00472650(-0.07f, 0.07f);
          FUN_00421160(&p, i);
        }
      }
    }
  }
  for (i = 0; i < 128; i++) {
    Smoke0063e588 *p = &DAT_0063e588[i];
    float age;
    RGBA_004212b0 col;
    float s;
    V2_004212b0 size;
    V3_004212b0 v;
    age = p->age;
    if (age < 1.0f && age >= DAT_005d757c) {
      float t, c;
      col = *(RGBA_004212b0 *)p->col;
      t = age * 6.2831855f;
      s = (float)(sin(t) * 0.05);
      c = (float)(cos(t) * 0.05);
      p->age += DAT_007f100c;
      if (age < 0.2f)
        col.a = (unsigned char)(p->col[3] * (age * 5.0f));
      else
        col.a = (unsigned char)(p->col[3] * (1.0f - age));
      size.x = p->sx * p->f20;
      size.y = p->sy * p->f20;
      p->ang += p->spin;
      v.y = age * 0.6f + p->pos[1];
      if (p->spin > DAT_005d757c) {
        v.x = s + p->pos[0];
        v.z = c + p->pos[2];
      } else {
        v.x = p->pos[0] - s;
        v.z = p->pos[2] - c;
      }
      FUN_00474de0(&v, &v, 0.05f);
      FUN_004769a0(&v);
      FUN_004769d0(&size);
      FUN_00476a30(p->ang);
      FUN_004769f0(&col);
      FUN_00476d00(DAT_0063e548);
    } else {
      DAT_0063e4a8[i >> 3] &= ~(1 << (i & 7));
    }
  }
  FUN_00476df0(DAT_0063e548);
}
