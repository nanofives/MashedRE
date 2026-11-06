// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046ef70 (h5): best 43.9% (regs 45.2%, ops 97.3%), 478/485 ins. Raw 26.9% (unaff_EDI).
   Static callee with car in EDI and one unused stack argument (the real caller 0x004709a0 pushes the previous
   matrix at 0x00470afd); a stand-in caller sits at the end (#ifndef MATCH_PORT). The 18 contact records at
   car + 0x4a8 (stride 0x40): +4 id (-1 empty), +8 normal, +0x14 depth, +0x2c a second vector, +0x38 load.
   Ghidra's `pfVar7[-2] != -NAN` after the loop is `contacts[0].id != -2` (cmp ecx, -2 at 0x0046f5b5).
   .rdata float constants as literals (0x005cc33c -1, 0x005cd50c -0.5, 0x005ccd04 500, 0x005ccad0 90,
   0x005cea04 50.24, 0x005cc9f4 8, 0x005cc564 0.25, 0x005cd120 50, 0x005cc31c 3, 0x005cc56c 0.1, ...).
   Ops 97.3% with the same instruction count: the structure is right. Residual (b): the original frame is 0x80,
   ours 0x7c, because it keeps &contacts[0] in its own stack slot (lea eax, [esi - 0xc] at 0x0046f00d) while ours
   rematerializes it from EDI; every stack offset after that shifts, which is most of the exact-% loss. Tried
   c = &c0[k] indexing (44.1%). Also x87 operand order in a few products. The permuter's best (78.6%,
   regs-renamed) moved statements before their inputs (rejected). */
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
typedef struct V3_0046ef70 { float x, y, z; } V3_0046ef70;
/* one 0x40-byte wheel/contact record at car + 0x4a8 + k * 0x40 */
typedef struct Contact_0046ef70 {
  int a;
  int id;
  V3_0046ef70 n;
  float d;
  float pad1[5];
  V3_0046ef70 m;
  float s;
  float pad2;
} Contact_0046ef70;
float FUN_004c39b0(V3_0046ef70 *out, V3_0046ef70 *in);
float FUN_004c3ac0(V3_0046ef70 *v);

#define F_0046ef70(o) (*(float *)(car + (o)))
#define I_0046ef70(o) (*(int *)(car + (o)))

// MATCH: 0x0046ef70 FUN_0046ef70
static void FUN_0046ef70(float *unused, char *car)
{
  int k;
  int cnt;
  float lensum;
  float grip;
  float s;
  float r;
  float d;
  float L;
  float w;
  float h;
  float v0, v1, v2;
  Contact_0046ef70 *c;
  Contact_0046ef70 *c0;
  V3_0046ef70 T;
  V3_0046ef70 A;
  V3_0046ef70 B;
  V3_0046ef70 F;
  V3_0046ef70 G;
  V3_0046ef70 U;
  V3_0046ef70 P;
  V3_0046ef70 Q;
  V3_0046ef70 R;

  lensum = 0.0f;
  T.z = 0.0f;
  T.y = 0.0f;
  T.x = 0.0f;
  A.z = 0.0f;
  A.y = 0.0f;
  A.x = 0.0f;
  B.z = 0.0f;
  B.y = 0.0f;
  B.x = 0.0f;
  grip = 1.0f;
  cnt = 0;
  if (I_0046ef70(0x198) != 0)
    cnt = 1;
  if (I_0046ef70(0x25c) != 0)
    cnt++;
  if (I_0046ef70(0x320) != 0)
    cnt++;
  if (I_0046ef70(0x3e4) != 0)
    cnt++;
  c0 = (Contact_0046ef70 *)(car + 0x4a8);
  c = c0;
  for (k = 0; k < 18; k++, c++) {
    if (c->id == -1)
      continue;
    s = c->s;
    if (k < 4) {
      s *= -1.0f;
      if (c->n.y < 0.9f)
        s *= 4.0f;
    } else if (cnt == 4) {
      s *= -1.0f;
    } else {
      s *= -0.5f;
    }
    r = c->s / F_0046ef70(0x9e4);
    if (r < 0.0f) {
      r = -r;
      if (r > 1.0f)
        r = 1.0f;
      grip = 1.0f - r;
    }
    if (I_0046ef70(0xcfc) != 0 && s < 500.0f)
      s = 500.0f;
    s *= 0.75f;
    F.x = s * c->n.x;
    F.y = s * c->n.y;
    F.z = s * c->n.z;
    B.x = F.x + B.x;
    B.y = F.y + B.y;
    B.z = F.z + B.z;
    A.x = F.x + A.x;
    A.y = F.y + A.y;
    A.z = F.z + A.z;
    if (I_0046ef70(0xcfc) != 0) {
      G.x = c->n.x * 500.0f;
      G.y = c->n.y * 500.0f;
      G.z = c->n.z * 500.0f;
    } else {
      G.x = c->n.x * 90.0f;
      G.y = c->n.y * 90.0f;
      G.z = c->n.z * 90.0f;
    }
    B.x = G.x + B.x;
    B.y = G.y + B.y;
    B.z = G.z + B.z;
    A.x = G.x + A.x;
    A.y = G.y + A.y;
    A.z = G.z + A.z;
    if (k < 4)
      continue;
    FUN_004c39b0(&U, &c->m);
    d = U.x * c->n.x + U.y * c->n.y + U.z * c->n.z;
    L = FUN_004c3ac0(&F) * d;
    P.x = U.x * L;
    P.y = U.y * L;
    P.z = U.z * L;
    Q.x = F.x - P.x;
    Q.y = F.y - P.y;
    Q.z = F.z - P.z;
    R.x = Q.z * P.y - Q.y * P.z;
    R.y = Q.x * P.z - Q.z * P.x;
    R.z = Q.y * P.x - Q.x * P.y;
    if (c->m.y * F.y + F.x * c->m.x + c->m.z * F.z > 0.0f) {
      R.x = -R.x;
      R.y = -R.y;
      R.z = -R.z;
    }
    B.x = P.x + B.x;
    B.y = P.y + B.y;
    B.z = P.z + B.z;
    A.x = F.x + A.x;
    A.y = F.y + A.y;
    A.z = F.z + A.z;
    G.x = Q.x * c->d;
    G.y = Q.y * c->d;
    G.z = Q.z * c->d;
    w = FUN_004c3ac0(&G) * 50.24f;
    lensum = w + lensum;
    if (FUN_004c3ac0(&R) > 0.0001f) {
      FUN_004c39b0(&R, &R);
      U.x = R.x * w;
      U.y = R.y * w;
      U.z = R.z * w;
      T.x = T.x - U.x;
      T.y = T.y - U.y;
      T.z = T.z - U.z;
    }
  }
  r = 1.0f / (float)I_0046ef70(0x9ec);
  A.x *= r;
  A.y *= r;
  A.z *= r;
  B.x *= r;
  B.y *= r;
  B.z *= r;
  T.x *= r;
  T.y *= r;
  T.z *= r;
  lensum *= r;
  if (lensum - FUN_004c3ac0(&T) != 0.0f) {
    G.x = A.x - B.x;
    G.y = A.y - B.y;
    G.z = A.z - B.z;
  }
  h = F_0046ef70(0x5c) * 0.0027778f * 8.0f;
  I_0046ef70(0xb20) = 1;
  T.x *= h;
  T.y *= h;
  T.z *= h;
  L = FUN_004c3ac0(&T);
  if (L != 0.0f && L < 0.25f) {
    h = 0.25f / L;
    if (h > 50.0f)
      h = 50.0f;
    T.x *= h;
    T.y *= h;
    T.z *= h;
  }
  T.x *= 0.5f;
  T.y *= 0.5f;
  T.z *= 0.5f;
  F_0046ef70(0x144) = T.x + F_0046ef70(0x144);
  F_0046ef70(0x148) = T.y + F_0046ef70(0x148);
  F_0046ef70(0x14c) = T.z + F_0046ef70(0x14c);
  v0 = B.x + F_0046ef70(0x9b0);
  F_0046ef70(0x9b0) = v0;
  v1 = B.y + F_0046ef70(0x9b4);
  F_0046ef70(0x9b4) = v1;
  v2 = B.z + F_0046ef70(0x9b8);
  F_0046ef70(0x9b8) = v2;
  if (c0->id != -2) {
    h = grip * 3.0f;
    if (h > 0.9f)
      h = 0.9f;
    F_0046ef70(0x9b0) = v0 * h;
    F_0046ef70(0x9b4) = v1 * h;
    F_0046ef70(0x9b8) = v2 * h;
  }
  if (I_0046ef70(0x9e0) == 0x40800000) {
    L = FUN_004c3ac0((V3_0046ef70 *)(car + 0x9b0));
    if (L > 0.0001f) {
      FUN_004c39b0(&G, (V3_0046ef70 *)(car + 0x9b0));
      h = (G.y * F_0046ef70(0x9cc) + G.x * F_0046ef70(0x9c8) + G.z * F_0046ef70(0x9d0)) * 0.1f * L;
      G.x = h * F_0046ef70(0x9c8);
      G.y = h * F_0046ef70(0x9cc);
      G.z = h * F_0046ef70(0x9d0);
      F_0046ef70(0x9b0) = F_0046ef70(0x9b0) - G.x;
      F_0046ef70(0x9b4) = F_0046ef70(0x9b4) - G.y;
      F_0046ef70(0x9b8) = F_0046ef70(0x9b8) - G.z;
      FUN_004c3ac0(&B);
    }
  }
}

#undef F_0046ef70
#undef I_0046ef70

#ifndef MATCH_PORT
/* stand-in caller (not the real TU): the real caller 0x004709a0 passes car in EDI (call at 0x00470afe) */
void StandIn_0046ef70(char *car, float *m)
{
  if (*(int *)(car + 0x9ec) != 0)
    FUN_0046ef70(m, car);
}
#endif
