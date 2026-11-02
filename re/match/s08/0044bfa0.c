// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044bfa0 (934 B): 74.3% (regs 74.3%, ops 96.6%), by hand. Raw converter 24.5%.
   Shape: uint frame time -> dt = t * 0.02f; sin/cos intrinsics with double s (the 0.35/0.15 constants are
   doubles holding float values: float literals promoted against the double sin/cos result); frame translate
   by -pos, rotate about (0,1,0), translate back; then two identical basis rebuilds (clump at +0x08 into the
   matrix at +0x30 with pos.y 3.52f, clump at +0x00 into +0xa4 with pos.y 3.51f) through FUN_004c0ed0 (LTM)
   and FUN_004c39b0 (normalize, float result discarded); last a dot product with *(FUN_004260b0()+4)+0x30.
   0x005cc348 (1.5f) is a global: as a literal the compiler folds x*1.5f*1.5f into one multiply, the
   original multiplies twice. 0x005cc33c = -1.0f, 0x005cc320 = 1.0f, 0x005ce26c = 3.5f are literals.
   Residual (b): frame layout. The original orders the six 12-byte locals v, at, right, up, pos, axis
   (v lowest); this source gets at, right, v, up, pos, axis. Declaration order (forward, reverse), names,
   nested scopes, float[3] for v and an inline scale helper do not move it. Every esp offset that touches
   v/at/right differs, and the dot-product operand order follows the layout. relocheck not applicable below 100% (offsets shift), cxx ok. Permuter 300 s: 75.9% only through statement moves that change the call and negation order the disassembly shows, not kept. */
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
MGLOBAL(float, DAT_005cc348, 0x005cc348);
typedef struct V3_bfa0 {
  float x;
  float y;
  float z;
} V3_bfa0;
double sin(double x);
double cos(double x);
#pragma intrinsic(sin, cos)
int FUN_004260b0(void);
void FUN_0044bbc0(int param_1);
void FUN_0044bc30(int param_1, float param_2);
int FUN_004c0ed0(int p0);
int FUN_004c1340(int p0, V3_bfa0 *p1, int p2);
int FUN_004c1520(int p0, V3_bfa0 *p1, float p2, int p3);
float FUN_004c39b0(V3_bfa0 *p0, V3_bfa0 *p1);
void FUN_004c51a0(float *p0, V3_bfa0 *p1, int p2);
// MATCH: 0x0044bfa0 FUN_0044bfa0
void FUN_0044bfa0(int param_1, uint param_2)
{
  float dt;
  double s;
  float r;
  V3_bfa0 *p;
  V3_bfa0 v;
  V3_bfa0 at;
  V3_bfa0 right;
  V3_bfa0 up;
  V3_bfa0 pos;
  V3_bfa0 axis;

  dt = (float)param_2 * 0.02f;
  axis.x = 0.0f;
  axis.y = 1.0f;
  axis.z = 0.0f;
  s = sin(*(float *)(param_1 + 0x28));
  *(float *)(param_1 + 0x20) += s * 0.35f;
  *(float *)(param_1 + 0x24) += cos(*(float *)(param_1 + 0x2c)) * 0.35f;
  *(float *)(param_1 + 0x1c) = s * 0.15f;
  *(float *)(param_1 + 0x28) += dt * 0.0185f;
  v = *(V3_bfa0 *)(*(int *)(*(int *)param_1 + 4) + 0x40);
  v.x = v.x * -1.0f;
  v.y = v.y * -1.0f;
  v.z = v.z * -1.0f;
  FUN_004c1340(*(int *)(*(int *)param_1 + 4), &v, 2);
  FUN_004c1520(*(int *)(*(int *)param_1 + 4), &axis, *(float *)(param_1 + 0x1c), 2);
  v.x = v.x * -1.0f;
  v.y = v.y * -1.0f;
  v.z = v.z * -1.0f;
  FUN_004c1340(*(int *)(*(int *)param_1 + 4), &v, 2);
  FUN_0044bbc0(param_1);
  FUN_0044bc30(param_1, dt);

  up.x = 0.0f;
  up.y = 1.0f;
  up.z = 0.0f;
  pos = *(V3_bfa0 *)(FUN_004c0ed0(*(int *)(*(int *)(param_1 + 8) + 4)) + 0x30);
  at = *(V3_bfa0 *)(FUN_004c0ed0(*(int *)(*(int *)(param_1 + 8) + 4)) + 0x20);
  right = *(V3_bfa0 *)FUN_004c0ed0(*(int *)(*(int *)(param_1 + 8) + 4));
  at.y = 0.0f;
  right.y = 0.0f;
  FUN_004c39b0(&at, &at);
  FUN_004c39b0(&right, &right);
  pos.y = 3.52f;
  FUN_004c51a0((float *)(param_1 + 0x30), &pos, 0);
  *(V3_bfa0 *)(param_1 + 0x50) = at;
  *(V3_bfa0 *)(param_1 + 0x40) = up;
  *(V3_bfa0 *)(param_1 + 0x30) = right;

  up.x = 0.0f;
  up.y = 1.0f;
  up.z = 0.0f;
  pos = *(V3_bfa0 *)(FUN_004c0ed0(*(int *)(*(int *)param_1 + 4)) + 0x30);
  at = *(V3_bfa0 *)(FUN_004c0ed0(*(int *)(*(int *)param_1 + 4)) + 0x20);
  right = *(V3_bfa0 *)FUN_004c0ed0(*(int *)(*(int *)param_1 + 4));
  at.y = 0.0f;
  right.y = 0.0f;
  FUN_004c39b0(&at, &at);
  FUN_004c39b0(&right, &right);
  pos.y = 3.51f;
  FUN_004c51a0((float *)(param_1 + 0xa4), &pos, 0);
  *(V3_bfa0 *)(param_1 + 0xc4) = at;
  *(V3_bfa0 *)(param_1 + 0xb4) = up;
  *(V3_bfa0 *)(param_1 + 0xa4) = right;

  p = (V3_bfa0 *)(*(int *)(FUN_004260b0() + 4) + 0x30);
  r = (p->y * at.y + at.x * p->x + p->z * at.z + 1.0f) * DAT_005cc348 * DAT_005cc348 + 3.5f;
  *(float *)(param_1 + 0x104) = r;
  *(float *)(param_1 + 0x110) = r;
}
