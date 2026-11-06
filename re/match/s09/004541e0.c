// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x004541e0 (h3): 100% (regs 100%, whole symbol byte-identical, 307 B). Raw form 15.6% (unaff_ESI).
   The 0x2c-byte entry (table 0x00688240) arrives in ESI: FUN_004541e0 is a file-static callee (pilot finding 1).
   Its real caller is 0x00454350 (call at 0x00454758); this file uses a small stand-in caller (not a MATCH target)
   to make 9176 pick the register convention. What moved it: the two .rdata floats are compiler literals (3.5f at
   0x005ce26c, 40.0f at 0x005cd274; as globals the fmul operands swap), V3 struct copies for +0x10 and +0x90, the
   two FUN_004c1480 calls written out in each branch (9176 tail-merges them; a shared pointer temp got other
   registers), FUN_004c1480 has 3 args (the push 0 before the inner call is its third). Field names = offsets. */
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
typedef struct V3 { float x; float y; float z; } V3;
typedef struct Mat40 { V3 f0; int fc; V3 f10; int f1c; V3 f20; int f2c; V3 f30; int f3c; } Mat40;
typedef struct Frame34 { int f0; int f4; int f8; int fc; Mat40 f10; } Frame34;
typedef struct Clump20 { int f0; int *f4; } Clump20;
typedef struct Rec44 { V3 f0; float fc; V3 f10; V3 f1c; int f28; int f2c; Clump20 *f30; Frame34 *f34;
  int *f38; float f3c; float f40; } Rec44;
typedef struct Obj54 { int f0; int *f4; char pad8[0x40]; void (*f48)(struct Obj54 *); } Obj54;
typedef struct Obj0c { char pad0[0x10]; V3 f10; char pad1c[0x74]; V3 f90; } Obj0c;
typedef struct Ent2c { Rec44 *f0[2]; int f8; Obj0c *fc; int f10; Obj54 *f14[3]; Clump20 *f20; int f24;
  int f28; } Ent2c;
MGLOBALA(Rec44, DAT_00688020, 8, 0x00688020);
MGLOBALA(Ent2c, DAT_00688240, 4, 0x00688240);
int FUN_004c1210(Frame34 *p0);
void FUN_004c15c0(void *p0);
int FUN_004e43b0(int p0, Clump20 *p1);
int FUN_004e4440(Clump20 *p0);
void FUN_004b6520(void *p0, int p1);
int FUN_004e4800(Clump20 *p0);
int FUN_004e45b0(int p0, Clump20 *p1);
Mat40 *FUN_004c0ed0(void *p0);
int FUN_004c1480(void *p0, void *p1, int p2);
// MATCH: 0x004541e0 FUN_004541e0
static void FUN_004541e0(Ent2c *e)
{
  V3 a;
  V3 b;
  Rec44 *r;
  int n;

  a.x = e->fc->f10.x * 3.5f;
  a.y = e->fc->f10.y * 3.5f;
  a.z = e->fc->f10.z * 3.5f;
  n = e->f8;
  if (n == 2) {
    e->f24 = 4;
  } else if (n == 1) {
    e->f28 = 4;
  }
  e->f8 = n - 1;
  r = e->f0[e->f8];
  r->f28 = 0;
  r->f10 = FUN_004c0ed0(r->f34)->f30;
  FUN_004c1210(r->f34);
  if (e->f8 == 1) {
    FUN_004c1480(r->f34, FUN_004c0ed0(e->f14[2]->f4), 0);
  } else if (e->f8 == 0) {
    FUN_004c1480(r->f34, FUN_004c0ed0(e->f14[1]->f4), 0);
  }
  r->f2c = 2;
  b = e->fc->f90;
  a.x = b.x * 40.0f + a.x;
  a.y = b.y * 40.0f + a.y;
  a.z = b.z * 40.0f + a.z;
  r->f1c = a;
  e->f0[e->f8] = 0;
}
typedef struct Obj0ac { char pad0[0x30]; V3 f30; char pad3c[0x70]; Ent2c *fac; } Obj0ac;
void FUN_00465ca0(int p0, V3 *p1);
/* stand-in caller (not a MATCH target): same shape as the original's caller at 0x00454740 */
void standin_00454740(Obj0ac *o, int k)
{
  V3 v;
  if (k == 2) {
    FUN_004541e0(o->fac);
    v = o->f30;
    FUN_00465ca0(0x12, &v);
  }
}
