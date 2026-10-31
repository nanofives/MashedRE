// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041e6c0 (385 B): best 98.8% (regs 98.8%), by hand. Raw g2s was 32.1%.
   This file also holds five static callees, each BYTE-IDENTICAL (100.0%, regs 100.0%):
   0x0041e150 (raw 90.9%), 0x0041e170 (raw 57.1%), 0x0041e1c0 (raw 45.5%), 0x0041e1e0 (raw did not compile),
   0x0041e4b0 (raw 95.6%).
   Register-passed arguments (pilot finding 1): all five are `static`, defined before their caller, /Ob1.
   0x0041e150 (src EAX, dst stack), 0x0041e170 (value EAX, out EDI), 0x0041e1c0 (a EAX, b ECX, float stack,
   out ESI; parameter order (a, b, f, out)), 0x0041e1e0 (record EBX), 0x0041e4b0 (record EAX). 0x0041e6c0 loads
   `mov ebx, 0x0063d610` / `mov eax, ebx` for the last two.
   Callee arity (pilot finding 3): Ghidra gave FUN_00429a90 two args; it takes one. The float pushed for
   FUN_0041e1c0 is FUN_00429a70's argument slot reused (`push 1; call; fstp [esp]`). That fixed the C2708.
   0x0041e1e0: the 12->11 dword frame needed the two compare temporaries declared inside each if block (the
   compiler then packs the FUN_00429a90 spill into one of their slots), `<= 0x46` (cmp 0x46 / jg), `n & 0x80` for
   the `mov al; test al, al; jns` test, d computed before the state store, and the second block comparing
   (old < new) with its own operand order.
   0x0041e170: quotient and remainder computed as q = h / 6000, (h - q * 6000) / 100 (h % 6000 gave idiv).
   0x0041e6c0: the two translation vectors as one RwV3d[2] (separate vectors swapped their stack slots, 90.1%);
   the identity block is RW's RwMatrixSetIdentity macro (chained stores, flags |= 0x20003).
   Residual 0x0041e6c0 (class b): one instruction, the tail-merged `fstp [esp+0x14]` (scale.z) is scheduled
   before `add esp, 0xc` instead of after `push edx`. Not moved by: separate globals for 0x0063d7c0/d7d8/d7dc,
   scale inside the offsets array, `x = x * k`, aggregate initializers, scale init order, flags-first, inverted
   condition, scaling before the translate call, a frame local; permuter 372 compiles over two runs.
   Globals: 0x0063d7c0, 0x0063d7d8, 0x0063d7dc are fields +0x1b0, +0x1c8, +0x1cc of the record at 0x0063d610.
   DAT_0063d5f8 is .bss (6 dwords up to 0x0063d610, indexed by the state 0..4). Same record layout as
   re/match/s04/0041e850.c. */
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
typedef struct RpMaterial { void *texture; unsigned int color; } RpMaterial;
typedef struct RpGeometry { char pad0[0x20]; RpMaterial **materials; } RpGeometry;
typedef struct RpAtomic RpAtomic;
typedef RpAtomic *(*RpAtomicCallBackRender)(RpAtomic *);
struct RpAtomic { char pad0[0x18]; RpGeometry *geometry; char pad1c[0x48 - 0x1c]; RpAtomicCallBackRender renderCallBack; };
typedef struct RwV3d { float x, y, z; } RwV3d;
typedef struct RwMatrix { RwV3d right; unsigned int flags; RwV3d up; unsigned int pad1; RwV3d at; unsigned int pad2; RwV3d pos; unsigned int pad3; } RwMatrix;
typedef struct RwFrame RwFrame;
typedef struct Mgr {
  RpAtomic *digit[6][11];
  RpAtomic *a108[10];
  char pad130[0x16c - 0x130];
  RpAtomic *a16c;
  RpAtomic *a170;
  RpAtomic *a174;
  RpAtomic *a178;
  char pad17c[0x188 - 0x17c];
  int cur[3];
  int t194[3];
  char pad1a0[0x1b0 - 0x1a0];
  RwFrame *frame;
  int state;
  int laststate;
  int n1bc;
  int n1c0;
  int n1c4;
  int n1c8;
  int n1cc;
  int n1d0;
} Mgr;
MGLOBAL(Mgr, DAT_0063d610, 0x0063d610);
MGLOBALA(unsigned int, DAT_0063d5f8, 6, 0x0063d5f8);
int FUN_0040e350(void);
int FUN_00411580(int, float *, int *, int *);
int FUN_004115c0(int, float *, int *, int *);
float FUN_00429a70(int);  /* float10 return */
int FUN_00429a80(int);
int FUN_00429a90(int);
RwMatrix *FUN_004c51a0(RwMatrix *, RwV3d *, int);
RwFrame *FUN_004c1480(RwFrame *, RwMatrix *, int);
RwFrame *FUN_004c13e0(RwFrame *, RwV3d *, int);

// MATCH: 0x0041e150 FUN_0041e150
static void FUN_0041e150(int *t, int *out)
{
  *out = (t[0] * 60 + t[1]) * 100 + t[2];
}

// MATCH: 0x0041e170 FUN_0041e170
static void FUN_0041e170(int h, int *t)
{
  int q = h / 6000;
  int s = (h - q * 6000) / 100;
  t[0] = q;
  t[2] = h - (q * 60 + s) * 100;
  t[1] = s;
}

// MATCH: 0x0041e1c0 FUN_0041e1c0
static void FUN_0041e1c0(int a, int b, float f, int *t)
{
  t[0] = a;
  t[1] = b;
  t[2] = (int)(f * 100.0f);
}

// MATCH: 0x0041e1e0 FUN_0041e1e0
static void FUN_0041e1e0(Mgr *m)
{
  float f;
  int b;
  int c;
  int t1[3];
  int t2[3];
  int d;

  b = 0;
  c = 0;
  f = 0;
  if (m->n1c0 <= 0x50) {
    m->state = 4;
    FUN_0041e1c0(FUN_00429a80(1), FUN_00429a90(1), FUN_00429a70(1), m->cur);
    m->n1c0++;
    return;
  }
  FUN_0041e1c0(FUN_00429a80(0), FUN_00429a90(0), FUN_00429a70(0), t1);
  m->cur[0] = t1[0];
  m->cur[1] = t1[1];
  m->cur[2] = t1[2];
  m->state = 0;
  if (m->n1c8 == 0 && FUN_004115c0(0, &f, &b, &c)) {
    m->n1c8 = 1;
    m->n1bc = 0;
  } else if (m->n1c8 == 1 && FUN_004115c0(1, &f, &b, &c)) {
    m->n1c8 = 2;
    m->n1bc = 0;
  }
  if (m->n1bc == 0)
    FUN_0041e1c0(c, b, f, m->t194);
  if (m->n1bc <= 0x46 && m->n1c8 > 0) {
    m->n1bc++;
    if (FUN_00411580(m->n1c8 - 1, &f, &b, &c)) {
      int x;
      int y;
      FUN_0041e1c0(c, b, f, t2);
      FUN_0041e150(m->t194, &x);
      FUN_0041e150(t2, &y);
      if (x < y) {
        d = y - x;
        m->state = 2;
      } else if (x > y) {
        d = x - y;
        m->state = 1;
      } else {
        d = 0;
        m->state = 3;
      }
      FUN_0041e170(d, m->cur);
    }
  }
  if (FUN_0040e350() == 7) {
    if (m->n1c4 & 0x80) {
      int x;
      int y;
      FUN_0041e1c0(FUN_00429a80(1), FUN_00429a90(1), FUN_00429a70(1), t2);
      FUN_0041e150(t2, &x);
      FUN_0041e150(t1, &y);
      if (y < x) {
        d = x - y;
        m->state = 2;
      } else if (y > x) {
        d = y - x;
        m->state = 1;
      } else {
        d = 0;
        m->state = 3;
      }
      FUN_0041e170(d, m->cur);
    }
    if (t1[0] == 9 && t1[1] == 59 && t1[2] == 99) {
      m->cur[0] = -1;
      m->cur[1] = -1;
      m->cur[2] = -1;
    }
    m->n1c4++;
  }
}

// MATCH: 0x0041e4b0 FUN_0041e4b0
static void FUN_0041e4b0(Mgr *m)
{
  int s = m->state;
  if (s != m->laststate) {
    m->laststate = s;
    m->a108[0]->geometry->materials[0]->color = DAT_0063d5f8[s];
    m->a108[1]->geometry->materials[0]->color = DAT_0063d5f8[m->state];
    m->a108[2]->geometry->materials[0]->color = DAT_0063d5f8[m->state];
    m->a108[3]->geometry->materials[0]->color = DAT_0063d5f8[m->state];
    m->a108[4]->geometry->materials[0]->color = DAT_0063d5f8[m->state];
    m->a108[5]->geometry->materials[0]->color = DAT_0063d5f8[m->state];
    m->a108[6]->geometry->materials[0]->color = DAT_0063d5f8[m->state];
    m->a108[7]->geometry->materials[0]->color = DAT_0063d5f8[m->state];
    m->a108[8]->geometry->materials[0]->color = DAT_0063d5f8[m->state];
    m->a108[9]->geometry->materials[0]->color = DAT_0063d5f8[m->state];
    m->a178->geometry->materials[0]->color = DAT_0063d5f8[m->state];
    m->a174->geometry->materials[0]->color = DAT_0063d5f8[m->state];
  }
}

// MATCH: 0x0041e6c0 FUN_0041e6c0
void FUN_0041e6c0(void)
{
  RwV3d off[2] = {{-0.375f, -0.3f, 1.0f}, {0.35f, 0.25f, 1.0f}};
  RwV3d scale = {1.0f, 1.0f, 1.0f};
  RwMatrix m;

  DAT_0063d610.n1cc = DAT_0063d610.n1c8;
  m.right.x = m.up.y = m.at.z = 1.0f;
  m.right.y = m.right.z = m.up.x = 0.0f;
  m.up.z = m.at.x = m.at.y = 0.0f;
  m.pos.x = m.pos.y = m.pos.z = 0.0f;
  m.flags = m.flags | 0x20003;
  if (FUN_0040e350() == 7) {
    FUN_004c51a0(&m, &off[1], 0);
    scale.x *= 1.5f;
    scale.y *= 1.5f;
    scale.z *= 1.5f;
  } else {
    FUN_004c51a0(&m, &off[0], 0);
    scale.x *= 1.125f;
    scale.y *= 1.125f;
    scale.z *= 1.125f;
  }
  FUN_004c1480(DAT_0063d610.frame, &m, 0);
  FUN_004c13e0(DAT_0063d610.frame, &scale, 1);
  FUN_0041e1e0(&DAT_0063d610);
  FUN_0041e4b0(&DAT_0063d610);
}
