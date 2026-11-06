// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h4 0x00490500: 56.0% (regs 61.0%, ops 95.9%), 661/661 instructions, by hand from the decomp and the
   disassembly (raw converter 28.3%). Structure is complete: 10 x 0x488 records at 0x0076d980 (three 8-entry
   sub-arrays a/b/c), 100 x 0x24 records at 0x00770718, 40 x 0x20 records at 0x00766a00, the six batch flushes;
   FUN_004a2c48 is the CRT _ftol (written as (unsigned char) casts), the .rdata floats are literals (see
   0x00490380), the c-loop state machine has the 0x00490380 shape. The colour locals are ordered mid, grey, white,
   black (best of all 24 orders). Permuter 300 s: +1.5 via if/else swap in the ccount test (kept); a call-order
   move it also proposed changes behaviour and was not kept.
   Residual (b): the original keeps three outer-loop induction pointers (record+0x1c in ebx, &scale in edi,
   &stage in esi, spilled at [esp+0x28..0x30]); this source gets two (the compiler merges the scale pointer into
   the record pointer) or one (index form, 46.7%). That shifts every record offset and register in the 10-record
   loop. The original frame is also 8-aligned (and esp,-8; sub esp,0x40): a double local that is never spilled;
   double for dy / lim / the uv struct did not produce it with 0x40 bytes of locals (double f does, at 0x48). */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef struct { unsigned char red, green, blue, alpha; } RwRGBA;
typedef struct { float x, y, z; } RwV3d;
typedef struct { float u0, v0, u1, v1; } UvRect;
typedef struct {
  int state;            /* +0x000 */
  RwV3d pos;            /* +0x004 */
  float size;           /* +0x010 */
  int n;                /* +0x014 */
  int m;                /* +0x018 */
  float timer;          /* +0x01c */
  float f20;            /* +0x020 */
  float f24;            /* +0x024 */
  int count2;           /* +0x028 */
  int count3;           /* +0x02c */
  RwV3d apos[8];        /* +0x030 */
  RwV3d avel[8];        /* +0x090 */
  float asize[8][2];    /* +0x0f0 */
  float aalpha[8];      /* +0x130 */
  float afade[8];       /* +0x150 */
  RwV3d centre;         /* +0x170 */
  float scale[2];       /* +0x17c */
  int stage;            /* +0x184 */
  RwV3d bpos[8];        /* +0x188 */
  RwV3d bvel[8];        /* +0x1e8 */
  float bsize[8][2];    /* +0x248 */
  float balpha[8];      /* +0x288 */
  float bfade[8];       /* +0x2a8 */
  float btime[8];       /* +0x2c8 */
  int bstage[8];        /* +0x2e8 */
  RwV3d cpos[8];        /* +0x308 */
  RwV3d cvel[8];        /* +0x368 */
  float csize[8][2];    /* +0x3c8 */
  float ctime[8];       /* +0x408 */
  float camp[8];        /* +0x428 */
  int cstate[8];        /* +0x448 */
  int ccount[8];        /* +0x468 */
} Blast0076d980;        /* 0x488 */
typedef struct {
  int active;           /* +0x00 */
  RwV3d pos;            /* +0x04 */
  float size[2];        /* +0x10 */
  float vy;             /* +0x18 */
  RwRGBA colour;        /* +0x1c */
  float fade;           /* +0x20 */
} Puff00770718;         /* 0x24 */
typedef struct {
  int state;            /* +0x00 */
  RwV3d pos;            /* +0x04 */
  float size[2];        /* +0x10 */
  int stage;            /* +0x18 */
  float t;              /* +0x1c */
} Ring00766a00;         /* 0x20 */
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBAL(float, DAT_00769f48, 0x00769f48);
MGLOBAL(float, DAT_00769f40, 0x00769f40);
MGLOBAL(int, DAT_007706d0, 0x007706d0);
MGLOBAL(int, DAT_007669f8, 0x007669f8);
MGLOBAL(int, DAT_0076a0f8, 0x0076a0f8);
MGLOBAL(int, DAT_00769f44, 0x00769f44);
MGLOBAL(int, DAT_00771528, 0x00771528);
MGLOBALA(UvRect, DAT_00614530, 8, 0x00614530);
MGLOBALA(UvRect, DAT_006145b0, 15, 0x006145b0);
MGLOBAL(UvRect, DAT_00614690, 0x00614690);
MGLOBALA(int, DAT_007668f8, 32, 0x007668f8);
MGLOBALA(int, DAT_00766978, 32, 0x00766978);
MGLOBALA(int, DAT_00766f00, 16, 0x00766f00);
MGLOBALA(int, DAT_0076d900, 16, 0x0076d900);
MGLOBALA(int, DAT_0076d940, 16, 0x0076d940);
MGLOBALA(int, DAT_007706d8, 16, 0x007706d8);
MGLOBALA(Blast0076d980, DAT_0076d980, 10, 0x0076d980);
MGLOBALA(Puff00770718, DAT_00770718, 100, 0x00770718);
MGLOBALA(Ring00766a00, DAT_00766a00, 40, 0x00766a00);
double sin(double);
#pragma intrinsic(sin)
float FUN_00472650(float lo, float hi);
int FUN_00472690(int lo, int hi);
void FUN_004769a0(RwV3d *pos);
void FUN_004769d0(float *size);
void FUN_004769f0(RwRGBA *col);
void FUN_00476a30(float angle);
void FUN_00476a40(UvRect *uv);
void FUN_00476d00(int *batch);
void FUN_00476df0(int *batch);
int FUN_0048ebc0(int i);
void FUN_0048ebf0(RwV3d *pos, float size, int i);
void FUN_0048f290(RwV3d *pos, float r, float f18, RwRGBA *rgb);
void FUN_0048f420(RwV3d *pos, float r, int kind);
int FUN_0048fe70(RwV3d *pos);
// MATCH: 0x00490500 FUN_00490500
void FUN_00490500(void)
{
  RwRGBA mid = {0x80, 0x80, 0x80, 0xff};
  RwRGBA grey = {0x60, 0x60, 0x60, 0xff};
  RwRGBA white = {0xff, 0xff, 0xff, 0xff};
  RwRGBA black = {0, 0, 0, 0xff};
  RwRGBA c;
  UvRect uv;
  Blast0076d980 *rec;
  Puff00770718 *pf;
  Ring00766a00 *rg;
  int live;
  int *stage;
  float *scale;
  int i, j, cnt, st, r;
  float f, dy, lim;

  DAT_00769f48 += DAT_007f100c * 0.5f;
  DAT_00769f40 += DAT_007f100c * 0.4f;
  live = 0;
  DAT_007706d0 = 0;
  DAT_007669f8 = 0;
  DAT_0076a0f8 = 0;
  DAT_00769f44 = 0;
  rec = DAT_0076d980; scale = DAT_0076d980[0].scale; stage = &DAT_0076d980[0].stage;
  for (i = 0; i < 10; i++, rec++, scale += 0x488/4, stage += 0x488/4) {
    if (rec->state == 0) continue;
    live++;
    white.alpha = 0xff;
    FUN_004769f0(&white);
    if (rec->m < rec->n && *stage == 8) {
      if (rec->timer == 0.0f) {
        rec->timer = FUN_00472650(0.01f, 0.02f);
      } else {
        rec->timer -= DAT_007f100c;
        if (rec->timer < 0.0f) FUN_0048ebf0(&rec->pos, rec->size, i);
      }
    }
    if (*stage < 8) *stage++;
    if (*stage < 4) {
      scale[0] *= 1.1f;
      scale[1] *= 1.1f;
    } else {
      scale[0] *= 0.9090909f;
      scale[1] *= 0.9090909f;
    }
    if (*stage < 8) {
      uv = DAT_00614530[*stage];
      FUN_00476a40(&uv);
      FUN_004769a0(&rec->centre);
      FUN_004769d0(scale);
      FUN_00476d00(DAT_007706d8);
      DAT_007669f8++;
    }
    cnt = FUN_0048ebc0(i) * rec->m;
    f = rec->size * 2.5f;
    if (f > 1.0f) f = 1.0f;
    for (j = 0; j < cnt; j++) {
      rec->apos[j].x += rec->avel[j].x;
      rec->apos[j].y += rec->avel[j].y;
      rec->apos[j].z += rec->avel[j].z;
      rec->asize[j][0] += f * 0.005f;
      rec->asize[j][1] += f * 0.005f;
      rec->aalpha[j] -= rec->afade[j];
      if (rec->aalpha[j] < 16.0f) rec->aalpha[j] = 0.0f;
      black.alpha = (unsigned char)rec->aalpha[j];
      if (black.alpha) {
        FUN_00476a30((j & 1) ? -DAT_00769f40 : DAT_00769f40);
        FUN_004769d0(rec->asize[j]);
        FUN_004769f0(&black);
        FUN_004769a0(&rec->apos[j]);
        FUN_00476d00(DAT_0076d900);
        DAT_00769f44++;
      }
    }
    if (rec->state == 1) {
      white.alpha = 0xff;
      FUN_004769f0(&white);
      for (j = 0; j < rec->count2; j++) {
        st = rec->cstate[j];
        if (st == 0) continue;
        rec->cpos[j].x += rec->cvel[j].x;
        rec->cpos[j].z += rec->cvel[j].z;
        rec->ctime[j] += DAT_007f100c;
        if (st == 3) {
          dy = rec->camp[j] = rec->camp[j] * 1.1f;
        } else {
          dy = sin(rec->ctime[j] * 8.0f) * rec->cvel[j].y;
          if (st == 1 && dy < 0.0f) rec->cstate[j] = 2;
          if (rec->cstate[j] == 2) {
            if (dy > rec->camp[j]) {
              rec->cstate[j] = 3;
              dy = -rec->cvel[j].y;
            }
            rec->camp[j] = dy;
          }
        }
        rec->cpos[j].y += dy;
        FUN_004769d0(rec->csize[j]);
        FUN_004769a0(&rec->cpos[j]);
        FUN_00476a30(DAT_00769f40 * 64.0f);
        FUN_00476d00(DAT_0076d940);
        DAT_007706d0++;
        r = FUN_0048fe70(&rec->cpos[j]);
        if (r == 0 || rec->cstate[j] < 2) {
          if (rec->ccount[j] != 0) {
            if (FUN_00472690(0, 0x19) == 0xf)
              FUN_0048f290(&rec->cpos[j], rec->csize[j][0] * 2.5f, 0.0025f, &grey);
          } else {
            if (FUN_00472690(0, 5) == 5)
              FUN_0048f290(&rec->cpos[j], rec->csize[j][0] * 5.0f, 0.0025f, &grey);
          }
        } else {
          if (r != 1 && rec->ccount[j] < FUN_00472690(1, 3)) {
            rec->cvel[j].y *= 0.44444445f;
            rec->ccount[j]++;
            rec->cstate[j] = 1;
            rec->ctime[j] = 0.0f;
            rec->camp[j] = 0.0f;
            rec->csize[j][0] *= 0.7692308f;
            rec->csize[j][1] *= 0.7692308f;
          } else {
            FUN_0048f420(&rec->cpos[j], rec->csize[j][0] + rec->csize[j][0], 0);
            rec->cstate[j] = 0;
          }
        }
      }
    }
    for (j = 0; j < rec->count3; j++) {
      f = rec->size * 2.5f;
      if (f > 1.0f) f = 1.0f;
      st = rec->bstage[j];
      rec->btime[j] += DAT_007f100c;
      uv = DAT_006145b0[st];
      f *= 0.0035f;
      rec->bsize[j][0] += f;
      rec->bsize[j][1] += f;
      if (st >= 8) {
        rec->bpos[j].x += rec->bvel[j].x;
        rec->bpos[j].y += rec->bvel[j].y;
        rec->bpos[j].z += rec->bvel[j].z;
        rec->balpha[j] -= rec->bfade[j];
      }
      if (rec->btime[j] > 0.03f) {
        rec->btime[j] = 0.0f;
        rec->bstage[j] = st + 1;
        if (st + 1 == 15) rec->bstage[j] = 14;
      }
      if (rec->balpha[j] < 16.0f) rec->balpha[j] = 0.0f;
      mid.alpha = (unsigned char)rec->balpha[j];
      if (mid.alpha) {
        FUN_004769f0(&mid);
        FUN_00476a40(&uv);
        FUN_004769a0(&rec->bpos[j]);
        FUN_004769d0(rec->bsize[j]);
        FUN_00476d00(DAT_00766f00);
        DAT_0076a0f8++;
      }
    }
    if (DAT_00769f44 == 0 && DAT_0076a0f8 == 0) rec->state = 0;
  }
  if (live == 0) DAT_00771528 = 0;
  for (i = 0; i < 100; i++) {
    pf = &DAT_00770718[i];
    if (pf->active == 0) continue;
    pf->pos.y += pf->vy;
    if ((float)pf->colour.alpha - pf->fade < 16.0f) {
      pf->active = 0;
    } else {
      pf->colour.alpha = (unsigned char)((float)pf->colour.alpha - pf->fade);
      if (pf->vy == 0.0f) {
        pf->pos.y += 0.0001f;
        pf->size[0] += 0.0075f;
        pf->size[1] += 0.0075f;
      }
      uv = DAT_00614690;
      FUN_00476a40(&uv);
      FUN_004769f0(&pf->colour);
      FUN_004769a0(&pf->pos);
      FUN_004769d0(pf->size);
      FUN_00476d00(DAT_007668f8);
    }
  }
  for (i = 0; i < 40; i++) {
    rg = &DAT_00766a00[i];
    st = rg->state;
    if (st == 0) continue;
    rg->t += DAT_007f100c;
    uv = DAT_006145b0[rg->stage];
    c.red = 0xff; c.green = 0xff; c.blue = 0xff; c.alpha = 0xff;
    if (st == 2) {
      lim = 0.02f;
      rg->pos.y += 0.01f;
    } else {
      lim = 0.04f;
    }
    if (rg->t > lim) {
      rg->stage++;
      rg->t = 0.0f;
      if (rg->stage == 1 && st != 2)
        FUN_0048f290(&rg->pos, rg->size[0] + rg->size[0], 0.001f, &grey);
      if (rg->stage == 15) {
        rg->state = 0;
        continue;
      }
    }
    c.alpha = 0xff - rg->stage * 12;
    FUN_00476a40(&uv);
    FUN_004769f0(&c);
    FUN_004769a0(&rg->pos);
    FUN_004769d0(rg->size);
    FUN_00476d00(DAT_00766978);
  }
  FUN_00476df0(DAT_00766978);
  FUN_00476df0(DAT_0076d940);
  FUN_00476df0(DAT_007668f8);
  FUN_00476df0(DAT_00766f00);
  FUN_00476df0(DAT_007706d8);
  FUN_00476df0(DAT_0076d900);
}
