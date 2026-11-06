// FLAGS: /O2 /GS /Ob1
/* 0x00475f90: 96.4% (regs 96.4%), hand pass h9 (MATCH-S12); raw converter 31.2%. The file holds the two
   register-arg callees as file-static functions defined before the caller (pilot finding 1): 0x00475f30 (out
   pointer in EAX) and 0x00475f50 (out pointer in ESI), both 100% here, bodies as in their own files. Ghidra's size
   stops 7 instructions before the real end (dec edi / jne / pops / ret), and match.py counts those 7 as misses:
   inside Ghidra's range only one instruction differs (in-place 99.1%; --bytes: 6 differing bytes, first at +0x2e2).
   What moved the score: emitter struct (RwV3d pos/vel/pos2/vel2[4], t at +0xc4), `if (!(t < 0.0f))` (test ah,5
   / jnp), `pos += vel * (k * dt)` grouping (source order k * dt loads dt first), an explicit odd/tmp if-else for the alternating size and buffer,
   1.0/255/200 as literals; in the third loop `double s` with `(float)s` on y gives the original's x87 shape
   (fld mem / fmul st(1) for x and z, fld st(0) for y, s popped by fstp st(0) after z). Residual (b): the first
   fadd [v.x] in the third loop is scheduled two slots late (after the v.z copy store). Tried: scalar or block-
   scoped v, copy/s order, +=, casts on x/z, __inline IncScaled/AddScaled helpers (92.4/89.9%), permuter twice
   (no gain). */
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
#ifdef __cplusplus
#define MATCH_STATIC
#else
#define MATCH_STATIC static
#endif
typedef struct { float x, y, z; } RwV3d;
typedef struct { int d[16]; } PrimBuf;
typedef struct {
  RwV3d pos[4];
  RwV3d vel[4];
  RwV3d pos2[4];
  RwV3d vel2[4];
  float f_c0;
  float t;
  float a;
  float f_cc;
  float scale;
  int extra;
} Emitter;
MGLOBAL(float, DAT_00613250, 0x00613250);
MGLOBAL(float, DAT_00613258, 0x00613258);
MGLOBAL(float, DAT_0061325c, 0x0061325c);
MGLOBAL(float, DAT_00613268, 0x00613268);
MGLOBAL(float, DAT_0061326c, 0x0061326c);
MGLOBAL(float, DAT_00613270, 0x00613270);
MGLOBAL(float, DAT_00613274, 0x00613274);
MGLOBAL(float, DAT_00613278, 0x00613278);
MGLOBAL(float, DAT_0061327c, 0x0061327c);
MGLOBAL(float, DAT_00613280, 0x00613280);
MGLOBAL(float, DAT_00613284, 0x00613284);
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBALA(PrimBuf, DAT_00691618, 2, 0x00691618);
MGLOBALA(PrimBuf, DAT_00692418, 2, 0x00692418);
MGLOBAL(PrimBuf, DAT_00692498, 0x00692498);
void FUN_004769a0(RwV3d *p);
void FUN_004769d0(float *p);
void FUN_004769f0(unsigned char *p);
void FUN_00476a30(float f);
void FUN_00476d00(PrimBuf *b);
float FUN_004c39b0(RwV3d *out, RwV3d *in);
// MATCH: 0x00475f30 FUN_00475f30
MATCH_STATIC void FUN_00475f30(float *out, float param_1, float param_2)
{
  float f;
  f = DAT_00613258 * DAT_00613278 * param_1 * param_2 + DAT_00613274;
  out[1] = f;
  out[0] = f;
}
// MATCH: 0x00475f50 FUN_00475f50
MATCH_STATIC void FUN_00475f50(unsigned char *out, float param_1)
{
  out[3] = (unsigned char)((param_1 < 0.5f ? param_1 : 1.0f - param_1) * 255.0f);
  out[0] = 0;
  out[1] = 0;
  out[2] = 0;
}
// MATCH: 0x00475f90 FUN_00475f90
void FUN_00475f90(Emitter *e)
{
  unsigned char c[4];
  float tmp;
  float size[2];
  RwV3d v;
  float t;
  double s;
  int i;
  int odd;

  t = e->t;
  if (!(t < 0.0f)) {
    if (t < DAT_0061325c) {
      size[0] = size[1] = DAT_00613270 * e->scale * t + DAT_0061326c;
      c[0] = c[1] = c[2] = c[3] = (unsigned char)((1.0f - t / DAT_0061325c) * 255.0f);
      FUN_004769d0(size);
      FUN_004769f0(c);
      for (i = 0; i < 4; i++) {
        e->pos[i].x += e->vel[i].x * (DAT_00613250 * DAT_007f100c);
        e->pos[i].y += e->vel[i].y * (DAT_00613250 * DAT_007f100c);
        e->pos[i].z += e->vel[i].z * (DAT_00613250 * DAT_007f100c);
        FUN_004769a0(&e->pos[i]);
        if (i & 1) {
          tmp = t;
          odd = 1;
        } else {
          tmp = -t;
          odd = 0;
        }
        FUN_00476a30(tmp);
        FUN_00476d00(&DAT_00691618[odd]);
      }
    }
    if (e->extra != 0) {
      FUN_00475f30(size, e->a, e->scale);
      FUN_00475f50(c, e->a);
      FUN_004769d0(size);
      FUN_004769f0(c);
      for (i = 0; i < 4; i++) {
        if (t > DAT_00613268) {
          e->vel2[i].y = 1.0f;
          FUN_004c39b0(&e->vel2[i], &e->vel2[i]);
        }
        e->pos2[i].x += e->vel2[i].x * (DAT_00613250 * DAT_007f100c);
        e->pos2[i].y += e->vel2[i].y * (DAT_00613250 * DAT_007f100c);
        e->pos2[i].z += e->vel2[i].z * (DAT_00613250 * DAT_007f100c);
        FUN_004769a0(&e->pos2[i]);
        if (i & 1) {
          tmp = t;
          odd = 1;
        } else {
          tmp = -t;
          odd = 0;
        }
        FUN_00476a30(tmp);
        FUN_00476d00(&DAT_00692418[odd]);
      }
    }
    if (t < DAT_00613284) {
      c[2] = 0;
      size[0] = size[1] = DAT_0061327c * e->scale;
      c[1] = 0;
      c[0] = 0;
      c[3] = (unsigned char)((1.0f - t / DAT_00613284) * 200.0f);
      FUN_004769d0(size);
      FUN_004769f0(c);
      for (i = 0; i < 4; i++) {
        s = DAT_00613280 * t;
        v = e->pos[i];
        v.x = (float)(e->vel[i].x * s + v.x);
        v.y = (float)s * e->vel[i].y + v.y;
        v.z = e->vel[i].z * s + v.z;
        FUN_004769a0(&v);
        FUN_00476d00(&DAT_00692498);
      }
    }
  }
}
