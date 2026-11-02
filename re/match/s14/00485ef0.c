// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00485ef0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs14_20261102/results.tsv. */
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned __int64 ulonglong;
typedef struct MVec3 {
  float x;
  float y;
  float z;
} MVec3;

typedef struct BoneRec {
  int index;
  void *pool;
  MVec3 v[7][5];
  int flag[7];
  float f1c8[7];
  float f1e4[7];
  float f200[7];
  int f21c[7];
  undefined4 f238;
} BoneRec;

MGLOBALA(BoneRec, DAT_006fde10, 4, 0x006fde10);

typedef struct MMatrix {
  MVec3 right;
  undefined4 flags;
  MVec3 up;
  undefined4 pad1;
  MVec3 at;
  undefined4 pad2;
  MVec3 pos;
  undefined4 pad3;
} MMatrix;

void FUN_0041f1e0(int obj, int id, MMatrix *out);

// MATCH: 0x00485c20 FUN_00485c20
static int FUN_00485c20(int obj, int idx, MVec3 *out)
{
  MMatrix m;

  if (idx == 0) {
    FUN_0041f1e0(obj, 0x21, &m);
    *out = m.pos;
    return 0;
  }
  if (idx == 1) {
    FUN_0041f1e0(obj, 0x22, &m);
    *out = m.pos;
    return 1;
  }
  if (idx == 2) {
    FUN_0041f1e0(obj, 0x24, &m);
    *out = m.pos;
    return 2;
  }
  if (idx == 3) {
    FUN_0041f1e0(obj, 0x23, &m);
    *out = m.pos;
    return 3;
  }
  if (idx == 4) {
    FUN_0041f1e0(obj, 0x10, &m);
    *out = m.pos;
    return 4;
  }
  if (idx == 5) {
    FUN_0041f1e0(obj, 0x35, &m);
    *out = m.pos;
    return 5;
  }
  if (idx == 6) {
    FUN_0041f1e0(obj, 0x36, &m);
    *out = m.pos;
    return 6;
  }
  return 0;
}

typedef struct UVRect {
  float u0;
  float v0;
  float u1;
  float v1;
} UVRect;

typedef struct Rgba {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
} Rgba;

MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBALA(UVRect, DAT_005cf368, 4, 0x005cf368);

void FUN_00474de0(MVec3 *out, MVec3 *in, float s);
void FUN_004893b0(void *pool);
void FUN_004893d0(void *pool, MVec3 *pos, Rgba *col);
void FUN_00489450(void *pool, float *size);
void FUN_00489480(void *pool, int a);
void FUN_004894a0(void *pool, float *uv0, float *uv1);
void FUN_004894f0(void *pool);
void FUN_00489890(void *pool);

// MATCH: 0x00485ef0 FUN_00485ef0
void FUN_00485ef0(void)
{
  int k;
  int j;
  int i;
  int idx;
  BoneRec *r;
  MVec3 v;
  MVec3 pos;
  Rgba col;
  float t;
  float u;
  float frac;
  float scale;
  float size[2];

  for (k = 0; k < 4; k++) {
    r = &DAT_006fde10[k];
    for (j = 0; j < 7; j++) {
      if (r->flag[j] != 0) {
        r->f1c8[j] += DAT_007f100c;
        if (r->f1c8[j] >= r->f1e4[j]) {
          r->flag[j] = 0;
        }
        FUN_00485c20(k, r->f21c[j], &v);
        i = 5;
        while (--i != 0) {
          r->v[j][i] = r->v[j][i - 1];
        }
        r->v[j][0] = v;
      }
    }
    for (j = 0; j < 7; j++) {
      if (r->flag[j] == 0) {
        FUN_004894f0(r->pool);
      }
      else {
        for (i = 0; i < 5; i++) {
          t = (float)i;
          u = t * 0.2f;
          frac = r->f1c8[j] / r->f1e4[j];
          idx = (int)(u * 15.0f);
          pos = r->v[j][i];
          scale = r->f200[j];
          col.r = 0xff;
          col.g = 0xff;
          col.b = 0xff;
          if (j != 5 && j != 6) {
            pos.y = t * 0.03f + pos.y;
          }
          col.a = (unsigned char)(int)((1.0f - u) * 100.0f);
          FUN_00474de0(&pos, &pos, 0.02f);
          FUN_004893d0(r->pool, &pos, &col);
          size[0] = size[1] = (1.0f - frac) * scale;
          FUN_00489450(r->pool, size);
          FUN_00489480(r->pool, 0);
          FUN_004894a0(r->pool, &DAT_005cf368[idx].u0, &DAT_005cf368[idx].u1);
        }
        FUN_004893b0(r->pool);
      }
    }
    FUN_00489890(r->pool);
  }
}
