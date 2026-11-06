// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047f940 (h7): 100% (regs 100%). Raw 36.5%. 0x110-byte descriptor table at 0x0086cb90 indexed by
   arg 5; 0.1f is a literal (0x005cc56c is .rdata); nested FUN_00559c40(FUN_0055dec0(a3), body, -1, 2);
   FUN_0055b940 returns float (discarded: fstp st(0)). The slot index is kept as a byte offset (n * 4) and the three
   slot arrays are addressed through it; with a plain index the original's hoisted `shl edi, 2` and its ebx/edi
   split did not come out (83.5%). */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3;
typedef struct {
  float f00;
  float f04;
  int pad08[3];
  int f14;
  V3 f18;
  float f24;
  float f28;
  int pad2c[2];
  int f34;
  float f38;
  int pad3c[53];
} Desc110;
typedef struct { int obj; V3 v; int flags; } Shape;
MGLOBAL(int, DAT_006c6eb0, 0x006c6eb0);
MGLOBALA(float, DAT_006c686c, 201, 0x006c686c);
MGLOBALA(int, DAT_006c6b90, 201, 0x006c6b90);
MGLOBALA(int, DAT_006c71d8, 201, 0x006c71d8);
MGLOBALA(int, DAT_006c9758, 201, 0x006c9758);
MGLOBALA(Desc110, DAT_0086cb90, 1, 0x0086cb90);
int FUN_004c0b30(void);
void FUN_004e4380(int p0, int p1);
int FUN_004e69a0(int p0);
void FUN_004e7e30(int p0, int p1);
void FUN_00559c40(int p0, int p1, int p2, int p3);
float FUN_0055b940(Shape *p0, int p1, int p2);
void FUN_0055c4a0(int p0, float p1);
void FUN_0055c4f0(int p0, float p1);
int FUN_0055dec0(int p0);
void FUN_0055e440(int p0, float p1, float p2);
void FUN_0057c220(int p0, int p1);
int FUN_0057c300(Shape *p0);
Shape *FUN_0057c500(void);
void FUN_0057c550(Shape *p0);
// MATCH: 0x0047f940 FUN_0047f940
void FUN_0047f940(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6)
{
  Shape *sh;
  int obj;
  int i;
  int n;
  int body;

  sh = FUN_0057c500();
  obj = sh->obj;
  sh->v = DAT_0086cb90[param_5].f18;
  FUN_0055e440(obj, DAT_0086cb90[param_5].f00 * 0.1f, DAT_0086cb90[param_5].f04 * 0.1f);
  FUN_0055c4f0(obj, DAT_0086cb90[param_5].f24);
  FUN_0055c4a0(obj, DAT_0086cb90[param_5].f28);
  FUN_0055b940(sh, DAT_0086cb90[param_5].f14, 0);
  for (i = 0; i < param_1; i++) {
    body = FUN_0057c300(sh);
    n = DAT_006c6eb0 * 4;  /* byte offset: the original hoists the shl */
    *(int *)((char *)DAT_006c71d8 + n) = FUN_004e69a0(param_6);
    *(int *)((char *)DAT_006c6b90 + n) = DAT_0086cb90[param_5].f34 + i;
    *(int *)((char *)DAT_006c9758 + n) = param_5;
    FUN_004e7e30(DAT_006c71d8[DAT_006c6eb0], FUN_004c0b30());
    FUN_0057c220(DAT_006c71d8[DAT_006c6eb0], body);
    FUN_004e4380(param_4, DAT_006c71d8[DAT_006c6eb0]);
    *(unsigned int *)(body + 8) &= 0xfffffff3;
    FUN_00559c40(FUN_0055dec0(param_3), body, -1, 2);
    DAT_006c6eb0 = DAT_006c6eb0 + 1;
    DAT_006c686c[DAT_006c6eb0] = DAT_0086cb90[param_5].f38 * DAT_0086cb90[param_5].f38;
  }
  FUN_0057c550(sh);
}
