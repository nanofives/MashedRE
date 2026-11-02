// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00421100 (83 B): 100.0% (regs 100, ops 100), by hand. Same file text as the file of 0x00421160 (327 B),
   both 100.0%. This file holds 0x00420d80, 0x00421100 and 0x00421160. 0x00421100 takes the owner index in EAX and the colour
   pointer in ECX (Ghidra extraout_*): pilot finding 1, `static`, defined before its caller 0x00421160. New: it only
   keeps the pointer in ECX across its call to 0x00420d80 when 0x00420d80 is DEFINED EARLIER IN THE SAME FILE (the
   compiler then knows 0x00420d80 leaves ECX alone); with only a prototype it moves the pointer to ESI (58.6%).
   0x00420d80 is therefore in this file too (its MATCH line is the real function, same body as 00420d80.c), and
   0x00421160 then also keeps the random scale on the x87 stack across the static call (59.8% -> 100%).
   DAT_0063e588 = 128 smoke records of 0x2c bytes, count at 0x0063fb88; 12.5/25.0/0.2/0.3 read through MGLOBAL.
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
typedef struct V3_00421160 { float x, y, z; } V3_00421160;
MGLOBALA(unsigned char, DAT_0063e4a8, 16, 0x0063e4a8);
MGLOBAL(const float, DAT_005cd2b4, 0x005cd2b4);
MGLOBAL(const float, DAT_005cc9e0, 0x005cc9e0);
MGLOBAL(const float, DAT_005cc9c0, 0x005cc9c0);
MGLOBAL(const float, DAT_005cc99c, 0x005cc99c);
// MATCH: 0x00420d80 FUN_00420d80
float FUN_00420d80(int i)
{
  Rec0063e4b8 *r = &DAT_0063e4b8[i];
  int j;
  float s = 0.0f;
  for (j = 0; j < 2; j++)
    s += r->v[j];
  return s;
}
float FUN_00472650(float lo, float hi);
int FUN_00472690(int lo, int hi);
// MATCH: 0x00421100 FUN_00421100
static void FUN_00421100(int owner, unsigned char *col)
{
  float t = FUN_00420d80(owner);
  if (t < DAT_005cd2b4) {
    col[0] = 0xdc;
    col[1] = 0xdc;
    col[2] = 0xdc;
    col[3] = 0x78;
  } else if (t < DAT_005cc9e0) {
    col[0] = 0x80;
    col[1] = 0x80;
    col[2] = 0xa0;
    col[3] = 0xc8;
  } else {
    col[0] = 0x28;
    col[1] = 0x28;
    col[2] = 0x28;
    col[3] = 0xf0;
  }
}
// MATCH: 0x00421160 FUN_00421160
void FUN_00421160(V3_00421160 *pos, int owner)
{
  Smoke0063e588 *p = &DAT_0063e588[DAT_0063fb88];
  float s;
  p->age = 0.0f;
  *(V3_00421160 *)p->pos = *pos;
  p->owner = owner;
  s = FUN_00472650(0.3f, 1.16f);
  FUN_00421100(owner, p->col);
  p->col[0] = (unsigned char)(p->col[0] * s);
  p->col[1] = (unsigned char)(p->col[1] * s);
  p->col[2] = (unsigned char)(p->col[2] * s);
  p->col[3] += (char)FUN_00472690(-15, 15);
  p->sx = FUN_00472650(0.8f, 1.2f) * DAT_005cc9c0;
  p->sy = FUN_00472650(0.8f, 1.2f) * DAT_005cc99c;
  p->f20 = FUN_00472650(1.1f, 1.5f);
  p->spin = FUN_00472650(-0.01f, 0.01f);
  p->ang = FUN_00472650(-3.1415927f, 3.1415927f);
  DAT_0063fb88++;
  DAT_0063e4a8[DAT_0063fb88 >> 3] |= 1 << (DAT_0063fb88 & 7);
  if (DAT_0063fb88 >= 128)
    DAT_0063fb88 = 0;
}
