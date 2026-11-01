// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044ecc0: 100% by hand (raw 72.9%). The 0x1c-byte effect slots as Fx[3][10] at 0x00895600 (the
   layout FUN_0044f830 fills), two int index loops (inner one becomes the dec ebx counter, outer the signed jl
   against 0x89595c), slot id read before the obj test, a 64-byte matrix local. */
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
typedef struct { float x, y, z; } V3_0044ecc0;
typedef struct { float m[16]; } Mat_0044ecc0;
typedef struct {
  V3_0044ecc0 pos;  /* 0x00 */
  int h;            /* 0x0c */
  int p10;          /* 0x10 */
  char *obj;        /* 0x14 */
  int id;           /* 0x18 */
} Fx_0044ecc0;
MGLOBAL(int, DAT_00684b5c, 0x00684b5c);
typedef Fx_0044ecc0 FxRow_0044ecc0[10];
MGLOBALA(FxRow_0044ecc0, DAT_00895600, 3, 0x00895600);
void FUN_0047d180(int p0, Mat_0044ecc0 *p1);
void FUN_004b5240(int p0, int p1);
void FUN_004c51a0(Mat_0044ecc0 *p0, V3_0044ecc0 *p1, int p2);
// MATCH: 0x0044ecc0 FUN_0044ecc0
void FUN_0044ecc0(void)
{
  int i;
  int j;
  int id;
  Fx_0044ecc0 *e;
  Mat_0044ecc0 m;

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 10; j++) {
      e = &DAT_00895600[i][j];
      id = e->id;
      if (e->obj != 0) {
        *(int *)(e->obj + 0xf0) = 0;
        e->obj = 0;
      }
      FUN_004b5240(e->h, 0);
      FUN_004c51a0(&m, &e->pos, 0);
      FUN_0047d180(id, &m);
    }
  }
  DAT_00684b5c = 0;
}
