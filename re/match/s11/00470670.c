// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00470670 (h5): best 98.0% (regs 98.0%), 199/199 ins. Raw: did not compile.
   0x00470670 is a static callee: car in EAX (copied to EDI), (i, dt, pad, ctx) on the stack. The real caller
   0x00470c70 (call at 0x00471076, mov eax, ebp) is not in this slice: a stand-in caller sits at the end (no MATCH
   line, #ifndef MATCH_PORT). Its own callees: 0x0046ddb0 (car in EDI) and 0x00467650 (car in ESI) are STAND-INS
   here (no MATCH line, #ifndef MATCH_PORT, shaped only for the convention); 0x00468980 (car in ECX, matrix in
   ESI) is h1's 90.1% body from 00468980.c, copied as the static callee (no MATCH line here).
   Moved the score: .rdata float constants written as literals (65.7 -> 86.2 -> 98.0; with MGLOBAL loads the
   compiler cannot hoist the `== 0` compare above the stores through the car pointer, and `F *= k` loads the
   constant first), the zero test as `if (speed != 0) {...} else {...}` (the original keeps the fnstsw flags across
   the eight zero stores), the 3-term dot as `c + (a + b)` (fld order 9d8*9b4, 9d4*9b0, then 9dc*9b8).
   Constants: 0x005d757c 0.0, 0x005cc320 1.0, 0x005cd6d4 64.0, 0x005ceaa8 1/256, 0x005cc32c 0.5, 0x005cc950 0.75,
   0x005cc9d0 128.0, 0x005ceaa4 6000.0, 0x005cea58 1/6000, 0x005cc348 1.5, 0x005cc9c8 0.9.
   Residual (b): `(float)pad[n] * car[0x190]` (2 x 2 ins): the original does fild of the zero-extended byte then
   fmul [car+0x190]; ours loads car[0x190] first and uses fimul (or, with `f = pad[n]` first, fld + fmulp).
   Tried casts (float)(int), operand order, f = pad; f *= ..., f *= in two steps; permuter (no gain). */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
typedef struct V3_00470670 { float x, y, z; } V3_00470670;
MGLOBAL(unsigned int, DAT_007f101c, 0x007f101c);
int FUN_0040e350(void);

#define F_00470670(o) (*(float *)(car + (o)))
#define I_00470670(o) (*(int *)(car + (o)))

#ifndef MATCH_PORT
/* Stand-ins (no MATCH line) for this TU's static callees, shaped only so cl 9176 gives each the register
   convention of the original call sites. */
extern void stub_call_00470670(int a, int b);
/* 0x0046ddb0: car in EDI, (dt, m, ctx) on the stack */
static void FUN_0046ddb0(float dt, float *m, int ctx, char *car)
{
  int k;
  for (k = 0; k < 4; k++) {
    stub_call_00470670(k, *(int *)(car + 0x198));
    stub_call_00470670(k, *(int *)(car + 0x25c));
    stub_call_00470670(k, k);
  }
  stub_call_00470670((int)m[0], ctx + (int)dt);
}
/* 0x00467650: car in ESI, (i, dt, m, pad) on the stack */
static void FUN_00467650(int i, float dt, float *m, unsigned char *pad, char *car)
{
  stub_call_00470670(*(int *)(car + 0x9b0), *(int *)(car + 0x9d0));
  stub_call_00470670(*(int *)(car + 0x9b4), *(int *)(car + 0x9d4));
  *(float *)(car + 0x9e4) = m[0] * dt;
  *(int *)(car + 0x9e8) = i + pad[0];
}
#else
void FUN_0046ddb0(float dt, float *m, int ctx, char *car);
void FUN_00467650(int i, float dt, float *m, unsigned char *pad, char *car);
#endif

/* 0x00468980: car in ECX, matrix in ESI (h1's matched body in 00468980.c; prototype only here) */
#ifndef MATCH_PORT
MGLOBAL(V3_00470670, DAT_006146f0, 0x006146f0);
MGLOBAL(V3_00470670, DAT_006146fc, 0x006146fc);
MGLOBAL(V3_00470670, DAT_00614708, 0x00614708);
double FUN_004a3384(double x);
float FUN_004c39b0(V3_00470670 *out, V3_00470670 *in);
void *FUN_004c4d20(float *m, V3_00470670 *axis, float angle, int combine);
static void FUN_00468980(int car, float *m, float param_3, int param_4)
{
  float s;
  float d;
  float ang;
  V3_00470670 v;
  V3_00470670 a;

  if (*(float *)(car + 0x9e0) == 0.0f) {
    if (*(int *)(car + 0x9f0) == 0) {
      s = param_3 * 0.001f;
      d = m[9] * DAT_006146fc.y + m[8] * DAT_006146fc.x + m[10] * DAT_006146fc.z;
      if (d < -1.0f) d = -1.0f;
      else if (d > 1.0f) d = 1.0f;
      ang = (float)-(FUN_004a3384(d) * 57.29578f - 90.0) * s;
      FUN_004c4d20(m, &DAT_006146f0, ang, 1);
      d = m[1] * DAT_006146fc.y + DAT_006146fc.x * m[0] + m[2] * DAT_006146fc.z;
      if (d < -1.0f) d = -1.0f;
      else if (d > 1.0f) d = 1.0f;
      ang = ((float)(FUN_004a3384(d) * 57.29578f) - 90.0f) * s;
      FUN_004c4d20(m, &DAT_00614708, ang, 1);
      return;
    }
    *(int *)(car + 0x9c4) = 0;
    *(int *)(car + 0x9c0) = 0;
    *(int *)(car + 0x9bc) = 0;
    v = *(V3_00470670 *)(car + 0x9b0);
    FUN_004c39b0(&v, &v);
    a.x = v.z;
    a.y = v.y;
    a.z = v.x;
    FUN_004c4d20(m, &a, param_3 * 0.05f, 1);
  }
}
#else
void FUN_00468980(int car, float *m, float param_3, int param_4);
#endif

// MATCH: 0x00470670 FUN_00470670
static void FUN_00470670(int i, float dt, unsigned char *pad, int ctx, char *car)
{
  int mode;
  float *m;
  unsigned int k;
  float f;
  float g;
  float d;

  mode = FUN_0040e350();
  m = (float *)(car + I_00470670(0x9a8) * 0x40 + 0x928);
  I_00470670(0xb20) = 0;
  I_00470670(0xb1c) = 0;
  I_00470670(0xb18) = 0;
  I_00470670(0xb14) = 0;
  I_00470670(0x3f4) = 0;
  I_00470670(0x330) = 0;
  I_00470670(0x26c) = 0;
  I_00470670(0x1a8) = 0;
  if (F_00470670(0x9e4) != 0.0f) {
    d = F_00470670(0x9dc) * F_00470670(0x9b8) + (F_00470670(0x9d8) * F_00470670(0x9b4) + F_00470670(0x9d4) * F_00470670(0x9b0));
    if (d < 0.0f)
      d = -d;
    F_00470670(0xb0c) = (1.0f - d / F_00470670(0x9e4)) * F_00470670(0x9e4);
  } else {
    I_00470670(0xb0c) = 0;
  }
  if (pad[0])
    I_00470670(0xb24) = (int)(I_00470670(0xb24) + dt);
  else
    I_00470670(0xb24) = 0;
  if (pad[1])
    I_00470670(0xb28) = (int)(I_00470670(0xb28) + dt);
  else
    I_00470670(0xb28) = 0;
  k = DAT_007f101c & 0xf;
  f = 0.0f;
  if (pad[0]) {
    if (F_00470670(0x9e4) > 64.0f)
      I_00470670(0xb20) = 1;
    f = (float)pad[0] * F_00470670(0x190) * 0.00390625f;
    if (mode == 7)
      f = 0.0f;
    f *= 0.5f;
    if (I_00470670(0xbf0))
      f *= 0.75f;
    if (pad[5] > 128.0f) {
      f *= 1.5f;
    } else {
      g = (float)I_00470670(0xb24);
      if (g > 6000.0f)
        g = 6000.0f;
      f = (g + 6000.0f) * f * 0.00016666667f;
    }
    F_00470670(0x1a8) = f;
    F_00470670(0x26c) = f;
  }
  F_00470670(0x1ac + k * 4) = f;
  F_00470670(0x270 + k * 4) = f;
  if (pad[1]) {
    if (F_00470670(0x9e4) > 64.0f)
      I_00470670(0xb20) = 1;
    f = (float)pad[1] * F_00470670(0x190) * 0.00390625f;
    if (mode == 7)
      f = 0.0f;
    f *= 0.5f;
    if (I_00470670(0xbf0))
      f *= 0.75f;
    if (pad[5] > 128.0f) {
      f *= 1.5f;
    } else {
      g = (float)I_00470670(0xb28);
      if (g > 6000.0f)
        g = 6000.0f;
      f = (g + 6000.0f) * f * 0.00016666667f;
    }
    f = -f;
    F_00470670(0x1a8) = f;
    F_00470670(0x26c) = f;
    F_00470670(0x1ac + k * 4) = f;
    F_00470670(0x270 + k * 4) = f;
  }
  FUN_0046ddb0(dt, m, ctx, car);
  FUN_00467650(i, dt, m, pad, car);
  FUN_00468980((int)car, m, dt, (int)pad);
  if (I_00470670(0x9f0) == 2) {
    F_00470670(0x9b0) *= 0.9f;
    F_00470670(0x9b4) *= 0.9f;
    F_00470670(0x9b8) *= 0.9f;
  }
}

#undef F_00470670
#undef I_00470670

#ifndef MATCH_PORT
/* stand-in caller (not the real TU): the real caller 0x00470c70 passes car in EAX (mov eax, ebp at 0x00471073) */
void StandIn_00470670(char *cars, int n, float dt, unsigned char *pad, int ctx)
{
  int i;
  for (i = 0; i < n; i++)
    FUN_00470670(i, dt, pad, ctx, cars + i * 0xd04);
}
#endif
