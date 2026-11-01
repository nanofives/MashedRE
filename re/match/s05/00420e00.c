// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00420e00 (594 B): 100.0% (regs 100, ops 100), by hand. This file also holds its caller 0x004215a0
   (8 instructions, 100.0%; 004215a0.c is the raw 100% source, which calls a `void` prototype and matches only
   because the record pointer happens to sit in ESI). 0x00420e00 takes the record in ESI (Ghidra unaff_ESI):
   pilot finding 1, `static`, defined before 0x004215a0. Per channel j = 0, 1: report (int)v[j] to 0x0046be50,
   then `if (a[j] == 0 && v >= 17.5) {...} else if (b[j] == 0 && v >= 10.0) {... 8 x 0x00490020}`; 17.5/10.0 read
   through MGLOBAL. DAT_0063e4b8 = 4 records of 0x24 bytes.
   Residual: none. Relocheck OK. */
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
typedef struct V3_00420e00 { float x, y, z; } V3_00420e00;
typedef struct Mat_00420e00 { float m[12]; V3_00420e00 pos; float pad; } Mat_00420e00;
typedef struct RGBA_00420e00 { unsigned char r, g, b, a; } RGBA_00420e00;
MGLOBAL(const float, DAT_005cd2b0, 0x005cd2b0);
MGLOBAL(const float, DAT_005cc55c, 0x005cc55c);
void FUN_0046be50(int id, int ch, int val, float f);
int FUN_0041f1c0(int id, int n);
void FUN_0041f1e0(int id, int n, Mat_00420e00 *m);
void FUN_0048f590(V3_00420e00 *pos, float k);
void FUN_00490020(V3_00420e00 *pos, RGBA_00420e00 *col, int id, int p3, int p4, float p5, int p6);
void FUN_0041ede0(int id, int a0, int a1);
void FUN_0041ee50(int id, int b0, int b1);
// MATCH: 0x00420e00 FUN_00420e00
static void FUN_00420e00(Rec0063e4b8 *r)
{
  float v;
  RGBA_00420e00 col;
  V3_00420e00 pos;
  Mat_00420e00 m;
  int j;
  col.r = 0xff;
  col.g = 0xff;
  col.b = 0xff;
  col.a = 0x40;
  v = r->v[0];
  FUN_0046be50(r->id, 5, (int)v, -1.0f);
  if (r->a[0] == 0 && v >= DAT_005cd2b0) {
    r->a[0] = 1;
    r->b[0] = 1;
    FUN_0041f1c0(r->id, 0x33);
    FUN_0041f1e0(r->id, 0x33, &m);
    pos = m.pos;
    FUN_0048f590(&pos, 2.0f);
  } else if (r->b[0] == 0 && v >= DAT_005cc55c) {
    r->b[0] = 1;
    FUN_0041f1c0(r->id, 0xe);
    FUN_0041f1e0(r->id, 0xe, &m);
    pos = m.pos;
    for (j = 0; j < 8; j++)
      FUN_00490020(&pos, &col, r->id, 0, 0, 1.0f, 0);
  }
  v = r->v[1];
  FUN_0046be50(r->id, 4, (int)v, -1.0f);
  if (r->a[1] == 0 && v >= DAT_005cd2b0) {
    r->a[1] = 1;
    r->b[1] = 1;
    FUN_0041f1c0(r->id, 0x34);
    FUN_0041f1e0(r->id, 0x34, &m);
    pos = m.pos;
    FUN_0048f590(&pos, 2.0f);
  } else if (r->b[1] == 0 && v >= DAT_005cc55c) {
    r->b[1] = 1;
    FUN_0041f1c0(r->id, 0xf);
    FUN_0041f1e0(r->id, 0xf, &m);
    pos = m.pos;
    for (j = 0; j < 8; j++)
      FUN_00490020(&pos, &col, r->id, 0, 0, 1.0f, 0);
  }
  FUN_0041ede0(r->id, r->a[0], r->a[1]);
  FUN_0041ee50(r->id, r->b[0], r->b[1]);
}
// MATCH: 0x004215a0 FUN_004215a0
void FUN_004215a0(void)
{
  int i;
  for (i = 0; i < 4; i++)
    FUN_00420e00(&DAT_0063e4b8[i]);
}
