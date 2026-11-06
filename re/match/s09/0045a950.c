// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x0045a950 (h9, by hand): best 92.3% (regs 92.9%). Raw converter 11.3%. Spark records are the
   4x5x5 array of 0x34-byte records at 0x0068bd00 (same array 0x0045a3a0 clears), indexed [car+0xb0][o+0x14][o+0x18].
   Moved it: float literals for the .rdata constants (0x005d757c 0.0f, 0x005ce4f0 -0.02f, 0x005cc9f4 8.0f,
   0x005cc9c0 0.2f, 0x005cc734 110.0f; the rand ranges are +-pi and +-0.002f), block-scoped `t` / `w` (gives the
   0x28 frame: the compiler packs them over pos / v and puts `a` in the dead parameter home), the two
   FUN_00407a40 calls as separate statements (inline in the compare they are evaluated right to left),
   `s = x * 0.2f; s = s * 110.0f` as two statements (one expression folds to a single fmul 22.0f), memset for
   the zeroed output vector (plain or initializer zero stores are dead-store-eliminated, the original keeps them).
   Residual (b): memset zeroes through `xor ecx,ecx` + register stores where the original stores immediates
   (volatile stores: 91.7%), and the last block's scheduling (the original pushes car+0xb0 via `mov ecx,edx`
   before the 110.0f multiply and computes w.y / w.z as `fld mem; fmul st(1)`, keeping s to a final fstp st(0)).
   Tried: inline zero / scale helpers, int-typed zero stores, pointer alias, a local for car+0xb0, permuter. */
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
typedef struct { float x, y, z; } V3_950;
typedef struct {
  V3_950 pos;     /* +0x00 */
  V3_950 vel;     /* +0x0c */
  int f18[4];     /* +0x18 */
  float ang;      /* +0x28 */
  int f2c;        /* +0x2c */
  int active;     /* +0x30 */
} Spark950;
typedef Spark950 Spark950x5x5[5][5];
typedef struct {
  char pad00[0x20];
  V3_950 f20;     /* +0x20 */
  char pad2c[0x90 - 0x2c];
  V3_950 f90;     /* +0x90 */
  char pad9c[0xb0 - 0x9c];
  int b0;         /* +0xb0 */
} Car950;
typedef struct { int f0; int frame; } Sub24_950;
typedef struct {
  int f00;
  Car950 *car;    /* +0x04 */
  float f08;      /* +0x08 */
  int f0c, f10;
  int f14;        /* +0x14 */
  int f18;        /* +0x18 */
  int f1c;        /* +0x1c */
  int f20;
  Sub24_950 *p24; /* +0x24 */
} Obj950;
MGLOBALA(Spark950x5x5, DAT_0068bd00, 4, 0x0068bd00);
int FUN_00407a40(int car);
void FUN_0040e180(int *a, int *b);
int FUN_0046bf50(V3_950 *v, int car);
int FUN_0046cb30(V3_950 *v, int car);
float FUN_00472650(float lo, float hi);
char *FUN_004c0ed0(int frame);
void *memset(void *d, int c, unsigned int n);
#pragma intrinsic(memset)
// MATCH: 0x0045a950 FUN_0045a950
void FUN_0045a950(Obj950 *o)
{
  Car950 *car;
  Spark950 *e;
  V3_950 pos;
  V3_950 v;
  V3_950 tmp;
  int a;
  int b;
  int win;
  int pa;
  int pb;

  pos = *(V3_950 *)(FUN_004c0ed0(o->p24->frame) + 0x30);
  car = o->car;
  v = car->f90;
  v.x = v.x * 0.0f;
  v.y = v.y * 0.0f;
  v.z = v.z * 0.0f;
  pos.x = pos.x + v.x;
  pos.y = pos.y + v.y;
  pos.z = pos.z + v.z;
  if (o->f1c != 0 && o->f08 <= 0.0f) {
    V3_950 t;
    e = &DAT_0068bd00[car->b0][o->f14][o->f18];
    if (++o->f18 >= 5) {
      o->f14++;
      o->f1c = 0;
      o->f18 = 0;
    }
    e->active = 1;
    o->f08 = 0.02f;
    e->ang = FUN_00472650(-3.14159274f, 3.14159274f);
    e->pos = pos;
    FUN_0046cb30(&tmp, o->car->b0);
    car = o->car;
    t.x = car->f20.x * -0.02f;
    t.y = car->f20.y * -0.02f;
    t.z = car->f20.z * -0.02f;
    t.x = FUN_00472650(-0.002f, 0.002f) * 8.0f + t.x;
    t.z = FUN_00472650(-0.002f, 0.002f) * 8.0f + t.z;
    e->vel = t;
    a = 0;
    b = 0;
    FUN_0040e180(&a, &b);
    pa = FUN_00407a40(a);
    pb = FUN_00407a40(b);
    win = a;
    if (pa <= pb) {
      win = b;
    }
    car = o->car;
    if (car->b0 != win) {
      V3_950 w;
      float s;
      memset(&w, 0, sizeof(w));
      s = (float)o->f18 * 0.2f;
      s = s * 110.0f;
      w.x = s * car->f20.x;
      w.y = car->f20.y * s;
      w.z = car->f20.z * s;
      FUN_0046bf50(&w, car->b0);
    }
  }
}
