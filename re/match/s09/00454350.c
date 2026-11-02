// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00454350 (h3): 98.2% (regs 98.2%, ops 98.6%), 281/281 instructions. Raw form 16.3%. The file holds
   0x00454170 (record in ESI, file-static callee, 100%, own MATCH line; state 5 calls it with mov esi,ebp).
   What moved it: no char array in the locals (a char[40] in the hit struct brought in the /GS cookie and a shared
   epilogue), the hit struct at 0x40 bytes and the query struct at 0x8c bytes (frame 0x140), the hit test as
   `!= 0` with the colour path first (je to the query path), `!(t > 5.0f)` (gives je; `t <= 5.0f` gives jp), the
   gravity step as the .rdata V3 at 0x005ce42c (0,-5,0) times DAT_007f100c, the rest of the .rdata floats as
   literals (5.0, 0.05, 720.0, 2.5, 10.0, 360.0, 1.5, 0.1, 3.0, 40.0, 150.0, 200.0). FUN_0045bfe0 takes 2 args,
   FUN_004b4cd0 1 (the add esp,0xc covers both), FUN_00454320 is the query callback (cdecl, 2 args).
   Residual (b): two x87 scheduling spots, one fstp of a.x scheduled before the third push of the FUN_004b5080
   args, and the fld of the gravity x scheduled before the p->f10 store. Tried: += forms, statement reorders,
   per-component stores, pointer spellings, the permuter (415 compiles, no gain). Field names are offsets only. */
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
MGLOBALA(Rec44, DAT_00688020, 8, 0x00688020);
int FUN_004c1210(Frame34 *p0);
void FUN_004c15c0(void *p0);
int FUN_004e43b0(int p0, Clump20 *p1);
int FUN_004e4440(Clump20 *p0);
typedef struct Sphere10 { V3 f0; float fc; } Sphere10;
typedef struct Query20 { Sphere10 f0; int f10; Rec44 *f14; int f18; void (*f1c)(void *, void *); int f20[27]; } Query20;
typedef struct Seg18 { V3 f0; V3 fc; } Seg18;
typedef struct Hit3c { V3 f0; int fc; float f10[10]; float f38; int f3c; } Hit3c;
typedef struct Rgba4 { unsigned char r; unsigned char g; unsigned char b; unsigned char a; } Rgba4;
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBAL(V3, DAT_005f9808, 0x005f9808);
MGLOBAL(V3, DAT_006146fc, 0x006146fc);
void FUN_00454320(void *p0, void *p1);
void FUN_00453eb0(Rec44 *p0, float p1, float p2, float p3, int p4);
int FUN_0045bfe0(Seg18 *p0, Hit3c *p1);
void FUN_00475ab0(int p0, Mat40 *p1, int p2);
void FUN_00476880(V3 *p0, V3 *p1, float p2, float p3);
void FUN_00477760(int p0, Mat40 *p1, Rgba4 *p2, float p3, float p4);
int FUN_00484cf0(Query20 *p0);
void FUN_004b4650(V3 *p0, V3 *p1, V3 *p2, float p3);
int FUN_004b4cd0(int p0);
void FUN_004b5080(Mat40 *p0, float *p1, V3 *p2);
Mat40 *FUN_004c0ed0(void *p0);
int FUN_004c1480(void *p0, void *p1, int p2);
int FUN_004c1520(int *p0, V3 *p1, float p2, int p3);
void FUN_004c45f0(Mat40 *p0);
void FUN_004c5010(Mat40 *p0, V3 *p1, int p2);
Sphere10 *FUN_004e6100(Clump20 *p0);
MGLOBAL(V3, DAT_005ce42c, 0x005ce42c);
// MATCH: 0x00454170 FUN_00454170
static void FUN_00454170(Rec44 *p)
{
  int iVar1;
  p->f2c = 0;
  p->f28 = 0;
  p->f3c = 0;
  p->f0.x = 0;
  p->f0.y = 0;
  p->f0.z = 0;
  p->fc = 0;
  p->f40 = 0;
  FUN_004c15c0(p->f34);
  FUN_004c15c0(p->f38);
  if (p->f34->f4 != 0) {
    FUN_004c1210(p->f34);
  }
  iVar1 = FUN_004e4440(p->f30);
  if (iVar1 != 0) {
    FUN_004e43b0(iVar1, p->f30);
  }
}
// MATCH: 0x00454350 FUN_00454350
void FUN_00454350(Rec44 *p)
{
  V3 a;
  Rgba4 col;
  Seg18 seg;
  V3 scale;
  Mat40 m;
  Hit3c hit;
  Query20 q;
  int iVar1;

  if (p->f2c == 0) return;
  if (p->f2c == 1) {
    p->f10 = FUN_004c0ed0(p->f34)->f30;
    return;
  }
  if (p->f2c == 2) {
    if (!(p->f3c > 5.0f)) {
      a = p->f10;
      a.x = DAT_007f100c * p->f1c.x + a.x;
      seg.f0 = p->f10;
      a.y = DAT_007f100c * p->f1c.y + a.y;
      a.z = DAT_007f100c * p->f1c.z + a.z;
      seg.fc = a;
      if (FUN_004b4cd0(FUN_0045bfe0(&seg, &hit)) != 0) {
        col.g = 0x80;
        col.a = 0x80;
        col.r = 0xff;
        col.b = 0;
        FUN_004b4650(&a, &seg.f0, &seg.fc, hit.f38);
        a.x = hit.f0.x * 0.05f + a.x;
        a.y = hit.f0.y * 0.05f + a.y;
        a.z = hit.f0.z * 0.05f + a.z;
        FUN_004b5080(&m, hit.f10, &a);
        FUN_00477760(0, &m, &col, 2.5f, 10.0f);
        scale.y = 2.0f;
        scale.x = 2.0f;
        scale.z = 1.0f;
        FUN_004c5010(&m, &scale, 1);
        FUN_00475ab0(0, &m, 0);
      } else {
        q.f0 = *FUN_004e6100(p->f30);
        q.f10 = 0x1b;
        q.f14 = p;
        q.f18 = 0;
        q.f1c = FUN_00454320;
        FUN_00484cf0(&q);
        m = p->f34->f10;
        m.f30 = a;
        FUN_004c45f0(&m);
        FUN_004c1480(p->f34, &m, 0);
        p->f10 = a;
        p->f1c.x = DAT_005ce42c.x * DAT_007f100c + p->f1c.x;
        p->f1c.y = DAT_005ce42c.y * DAT_007f100c + p->f1c.y;
        p->f1c.z = DAT_005ce42c.z * DAT_007f100c + p->f1c.z;
        FUN_004c1520(p->f38, &DAT_005f9808, p->f40, 0);
        p->f40 = DAT_007f100c * 720.0f + p->f40;
        p->f3c = DAT_007f100c + p->f3c;
        return;
      }
    }
    p->f2c = 3;
    return;
  }
  if (p->f2c == 3) {
    p->f2c = 4;
    FUN_00476880(&p->f10, &DAT_006146fc, 360.0f, 1.5f);
    p->f0 = p->f10;
    p->fc = 0.1f;
    iVar1 = FUN_004e4440(p->f30);
    if (iVar1 != 0) {
      FUN_004e43b0(iVar1, p->f30);
    }
    return;
  }
  if (p->f2c == 4) {
    p->fc = 3.0f;
    FUN_00453eb0(p, 40.0f, 150.0f, 200.0f, 2);
    p->f2c = 5;
    return;
  }
  if (p->f2c == 5) {
    p->f2c = 0;
    FUN_00454170(p);
  }
}
