// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-FRESH4 0x00486830 (first hand source; raw g2s 26.4%): 46.7% (regs 56.9, ops 87.8),
   442/469 ins, 1331 of 1671 B differ. Kept as the next slice's start, not promoted.
   Shape done (from the listing, Ghidra's decomp of the colour bytes was CONCAT noise): 250 iterations, the
   first 150 on the particle pool at 0x006fe790 (0x7c B records), the last 100 on the trail pool at 0x006fa5d0
   (0x90 B records) indexed [i - 150] (the original's start pointer is 0x006fa5d0 - 0x5460); the inlined
   RwMatrixSetIdentity macro (chained stores, flags |= 0x20003) before FUN_004c51a0 / FUN_00452140; the alpha
   byte as (unsigned char)(int)(255.0f - t * DAT_005cf478) or (DAT_005cf474 - t * DAT_005cf474); 0x005cf468 /
   0x005cf46c / 0x005cf470..0x005cf478 are globals (two separate 0.5f slots, so not pooled literals); the
   bubble sort on dists[150] / ptrs[150] with `while (swapped == 1)`.
   Residuals: (1) the original frame is 8-aligned (push ebp / and esp,-8 / sub esp,0x5e0). A double local
   gives it only when it is dist2 (48.6%), but the original stores dist2 as a dword at [esp+0x30], so that
   form is not kept; double dx/dy/dz, tf, f: no alignment. (2) the original walks six pointer IVs (src,
   delta, dist2, particle, &particle.size for the loop end, the pool-B pointer); ours walks one or two.
   Separate symbols for the trail fields gave the src IV but 43.1%. (3) the 0x70 B gap between the light
   request and dists[] in the original frame is not reproduced (a padded request: 46.1%). */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
typedef struct { float x, y, z; } RwV3d;
typedef struct { float x, y, z; } V3;
typedef struct { float x, y; } V2;
typedef struct { float u0, v0, u1, v1; } UV4;
typedef struct { V3 right; unsigned int flags; V3 up; int pad1; V3 at; int pad2; V3 pos; int pad3; } RwMatrix;
typedef struct {
  V3 pos;
  unsigned int frame;
  float f10;
  float size;
  float rate;
  float anim;
  float age;
  float t;
  float f28;
  float life;
  V3 dir;
  char pad[0x34];
  int active;
  int kind;
  int glow;
} Particle;
typedef struct {
  V3 *src;
  V3 delta;
  float dist2;
  Particle p;
} Trail;
typedef struct {
  V3 pos;
  float radius;
  int type;
  Particle *user;
  int zero;
  void (*cb)(void);
} LightReq;
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBAL(float, DAT_005cf468, 0x005cf468);
MGLOBAL(float, DAT_005cf46c, 0x005cf46c);
MGLOBAL(unsigned int, DAT_005cf470, 0x005cf470);
MGLOBAL(float, DAT_005cf474, 0x005cf474);
MGLOBAL(float, DAT_005cf478, 0x005cf478);
MGLOBALA(UV4, DAT_005cf368, 16, 0x005cf368);
MGLOBALA(Trail, DAT_006fa5d0, 100, 0x006fa5d0);
MGLOBALA(Particle, DAT_006fe790, 150, 0x006fe790);
MGLOBAL(int, DAT_006fe700, 0x006fe700);
MGLOBAL(int, DAT_006fe740, 0x006fe740);
void FUN_004417e0(V3 *out);
int FUN_00426c00(void);
float FUN_00472650(float lo, float hi);
void FUN_004c51a0(RwMatrix *m, V3 *v, int op);
void FUN_00452140(void *out, void *in);
void FUN_004769d0(V2 *sz);
void FUN_00476a40(UV4 *uv);
void FUN_004769a0(V3 *pos);
void FUN_004769f0(unsigned int *col);
void FUN_00476a10(V3 *dir);
void FUN_00476d00(int *state);
void FUN_00476df0(int *state);
void FUN_00486630(void);
void FUN_00484cf0(LightReq *req);
void FUN_00466100(int a, int b, float c);
void FUN_00465ca0(int a, Particle *p);
void FUN_00466000(int a, Particle *p, int k, float vol, float freq);

// MATCH: 0x00486830 FUN_00486830
void FUN_00486830(void)
{
  int i;
  int n;
  unsigned int col;
  int flag;
  float dist2;
  V3 d;
  V3 cam;
  V3 pos2;
  V2 sz;
  UV4 uv;
  RwMatrix mat;
  LightReq req;
  float dists[150];
  Particle *ptrs[150];
  Particle *p;
  Trail *tr;
  V3 *src;
  float f;
  float dx;
  float dy;
  float dz;
  int swapped;
  int k;
  float tf;
  Particle *tp;

  i = 0;
  n = 0;
  FUN_004417e0(&cam);
  for (i = 0; i < 250; i++) {
    flag = i >= 150;
    if (flag) {
      tr = &DAT_006fa5d0[i - 150];
      src = tr->src;
      p = &tr->p;
      if (src == 0 || (src->x == 0.0f && src->y == 0.0f && src->z == 0.0f)) {
        p->active = 0;
      }
      else {
        d.x = p->pos.x - src->x;
        d.y = p->pos.y - src->y;
        d.z = p->pos.z - src->z;
        dist2 = d.z * d.z + (d.y * d.y + d.x * d.x);
        tr->dist2 = dist2;
        tr->delta = d;
        p->pos = *src;
        if (dist2 > 0.01f) {
          p->pos.x = FUN_00472650(-0.02f, 0.02f) + p->pos.x;
          p->pos.y = FUN_00472650(-0.02f, 0.02f) + p->pos.y;
          p->pos.z = FUN_00472650(-0.02f, 0.02f) + p->pos.z;
        }
        if (dist2 > 0.02f) {
          p->pos.x = FUN_00472650(-0.04f, 0.04f) + p->pos.x;
          p->pos.y = FUN_00472650(-0.04f, 0.04f) + p->pos.y;
          p->pos.z = FUN_00472650(-0.04f, 0.04f) + p->pos.z;
          p->size = FUN_00472650(-0.02f, 0.02f) + p->size;
        }
        if (p->active != 0) {
          dx = cam.x - p->pos.x;
          dy = cam.y - p->pos.y;
          dz = cam.z - p->pos.z;
          ptrs[n] = p;
          dists[n] = dz * dz + (dy * dy + dx * dx);
          n++;
        }
      }
    }
    else {
      p = &DAT_006fe790[i];
    }
    if (p->active != 0 && p->kind != 1) {
      if (FUN_00426c00() == 0x21) {
        mat.right.x = mat.up.y = mat.at.z = 1.0f;
        mat.right.y = mat.right.z = mat.up.x = 0.0f;
        mat.up.z = mat.at.x = mat.at.y = 0.0f;
        mat.pos.x = mat.pos.y = mat.pos.z = 0.0f;
        mat.flags = mat.flags | 0x20003;
        FUN_004c51a0(&mat, &p->pos, 0);
        FUN_00452140(&mat, &mat);
        p->pos = mat.pos;
        FUN_00452140(&p->dir, &p->dir);
      }
      p->age = DAT_007f100c + p->age;
      p->t = p->age / p->life;
      if (p->age > p->life) {
        p->t = 1.0f;
        p->active = 0;
        if (flag) {
          tr->src = 0;
        }
      }
      if (p->t < 0.02f) {
        f = p->t * 50.0f;
      }
      else {
        f = 1.0f - p->t;
      }
      sz.y = f * p->size * DAT_005cf46c;
      sz.x = DAT_005cf468 * f * p->size * 1.3f;
      FUN_004769d0(&sz);
      uv = DAT_005cf368[p->frame];
      FUN_00476a40(&uv);
      p->anim = DAT_007f100c + p->anim;
      p->frame = (int)(p->anim * p->rate * 30.0f);
      if (p->frame > 15) {
        p->frame = 0;
        p->anim = 0.0f;
      }
      pos2.x = p->pos.x;
      if (flag) {
        pos2.y = p->pos.y;
      }
      else {
        pos2.y = (1.0f - p->t) * 0.3f * p->anim + p->pos.y;
      }
      pos2.z = p->pos.z;
      FUN_004769a0(&pos2);
      col = DAT_005cf470;
      if (flag) {
        ((unsigned char *)&col)[3] = (unsigned char)(int)(255.0f - p->t * DAT_005cf478);
      }
      else {
        ((unsigned char *)&col)[3] = (unsigned char)(int)(DAT_005cf474 - p->t * DAT_005cf474);
      }
      FUN_004769f0(&col);
      FUN_00476d00(&DAT_006fe700);
      if (p->glow != 0) {
        ((unsigned char *)&col)[0] = ((unsigned char *)&col)[1] = ((unsigned char *)&col)[2] = 0x10;
        ((unsigned char *)&col)[3] = (unsigned char)(int)(255.0f - p->t * 255.0f);
        FUN_004769f0(&col);
        FUN_00476a10(&p->dir);
        FUN_00476d00(&DAT_006fe740);
        dx = cam.x - p->pos.x;
        dy = cam.y - p->pos.y;
        dz = cam.z - p->pos.z;
        ptrs[n] = p;
        dists[n] = dz * dz + (dy * dy + dx * dx);
        n++;
        if (p->glow != 0) {
          req.pos = p->pos;
          req.radius = 0.15f;
          req.user = p;
          req.zero = 0;
          req.cb = FUN_00486630;
          req.type = 0x11;
          FUN_00484cf0(&req);
        }
      }
    }
  }
  FUN_00476df0(&DAT_006fe700);
  FUN_00476df0(&DAT_006fe740);
  do {
    swapped = 0;
    for (k = 0; k < n - 1; k++) {
      if (dists[k + 1] < dists[k]) {
        tf = dists[k + 1];
        dists[k + 1] = dists[k];
        dists[k] = tf;
        tp = ptrs[k];
        ptrs[k] = ptrs[k + 1];
        ptrs[k + 1] = tp;
        swapped = 1;
      }
    }
  } while (swapped == 1);
  for (k = 0; k < 4; k++) {
    FUN_00466100(0x20, k, 0.0f);
  }
  if (n > 0) {
    if (n > 4) {
      n = 4;
    }
    for (i = 0; i < n; i++) {
      p = ptrs[i];
      FUN_00465ca0(0x20, p);
      FUN_00466000(0x20, p, i, (1.0f - p->age / p->life) * 0.6f, (float)i * 1000.0f + 22050.0f);
    }
  }
}
