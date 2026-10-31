// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041c410 (1294 B) + caller 0x0041cc50 (101 B), by hand. This file holds 0x0041cc50.
   0x0041c410: best 99.4% (regs 99.4, ops 99.7), 308/308 ins. Raw: 29.5%. 0x0041cc50: 100.0% (raw 96.0%).
   0x0041cc50 calls 0x0041c410 with the row record in ESI (Ghidra unaff_ESI): pilot finding 1, 0x0041c410 is
   `static`, defined before its only caller. Row record stride 0x114, 4 rows at 0x0063ce20 (same array as
   0x0041cb80 / 0x0041c380 / 0x0041ccc0): +0x000 int out[32], +0x080 atomic pointers [32] (atomic + 4 = frame),
   +0x104 frame, +0x108 idx, +0x10c mask, +0x110 flag. DAT_0063d270 is unsigned (fild + 2^32 fixup at 0x0041c8b0).
   Constants: 0x005cd118 = 1.125f (literal), 0x005cc9c0 = 0.2f, 0x005cc8f0 = 0.15f, 0x005cc320 = 1.0f.
   What moved it: struct + mask updates as `if (tbl[idx]) mask |= b; else mask &= ~b;` (76.9%); the 12 two-bit
   cases of the 32-entry switch written `if (!(A) || !(B)) goto zero; v = 1; break;` with one `zero: v = 0;
   break;` after `case 0: case 0x19:` (99.4%; the `v = A && B` / if-else / ternary forms let the compiler
   cross-jump the `mov eax,1` tails, the original keeps one per case). Case bodies in the original's layout
   order 1/2, 3, 0x11, 0x10, 0xf, 0x12, 0x13, 4/5, 6, 0x16, 0x15, 0x14, 0x17, 0x18, 0/0x19, 7..12.
   Residual (2 ins): the same `fld s.y` one-slot scheduling difference as 0x0041c180 in the `scale *= 1.125f;
   0x004c13e0(frame,&s,1)` block (see 0041c180.c for the rewrites tried); permuter 353 compiles, no gain.
   Class (a). relocheck BAD on the 1.125f literal is the shifted offset from that residual (the original
   operand read at our +0x400 is not the same instruction). C++: warning C4162 on `sin` only, no error. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef struct MatchV3d0041c410 { float x, y, z; } MatchV3d0041c410;
typedef struct MatchMatrix0041c410 {
  MatchV3d0041c410 right; unsigned int flags;
  MatchV3d0041c410 up; unsigned int pad1;
  MatchV3d0041c410 at; unsigned int pad2;
  MatchV3d0041c410 pos; unsigned int pad3;
} MatchMatrix0041c410;
typedef struct MatchRow0041c410 {
  int out[32];          /* +0x000 */
  char *atom[32];       /* +0x080; atom + 4 = frame */
  int pad100;           /* +0x100 */
  void *frame;          /* +0x104 */
  int idx;              /* +0x108 */
  unsigned int mask;    /* +0x10c */
  int flag;             /* +0x110 */
} MatchRow0041c410;     /* size 0x114 */
MGLOBALA(int, DAT_0063cdc0, 4, 0x0063cdc0);
MGLOBALA(int, DAT_0063cdd4, 4, 0x0063cdd4);
MGLOBALA(int, DAT_0063cde8, 4, 0x0063cde8);
MGLOBALA(int, DAT_0063cdf8, 4, 0x0063cdf8);
MGLOBALA(int, DAT_0063ce08, 4, 0x0063ce08);
MGLOBALA(MatchRow0041c410, DAT_0063ce20, 4, 0x0063ce20);
MGLOBAL(unsigned int, DAT_0063d270, 0x0063d270);
MGLOBALA(MatchV3d0041c410, DAT_005f337c, 4, 0x005f337c);
MGLOBAL(float, DAT_005cc9c0, 0x005cc9c0);
MGLOBAL(float, DAT_005cc8f0, 0x005cc8f0);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
double sin(double);
#pragma intrinsic(sin)
int FUN_0040b420(int idx);
int FUN_0040b6d0(int idx);
int FUN_0040b890(void);
void FUN_0040b540(int *p);
void FUN_0040b930(int *p);
void FUN_0040b970(int *p);
void FUN_0040b9a0(int *p, int b);
void FUN_0040ba00(int *p);
int FUN_004c13e0(void *frame, MatchV3d0041c410 *v, int combine);
int FUN_004c1480(void *frame, MatchMatrix0041c410 *m, int combine);
void FUN_004c51a0(MatchMatrix0041c410 *m, MatchV3d0041c410 *v, int combine);
// MATCH: 0x0041c410 FUN_0041c410
static void FUN_0041c410(MatchRow0041c410 *o)
{
  MatchV3d0041c410 s;
  MatchMatrix0041c410 m;
  int idx;
  int i;
  int k;
  unsigned int v;
  float f;

  idx = o->idx;
  k = 0;
  s.x = 1.0f;
  s.y = 1.0f;
  s.z = 1.0f;
  if (DAT_0063cde8[idx]) o->mask |= 0x40; else o->mask &= ~0x40;
  if (DAT_0063ce08[idx]) o->mask |= 0x80; else o->mask &= ~0x80;
  if (DAT_0063cdd4[idx]) o->mask |= 0x100; else o->mask &= ~0x100;
  if (DAT_0063cdc0[idx]) o->mask |= 0x200; else o->mask &= ~0x200;
  if ((o->mask & 0x80) && (o->mask & 0x100)) o->mask &= ~0x180;
  if ((o->mask & 0x40) && (o->mask & 0x100)) o->mask &= ~0x100;
  i = FUN_0040b420(idx);
  o->mask &= 0xfffe0fff;
  switch (i) {
  case 2: o->mask |= 0x1000; break;
  case 1: o->mask |= 0x2000; break;
  case -1: o->mask |= 0x8000; break;
  case -2: o->mask |= 0x10000; break;
  default: o->mask |= 0x4000; break;
  }
  for (i = 0; i < 32; i++) {
    v = 0;
    if (o->atom[i]) {
      switch (i) {
      case 1: case 2: v = o->mask & 0x800; break;
      case 3: if (!(o->mask & 0x40) || !(o->mask & 0x800)) goto zero; v = 1; break;
      case 0x11: if (!(o->mask & 0x1000) || !(o->mask & 0x800)) goto zero; v = 1; break;
      case 0x10: if (!(o->mask & 0x2000) || !(o->mask & 0x800)) goto zero; v = 1; break;
      case 0xf: if (!(o->mask & 0x4000) || !(o->mask & 0x800)) goto zero; v = 1; break;
      case 0x12: if (!(o->mask & 0x8000) || !(o->mask & 0x800)) goto zero; v = 1; break;
      case 0x13: if (!(o->mask & 0x10000) || !(o->mask & 0x800)) goto zero; v = 1; break;
      case 4: case 5: v = o->mask & 0x400; break;
      case 6: if (!(o->mask & 0x40) || !(o->mask & 0x400)) goto zero; v = 1; break;
      case 0x16: if (!(o->mask & 0x1000) || !(o->mask & 0x400)) goto zero; v = 1; break;
      case 0x15: if (!(o->mask & 0x2000) || !(o->mask & 0x400)) goto zero; v = 1; break;
      case 0x14: if (!(o->mask & 0x4000) || !(o->mask & 0x400)) goto zero; v = 1; break;
      case 0x17: if (!(o->mask & 0x8000) || !(o->mask & 0x400)) goto zero; v = 1; break;
      case 0x18: if (!(o->mask & 0x10000) || !(o->mask & 0x400)) goto zero; v = 1; break;
      case 0: case 0x19: v = 1; break;
      zero: v = 0; break;
      case 7: v = o->mask & 1; break;
      case 8: v = o->mask & 2; break;
      case 9: v = o->mask & 4; break;
      case 10: v = o->mask & 8; break;
      case 11: v = o->mask & 0x10; break;
      case 12: v = o->mask & 0x20; break;
      }
      o->out[i] = v;
    }
  }
  for (i = 0; i < 4; i++) {
    if (o->idx == DAT_0063cdf8[i]) { k = i; break; }
  }
  m.right.x = m.up.y = m.at.z = 1.0f;
  m.right.y = m.right.z = m.up.x = 0.0f;
  m.up.z = m.at.x = m.at.y = 0.0f;
  m.pos.x = m.pos.y = m.pos.z = 0.0f;
  m.flags |= 0x20003;
  FUN_004c51a0(&m, &DAT_005f337c[k], 0);
  FUN_004c1480(o->frame, &m, 0);
  s.x = s.x * 1.125f;
  s.y = s.y * 1.125f;
  s.z = s.z * 1.125f;
  FUN_004c13e0(o->frame, &s, 1);
  f = (float)FUN_0040b6d0(o->idx);
  s.x = f / (float)FUN_0040b890();
  s.z = 1.0f;
  s.y = 1.0f;
  FUN_004c13e0(*(void **)(o->atom[5] + 4), &s, 0);
  FUN_004c13e0(*(void **)(o->atom[2] + 4), &s, 0);
  if (o->mask & 0x40) {
    s.x = s.y = (float)(sin((float)DAT_0063d270 * DAT_005cc9c0) * DAT_005cc8f0 + DAT_005cc320);
  } else {
    s.x = s.y = 1.0f;
  }
  s.z = 1.0f;
  FUN_004c13e0(*(void **)(o->atom[6] + 4), &s, 0);
  FUN_004c13e0(*(void **)(o->atom[3] + 4), &s, 0);
}
// MATCH: 0x0041cc50 FUN_0041cc50
void FUN_0041cc50(void)
{
  int i;
  FUN_0040b930(DAT_0063cde8);
  FUN_0040b9a0(DAT_0063ce08, 0);
  FUN_0040ba00(DAT_0063cdd4);
  FUN_0040b970(DAT_0063cdc0);
  FUN_0040b540(DAT_0063cdf8);
  for (i = 0; i < 4; i++) {
    if (DAT_0063ce20[i].flag != 0)
      FUN_0041c410(&DAT_0063ce20[i]);
  }
  DAT_0063d270++;
}
