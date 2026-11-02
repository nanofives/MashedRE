// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x004764f0 (h2): 100% (BYTE-IDENTICAL whole symbol, 347 B: Ghidra's size 341 stops before the loop's
   jne/pops/ret), raw 26.9%. Register-arg function (ECX record, EDX int, 5 stack args, plain ret): static with a
   stand-in caller built only outside MATCH_PORT; this file also holds its register-arg callee 0x004764e0 (static,
   defined first). What moved it: 0.0f / 90.0f as literals (the compares at 0x005d757c / 0x005ccad0 hoist above the
   record stores only when the operand is a known constant), and r->da[i] / r->db[i] indexed directly (a pointer
   local moved the strength-reduced base from +0x34 to +0x38). Real callers 0x00476650/0x00476710/0x004767c0 hold
   copies of both statics. Residual: none. */
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
typedef struct { float x, y, z; } Vec3;
typedef struct {
  Vec3 a[4];     /* +0x00 */
  Vec3 da[4];    /* +0x30 */
  Vec3 b[4];     /* +0x60 */
  Vec3 db[4];    /* +0x90 */
  float dur;     /* +0xc0 */
  float t;       /* +0xc4 */
  float ratio;   /* +0xc8 */
  int active;    /* +0xcc */
  float v_d0;    /* +0xd0 */
  int v_d4;      /* +0xd4 */
} RecD8;
float FUN_004b4510(float lo, float hi);
float FUN_004c39b0(Vec3 *out, Vec3 *in);
void FUN_00476320(float *p0, float *p1, float p2);

// MATCH: 0x004764e0 FUN_004764e0
static void FUN_004764e0(float *out, float *org, float ang)
{
  FUN_00476320(out, org, ang);
}

// MATCH: 0x004764f0 FUN_004764f0
static void FUN_004764f0(RecD8 *r, int p2, Vec3 *src, float *p4, float spread, float dur, float p7)
{
  int i;
  Vec3 *a;
  Vec3 *b;

  r->t = 0;
  r->ratio = 0;
  r->v_d0 = p7;
  r->active = 1;
  r->v_d4 = p2;
  if (dur > 0.0f)
    r->t = -dur;
  for (i = 0; i < 4; i++) {
    r->a[i] = *src;
    r->b[i] = *src;
    if (spread > 90.0f) {
      r->da[i].x = FUN_004b4510(-1.0f, 1.0f);
      r->da[i].y = FUN_004b4510(-1.0f, 1.0f);
      r->da[i].z = FUN_004b4510(-1.0f, 1.0f);
      r->db[i].x = FUN_004b4510(-1.0f, 1.0f);
      r->db[i].y = FUN_004b4510(-1.0f, 1.0f);
      r->db[i].z = FUN_004b4510(-1.0f, 1.0f);
    } else {
      FUN_004764e0(&r->da[i].x, p4, spread);
      FUN_004764e0(&r->db[i].x, p4, spread);
    }
    FUN_004c39b0(&r->da[i], &r->da[i]);
    FUN_004c39b0(&r->db[i], &r->db[i]);
  }
}

#ifndef MATCH_PORT
/* stand-in caller (not a real function): gives FUN_004764f0 its ECX/EDX register convention */
void h2_standin_004764f0(RecD8 *r, Vec3 *s, float *p, float a, float b, float c)
{
  FUN_004764f0(r, 1, s, p, a, b, c);
  FUN_004764f0(r + 1, 2, s, p, b, a, c);
}
#endif
