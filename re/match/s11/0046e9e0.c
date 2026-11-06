// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046e9e0 (h5): best 81.7% (regs 81.9%), 393/385 ins. Raw: did not compile.
   Static callee with car in ESI, current matrix in EDI, previous matrix in EBX (none of them written, so not
   saved) and (dt, pad) on the stack; the order the compiler gives those three follows their use weight in the
   body (pilot finding 1). The real caller 0x004709a0 (call at 0x00470ad3) is matched separately in 004709a0.c;
   a stand-in caller sits at the end (#ifndef MATCH_PORT, no MATCH line).
   .rdata float constants as literals (aliasing, see 00470670.c); 0x00613108 (100.0) is .data, kept as MGLOBAL.
   Moved the score: field-wise vector copies instead of `acc = v` (int moves), -f into a temp for the second pad
   axis, zero-order and `s = f * 0.5f` before the three prev[12..14] stores (permuter, 76.4 -> 81.7).
   Residual (b): (1) the original keeps acc in memory where ours holds the three zeros on the x87 stack across the
   mode test; (2) the thrust products t*dt*G*g*0.0001*(1/3000): ours reassociates (loads G or folds the two
   literals) where the original multiplies left to right in place; (3) x87 stack scheduling in the cross-product
   rows and the `== 7` block (same reassociation). Splitting the product into statements did not stop it. The
   permuter's best (82.3%) moved statements past their uses (rejected). */
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
typedef struct V3_0046e9e0 { float x, y, z; } V3_0046e9e0;
MGLOBAL(float, DAT_00613108, 0x00613108);
int FUN_0040e350(void);
void FUN_0046d700(V3_0046e9e0 *out, int id);
void FUN_004c4670(float *out, float *in);

#define F_0046e9e0(o) (*(float *)(car + (o)))
#define I_0046e9e0(o) (*(int *)(car + (o)))

// MATCH: 0x0046e9e0 FUN_0046e9e0
static void FUN_0046e9e0(float dt, unsigned char *pad, char *car, float *cur, float *prev)
{
  float f;
  float s;
  float g;
  float t;
  float d;
  int n;
  V3_0046e9e0 acc;
  V3_0046e9e0 dir;
  V3_0046e9e0 v;

  f = dt * 0.00033333333f;
  s = f * 0.0027778f;
  acc.x = s * F_0046e9e0(0x9b0);
  acc.y = s * F_0046e9e0(0x9b4);
  acc.z = s * F_0046e9e0(0x9b8);
  if (FUN_0040e350() != 6 && FUN_0040e350() != 11 && FUN_0040e350() != 10) {
    acc.x = 0.0f;
    acc.z = 0.0f;
    acc.y = 0.0f;
  }
  s = f * 0.5f;
  prev[12] = acc.x + cur[12];
  prev[13] = cur[13] + acc.y;
  prev[14] = cur[14] + acc.z;
  acc.x = s * F_0046e9e0(0x9bc);
  acc.y = F_0046e9e0(0x9c0) * s;
  acc.z = s * F_0046e9e0(0x9c4);
  FUN_0046d700(&dir, I_0046e9e0(0));
  if (I_0046e9e0(0x10) == 0) {
    acc.z = 0.0f;
    acc.y = 0.0f;
    acc.x = 0.0f;
    g = F_0046e9e0(0x9e4) * 0.00066666666f;
    if (g > 1.0f)
      g = 1.0f;
    I_0046e9e0(0xcfc) = 0;
    if (pad[4] && pad[5]) {
      I_0046e9e0(0xcfc) = 1;
      g = 1.0f;
    }
    if (F_0046e9e0(0x9e0) != 0.0f) {
      t = pad[0];
      if (I_0046e9e0(0x20) != 0) {
        t += I_0046e9e0(0x20);
        if (t > 255.0f)
          t = 255.0f;
      }
      n = I_0046e9e0(0x30);
      if (n != 0) {
        f = 1.0f - n * 0.01f;
        t = f * t;
      }
      if (t != 0.0f) {
        f = t * dt * DAT_00613108 * g * 0.0001f * 0.00033333333f;
        v.x = dir.x * f;
        v.y = dir.y * f;
        v.z = dir.z * f;
        acc.x = v.x;
        acc.y = v.y;
        acc.z = v.z;
      }
      t = pad[1];
      if (I_0046e9e0(0x24) != 0) {
        t += I_0046e9e0(0x24);
        if (t > 255.0f)
          t = 255.0f;
      }
      if (n != 0) {
        f = 1.0f - n * 0.01f;
        t = f * t;
      }
      if (t != 0.0f) {
        f = t * dt * DAT_00613108 * g * 0.0001f * 0.00033333333f;
        s = -f;
        v.x = dir.x * s;
        v.y = s * dir.y;
        v.z = s * dir.z;
        acc.x = v.x + acc.x;
        acc.y = v.y + acc.y;
        acc.z = v.z + acc.z;
      }
    }
    if (pad[4] && pad[5]) {
      acc.x *= 1.5f;
      acc.y *= 1.5f;
      acc.z *= 1.5f;
    }
    if (I_0046e9e0(0x9e0) != 0x40800000) {
      acc.x += f * F_0046e9e0(0x9bc);
      acc.z += f * F_0046e9e0(0x9c4);
    }
    if (F_0046e9e0(0x9dc) * F_0046e9e0(0x9b8) + (F_0046e9e0(0x9d8) * F_0046e9e0(0x9b4) + F_0046e9e0(0x9d4) * F_0046e9e0(0x9b0)) < -0.1f
        && (!pad[4] || !pad[5])) {
      acc.x = -acc.x;
      acc.y = -acc.y;
      acc.z = -acc.z;
    }
  }
  if (F_0046e9e0(0x9e0) == 0.0f) {
    F_0046e9e0(0x144) = acc.x + F_0046e9e0(0x144);
    F_0046e9e0(0x148) = acc.y + F_0046e9e0(0x148);
    F_0046e9e0(0x14c) = acc.z + F_0046e9e0(0x14c);
  }
  if (FUN_0040e350() == 7 && I_0046e9e0(4) == 1) {
    f = (dt + dt) * 0.00033333333f * F_0046e9e0(0x9e4) * 0.0005f;
    acc.x = dir.x * f;
    acc.y = dir.y * f;
    acc.z = dir.z * f;
  }
  d = (3000.0f - dt * 4.0f) * 0.00033333333f;
  F_0046e9e0(0x144) = (acc.x + F_0046e9e0(0x144)) * d;
  F_0046e9e0(0x148) = (acc.y + F_0046e9e0(0x148)) * d;
  F_0046e9e0(0x14c) = (acc.z + F_0046e9e0(0x14c)) * d;
  s = dt * 0.002f;
  dir.x = s * F_0046e9e0(0x144);
  dir.y = s * F_0046e9e0(0x148);
  dir.z = s * F_0046e9e0(0x14c);
  if (pad[1] == 0 && pad[0] == 0) {
    acc.x += dir.x;
    acc.y = dir.y + acc.y;
    acc.z += dir.z;
  }
  dir.x = acc.y * cur[2] - acc.z * cur[1];
  dir.y = acc.z * cur[0] - acc.x * cur[2];
  dir.z = acc.x * cur[1] - acc.y * cur[0];
  prev[0] = dir.x + cur[0];
  prev[1] = dir.y + cur[1];
  prev[2] = dir.z + cur[2];
  dir.x = acc.y * cur[6] - acc.z * cur[5];
  dir.y = acc.z * cur[4] - acc.x * cur[6];
  dir.z = acc.x * cur[5] - acc.y * cur[4];
  prev[4] = dir.x + cur[4];
  prev[5] = dir.y + cur[5];
  prev[6] = dir.z + cur[6];
  dir.x = acc.y * cur[10] - acc.z * cur[9];
  dir.y = acc.z * cur[8] - acc.x * cur[10];
  dir.z = acc.x * cur[9] - acc.y * cur[8];
  prev[8] = dir.x + cur[8];
  prev[9] = dir.y + cur[9];
  prev[10] = dir.z + cur[10];
  FUN_004c4670(prev, prev);
}

#undef F_0046e9e0
#undef I_0046e9e0

#ifndef MATCH_PORT
/* stand-in caller (not the real TU): the real caller 0x004709a0 passes car in ESI, current matrix in EDI,
   previous matrix in EBX (call at 0x00470ad3) */
void StandIn_0046e9e0(float dt, unsigned char *pad, char *car)
{
  int k;
  for (k = 0; k < 2; k++)
    FUN_0046e9e0(dt, pad, car, (float *)(car + *(int *)(car + 0x9a8) * 0x40 + 0x928),
                 (float *)(car + *(int *)(car + 0x9ac) * 0x40 + 0x928));
}
#endif
