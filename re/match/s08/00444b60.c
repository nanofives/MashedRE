// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00444b60: 69.0% (regs 69.0%, ops 98.8%) by hand (h8), raw 10.8%. All 336 instructions in the original
   order; what is left is frame offsets and two x87 pops. What moved it: the spline as { float x, y } pt[64]; int n
   (0x204 B), the Catmull-Rom point p3 read as s->pt[k].x / s->pt[k].y (an explicit p3 pointer keeps s in a
   register; the original reloads it), p0 as &s->pt[i] with a 0-based loop (gives the inc/store/dec counter),
   the y result folded into the b[0] store.
   Residual (b): frame layout. Original slots a +0x10, t +0x18, b +0x1c, col +0x24, k +0x28, i +0x2c, offx +0x30,
   offy +0x34, &pt[k].y +0x38, rec +0x3c, px +0x40, buf +0x48 (free +0x0c and +0x44); ours start at +0x0c with offy
   below offx and buf at +0x40. Declaration order, block scopes and buf size do not move it. Also the original
   pops t*t at its last use inside the y expression; ours keeps t*t and t*t*t to the end of the statement
   (named t2/t3, inline t*t, t*(t*t) all compile the same). Permuter 1180 compiles: 69.6% with shuffled code,
   not kept. */
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
typedef struct { float x; float y; } SplinePt00444b60;
typedef struct { SplinePt00444b60 pt[64]; int n; } Spline00444b60;
typedef int (*SprintfFn00444b60)(char *, const char *, ...);
MGLOBAL(float, DAT_007f1a58, 0x007f1a58);
MGLOBAL(float, DAT_007f1a5c, 0x007f1a5c);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007f09e4, 0x007f09e4);
MGLOBALA(char, s_d_005cd794, 3, 0x005cd794);
void FUN_00443dc0(Spline00444b60 *s, float *a, float *b, int p3, int p4, int p5);
int FUN_004671a0(int p0);
void FUN_0046d4a0(int *out, int p1);
void FUN_004b55a0(int h, float *a, float *b, unsigned char *col);
int FUN_005586f0(int p0, char *text, int x, int y, int p4);
// MATCH: 0x00444b60 FUN_00444b60
void FUN_00444b60(Spline00444b60 *s)
{
  float a[2];
  float t;
  float b[2];
  unsigned char col[4];
  int k;
  int i;
  float offx;
  float offy;
  float *p3y;
  int rec;
  float px;
  float t2;
  float t3;
  int j;
  char buf[260];
  SplinePt00444b60 *p0;
  SplinePt00444b60 *p1;
  SplinePt00444b60 *p2;

  col[0] = 0xff;
  col[1] = 0;
  col[2] = 0xff;
  col[3] = 0xff;
  FUN_0046d4a0(&rec, 0);
  a[0] = *(float *)(rec + 0x30);
  a[1] = *(float *)(rec + 0x38);
  FUN_00443dc0(s, a, b, 0, 1, 1);
  offx = DAT_007f1a58 + 4.0f;
  offy = DAT_007f1a5c - 4.0f;
  for (i = 0; i < s->n; i++) {
    a[0] = (s->pt[i].y - offy) * 0.125f;
    a[1] = (offx - s->pt[i].x) * 0.125f;
    j = i + 1;
    if (j == s->n) {
      j = 0;
    }
    b[0] = (s->pt[j].y - offy) * 0.125f;
    b[1] = (offx - s->pt[j].x) * 0.125f;
    if (a[0] > -1.0f && b[0] > -1.0f && a[0] < 2.0f && b[0] < 2.0f &&
        a[1] > -1.0f && b[1] > -1.0f && a[1] < 2.0f && b[1] < 2.0f) {
      FUN_004b55a0(FUN_004671a0(0), a, b, col);
    }
    (*(SprintfFn00444b60 *)(DAT_007d3ff8 + 0xc4))(buf, s_d_005cd794, i);
    FUN_005586f0(DAT_007f09e4, buf, (int)(a[0] * 512.0f), (int)(a[1] * 512.0f), 0);
  }
  if (s->n > 3) {
    col[1] = 0xff;
    a[0] = -10000.0f;
    a[1] = -10000.0f;
    for (i = 0; i < s->n; i++) {
        k = i + 1;
        if (k >= s->n) {
          k -= s->n;
        }
        p1 = &s->pt[k];
        k++;
        if (k >= s->n) {
          k -= s->n;
        }
        p2 = &s->pt[k];
        k++;
        if (k >= s->n) {
          k -= s->n;
        }
        p0 = &s->pt[i];
        for (t = 0.0f; t <= 1.0f; t += 0.1f) {
        t2 = t * t;
        t3 = t * t2;
        px = (p1->x + p1->x + (p2->x - p0->x) * t +
              (((p1->x * 3.0f - p0->x) - p2->x * 3.0f) + s->pt[k].x) * t3 +
              ((p2->x * 4.0f + ((p0->x + p0->x) - p1->x * 5.0f)) - s->pt[k].x) * t2) * 0.5f;
        b[0] = ((p1->y + p1->y + (p2->y - p0->y) * t +
              (((p1->y * 3.0f - p0->y) - p2->y * 3.0f) + s->pt[k].y) * t3 +
              ((p2->y * 4.0f + ((p0->y + p0->y) - p1->y * 5.0f)) - s->pt[k].y) * t2) * 0.5f - offy) * 0.125f;
        b[1] = (offx - px) * 0.125f;
        if (a[0] != -10000.0f && a[0] > -1.0f && b[0] > -1.0f && a[0] < 2.0f && b[0] < 2.0f &&
            a[1] > -1.0f && b[1] > -1.0f && a[1] < 2.0f && b[1] < 2.0f) {
          FUN_004b55a0(FUN_004671a0(0), a, b, col);
        }
        a[0] = b[0];
        a[1] = b[1];
      }
    }
  }
}
