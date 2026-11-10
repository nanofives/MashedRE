// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x0045a190 (h9, by hand): best 93.0% (regs 93.0%). Raw converter 26.8%. The file also holds
   0x00459480 (static, slot pointer in EDI, 100%: body copied from re/match/s09/00459480.c, the other helper's
   file), because 0x0045a190 is its only caller and the EDI convention needs it in the same TU.
   Moved it: the callee in the file (EDI = slot pointer), the loop walker copied through a second pointer
   (`n = p - 1; p = n; ... while (n != base)`): a plain `p--` / `p -= 1` / for / while form makes the compiler
   switch to a down-counter in EBP plus a pointer biased to &slot->f34 (47%), an index loop gives the biased
   pointer (78%). Float literals for the .rdata constants (0x005cc56c 0.1f, 0x005cc9a0 0.05f, 0x005ccac0 0.4f,
   0x005cc99c 0.3f), sin() intrinsic (gives the and esp,-8 frame), `d *= 0.1f` per component then `pos += d`.
   Residual (b): `add edi,-0x58` where the original has `sub edi,0x58` (the copy that blocks the counter
   produces the add), and x87 scheduling in three places (the d.y / d.z subtractions and the 0.3f size scale:
   the original issues the fsub/fstp before the next push). Tried: split statements, V3* return type for
   FUN_004671d0, size as a struct, compound assignments, permuter (its only gain moved a call, rejected). */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
  int idx;          /* +0x00 */
  int f04;
  int f08;
  int f0c;
  int f10;          /* +0x10 */
  int f14;
  int f18;
  float f1c;        /* +0x1c */
  int f20[4];
  float f30;        /* +0x30 */
  int f34;
  int mat;          /* +0x38 */
  float f3c[3];     /* +0x3c */
  float f48[3];     /* +0x48 */
  int clump;        /* +0x54 */
} Slot459480;
int FUN_004671a0(int a);
float FUN_00474e70(float *a, float *b, float *c);
void FUN_004c0e50(int frame);
void FUN_004c45f0(int m);
void FUN_004c5010(int m, float *v, int op);
// MATCH: 0x00459480 FUN_00459480
static void FUN_00459480(Slot459480 *p)
{
  float v[3];
  float d;
  int m;

  v[2] = p->f1c * 4.0f;
  v[0] = 4.0f;
  v[1] = 4.0f;
  d = FUN_00474e70(p->f48, p->f3c, (float *)(*(int *)(FUN_004671a0(0) + 4) + 0x40));
  if (d < 0.003f) {
    d = 0.003f;
  } else if (d > 3.0f) {
    d = 3.0f;
  }
  d = d * 0.5f; v[0] = v[0] * d; v[1] = d * v[1];
  m = *(int *)(p->clump + 4) + 0x10;
  FUN_004c5010(m, v, 1);
  FUN_004c45f0(m);
  FUN_004c0e50(*(int *)(p->clump + 4));
}

typedef struct { float x, y, z; } V3_190;
typedef struct { uchar r, g, b, a; } Rgba190;
MGLOBALA(Slot459480, DAT_0068b9f8, 4, 0x0068b9f8);
MGLOBALA(char, DAT_0068b9b8, 0x40, 0x0068b9b8);
typedef int (*AtomicCb190)(int atomic, int data);
int FUN_00459400(int atomic, int data);
float *FUN_004671d0(int i);
void FUN_004769a0(V3_190 *p);
void FUN_004769d0(float *s);
void FUN_004769f0(Rgba190 *c);
void FUN_00476d00(void *p);
void FUN_00476df0(void *p);
void FUN_004b5580(int clump, void *c);
int FUN_004e66d0(int clump, AtomicCb190 cb, int data);
double sin(double);
#pragma intrinsic(sin)
// MATCH: 0x0045a190 FUN_0045a190
void FUN_0045a190(void)
{
  Slot459480 *p;
  Rgba190 c;
  float size[2];
  V3_190 pos;
  V3_190 d;
  float s;
  Slot459480 *n;
  float *cam;
  int *q;

  p = &DAT_0068b9f8[4];
  do {
    n = p - 1;
    p = n;
    if (p->f10 != 0) {
      p->f30 = p->f30 + 0.1f;
      FUN_00459480(p);
      FUN_004b5580(p->clump, &p->f34);
      if (p->f14 != 0) {
        c.r = ((uchar *)&p->f34)[0];
        c.g = ((uchar *)&p->f34)[1];
        c.b = ((uchar *)&p->f34)[2];
        c.a = 0xff;
        FUN_004769f0(&c);
        s = (float)sin(p->f30) * 0.05f + 0.4f;
        size[0] = s;
        size[1] = s;
        FUN_004769d0(size);
        pos = *(V3_190 *)&p->f20[1];
        d.x = (float)(FUN_004671d0(0)[0] - (double)(pos.x));
        d.y = FUN_004671d0(0)[1] - pos.y;
        cam = FUN_004671d0(0);
        d.x = d.x * 0.1f;
        d.y = d.y * 0.1f;
        d.z = (cam[2] - pos.z) * 0.1f;
        pos.x = d.x + pos.x;
        pos.y = d.y + pos.y;
        pos.z = d.z + pos.z;
        FUN_004769a0(&pos);
        FUN_00476d00(DAT_0068b9b8);
        size[0] = size[0] * 0.3f;
        size[1] = size[1] * 0.3f;
        FUN_004769d0(size);
        c.r = 0xff;
        c.g = 0xff;
        c.b = 0xff;
        c.a = 200;
        FUN_004769f0(&c);
        d.x = d.x * 0.1f;
        d.y = d.y * 0.1f;
        d.z = d.z * 0.1f;
        pos.x = d.x + pos.x;
        pos.y = d.y + pos.y;
        pos.z = d.z + pos.z;
        FUN_004769a0(&pos);
        FUN_00476d00(DAT_0068b9b8);
      }
      FUN_004e66d0(p->clump, FUN_00459400, 0);
    }
  } while (n != DAT_0068b9f8);
  FUN_00476df0(DAT_0068b9b8);
}
