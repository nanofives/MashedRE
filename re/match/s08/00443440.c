// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00443440: 58.4% (regs 67.5%, ops 94.1%) by hand (h7) + permuter (local declaration and statement
   order). Raw 12.0%. Ghidra's size (2225 B) stops at 0x00443cf1, before the real
   end (0x00443d0d). Nearest segment, ternary refinement (16 steps) with FUN_00443300, then three
   FUN_004233e0 headings 0.01 past the start, past dist/2 and past dist (squared step lengths), optional
   debug lines through FUN_004b55a0(FUN_004671a0(0), ...).
   Residual (b): control flow, loops and calls line up (ops 94.1%); the rest is register and stack-slot
   assignment. The original keeps path/pos/seg in esi/edi/ebx (ours edi/ebx/esi) and packs many locals into
   shared slots (frame 0x154, ours larger). The squared distance stores v[1] with fst and multiplies from memory,
   ours stores and reloads. Tried: struct and scalar forms of v (scalars drop it to registers), an __inline
   length helper, operand order, int declaration order; permuter 1322 compiles. Budget reached. */
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
typedef struct {
  float x;
  float z;
} Pt_00443440;
typedef struct {
  Pt_00443440 pt[64];
  int count;
} Path_00443440;
typedef struct {
  uchar r;
  uchar g;
  uchar b;
  uchar a;
} RGBA_00443440;
MGLOBAL(float, DAT_007f1a58, 0x007f1a58);
MGLOBAL(float, DAT_007f1a5c, 0x007f1a5c);
void FUN_00443300(Path_00443440 *p0, int p1, float p2, float *p3);
float FUN_004233e0(float p0, float p1);
int FUN_004671a0(int p0);
void FUN_004b55a0(int p0, float *p1, float *p2, RGBA_00443440 *p3);
// MATCH: 0x00443440 FUN_00443440
float FUN_00443440(Path_00443440 *param_1, float *param_2, float param_3, float *param_4, int param_5)
{
  RGBA_00443440 col;
  float v[2];
  float e[2];
  float p[2];
  float a[2];
  float b[2];
  float c[2];
  float q[2];
  float prev[2];
  float org[2];
  float dsq[64];
  float dcur;
  float best;
  float d;
  float lo;
  float dhi;
  float hi;
  float dlo;
  float t;
  float acc;
  float half;
  float ang0;
  float ang2;
  float r;
  float ang1;
  float a1;
  int seg;
  int seg2;
  int prevseg;
  int i;
  float a2;
  int n;

  col.r = 0xff;
  col.g = 0xff;
  col.b = 0;
  col.a = 0xff;
  if (param_5 == 1) {
    org[0] = DAT_007f1a58 + 4.0f;
    org[1] = DAT_007f1a5c - 4.0f;
  }
  seg = 0;
  best = 100000.0f;
  for (i = 0; i < param_1->count; i++) {
    v[0] = param_1->pt[i].x - param_2[0];
    v[1] = param_1->pt[i].z - param_2[1];
    d = v[1] * v[1] + v[0] * v[0];
    dsq[i] = d;
    if (d < best) {
      best = d;
      seg = i;
    }
  }
  FUN_00443300(param_1, seg, 0.01f, p);
  v[0] = p[0] - param_2[0];
  prevseg = seg - 1;
  v[1] = p[1] - param_2[1];
  dcur = v[1] * v[1] + v[0] * v[0];
  if (prevseg < 0) {
    prevseg = param_1->count - 1;
  }
  FUN_00443300(param_1, prevseg, 0.99f, q);
  v[0] = q[0] - param_2[0];
  v[1] = q[1] - param_2[1];
  if (v[1] * v[1] + v[0] * v[0] < dcur) {
    p[0] = q[0];
    p[1] = q[1];
    seg = prevseg;
  }
  hi = 1.0f;
  FUN_00443300(param_1, seg, 1.0f, a);
  v[0] = a[0] - param_2[0];
  v[1] = a[1] - param_2[1];
  lo = 0.0f;
  dhi = v[1] * v[1] + v[0] * v[0];
  n = 16;
  FUN_00443300(param_1, seg, 0.0f, b);
  v[0] = b[0] - param_2[0];
  v[1] = b[1] - param_2[1];
  dlo = v[1] * v[1] + v[0] * v[0];
  do {
    if (dhi > dlo) {
      hi = (hi + hi + lo) * 0.333333f;
      FUN_00443300(param_1, seg, hi, a);
      v[0] = a[0] - param_2[0];
      v[1] = a[1] - param_2[1];
      dhi = v[1] * v[1] + v[0] * v[0];
    } else {
      lo = (lo + lo + hi) * 0.333333f;
      FUN_00443300(param_1, seg, lo, b);
      v[0] = b[0] - param_2[0];
      v[1] = b[1] - param_2[1];
      dlo = v[1] * v[1] + v[0] * v[0];
    }
    n--;
  } while (n != 0);
  if (dhi > dlo) {
    t = lo;
    c[0] = b[0];
    c[1] = b[1];
  } else {
    t = hi;
    c[0] = a[0];
    c[1] = a[1];
  }
  hi = t + 0.01f;
  seg2 = seg;
  if (hi > 1.0f) {
    hi -= 1.0f;
    seg2 = seg + 1;
    if (seg2 == param_1->count) {
      seg2 = 0;
    }
  }
  FUN_00443300(param_1, seg2, hi, p);
  b[0] = p[0] - c[0];
  b[1] = p[1] - c[1];
  ang0 = FUN_004233e0(b[0], b[1]);
  prev[1] = c[1];
  prev[0] = c[0];
  half = param_3 * 0.5f;
  acc = 0.0f;
  hi = t;
  while (acc < half) {
    hi += 0.1f;
    if (hi > 1.0f) {
      hi -= 1.0f;
      seg++;
      if (seg == param_1->count) {
        seg = 0;
      }
    }
    FUN_00443300(param_1, seg, hi, p);
    b[0] = p[0] - prev[0];
    b[1] = p[1] - prev[1];
    prev[1] = p[1];
    prev[0] = p[0];
    acc += b[0] * b[0] + b[1] * b[1];
    if (param_5 != 0) {
      col.b = 0xff;
      col.g = 0;
      col.r = 0;
      e[0] = (param_2[1] - org[1]) * 0.125f;
      e[1] = (org[0] - param_2[0]) * 0.125f;
      v[0] = (p[1] - org[1]) * 0.125f;
      v[1] = (org[0] - p[0]) * 0.125f;
      if (v[0] > -1.0f && e[0] > -1.0f && v[0] < 2.0f && e[0] < 2.0f && v[1] > -1.0f && e[1] > -1.0f &&
          v[1] < 2.0f && e[1] < 2.0f) {
        FUN_004b55a0(FUN_004671a0(0), v, e, &col);
      }
    }
  }
  hi += 0.01f;
  if (hi > 1.0f) {
    hi -= 1.0f;
    seg++;
    if (seg == param_1->count) {
      seg = 0;
    }
  }
  FUN_00443300(param_1, seg, hi, p);
  b[0] = p[0] - prev[0];
  b[1] = p[1] - prev[1];
  ang1 = FUN_004233e0(b[0], b[1]);
  while (acc < param_3) {
    hi += 0.1f;
    if (hi > 1.0f) {
      hi -= 1.0f;
      seg++;
      if (seg == param_1->count) {
        seg = 0;
      }
    }
    FUN_00443300(param_1, seg, hi, p);
    b[1] = p[1] - prev[1];
    prev[0] = p[0];
    prev[1] = p[1];
    acc += b[0] * b[0] + b[1] * b[1];
    b[0] = p[0] - prev[0];
    if (param_5 != 0) {
      col.g = 0;
      col.r = 0;
      col.b = 0xff;
      e[0] = (param_2[1] - org[1]) * 0.125f;
      e[1] = (org[0] - param_2[0]) * 0.125f;
      v[0] = (p[1] - org[1]) * 0.125f;
      v[1] = (org[0] - p[0]) * 0.125f;
      if (v[0] > -1.0f && e[0] > -1.0f && v[0] < 2.0f && e[0] < 2.0f && v[1] > -1.0f && e[1] > -1.0f &&
          v[1] < 2.0f && e[1] < 2.0f) {
        FUN_004b55a0(FUN_004671a0(0), v, e, &col);
      }
    }
  }
  hi += 0.01f;
  if (hi > 1.0f) {
    hi -= 1.0f;
    seg++;
    if (seg == param_1->count) {
      seg = 0;
    }
  }
  FUN_00443300(param_1, seg, hi, p);
  b[0] = p[0] - prev[0];
  b[1] = p[1] - prev[1];
  ang2 = FUN_004233e0(b[0], b[1]);
  r = ang0 - ang2;
  while (r < 0.0f) {
    r += 360.0f;
  }
  *param_4 = r;
  a1 = ang0 - ang1;
  a2 = ang1 - ang2;
  while (a1 < 0.0f) {
    a1 += 360.0f;
  }
  while (a2 < 0.0f) {
    a2 += 360.0f;
  }
  if (a1 > 180.0f) {
    a1 = -(360.0f - a1);
  }
  if (a2 > 180.0f) {
    a2 = -(360.0f - a2);
  }
  if (a2 + a1 == 0.0f) {
    return 0.0f;
  }
  return a1 / (a2 + a1);
}
