// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00422570 (890 B): best 98.5% exact (regs 98.5, ops 100.0) by hand + match_permute. Raw g2s 25.9%.
   Takes 4 stack args (Ghidra shows 3): the 4th is a float, col[3] = (uchar)(arg4 * 153.0f) via _ftol.
   Copies the 0x40-byte matrix at r->f38+0x20 to a local, builds a sphere {pos at arg2+0x30, 1.5f}; when the
   squared distance to the per-player point DAT_006403b8[i] exceeds 0.125 (double 0x005cd318) it stores the new
   point, clears r->count and collects up to 0x40 0x40-byte entries with FUN_004b4ad0 (returns on 0). Each entry
   is dropped on material colour 0xff010101/0xff0000ff/0xffff0080, on any FUN_00472500(0..4) colour, or on
   entry+4 < 0.75f; FUN_00477e60 builds a plane from its 3 vertices, FUN_00477f50(plane, pos, &t) result 0 needs
   t <= 1.0f, result 2 needs t <= 0.1f; kept triangles are appended with y += r->f3c (0.01f). Then each kept
   triangle goes through FUN_004c3d90(out, tri, 3, &matrix) and x/y of the 3 outputs land in the uv slots.
   What moved it: dz*dz + (dx*dx + dy*dy) for the x87 order (87.6 -> 89.1), index loop buf[k] (88.7),
   a local TriPos pointer tp = &buf[k].v as the loop's first statement, which moves the induction pointer bias
   to entry+0x10 like the original (89.1 -> 98.5).
   Residual (b): the 2nd and 3rd y += r->f3c load f3c first (fld [ebx+0xf3c]; fadd [eax+..]) where the original
   loads the vertex first; the 1st one matches. Tried +=, a + b, b + a, a float* to f3c, a TriPos/V3 pointer
   for d, separate increment, explicit cast, and match_permute (no gain). */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3_570;
typedef struct { V3_570 c; float r; } Sphere_570;
typedef struct { float m[16]; } Mat_570;
typedef struct { V3_570 p[3]; } TriPos_570;
typedef struct { TriPos_570 pos; float uv[3][2]; } Tri_570;
typedef struct { char pad[0x20]; Mat_570 m; } Obj38_570;
typedef struct { unsigned char r, g, b, a; } RGBA_570;
typedef struct { int f0; RGBA_570 color; } Material_570;
typedef struct { unsigned short v0, v1, v2, mat; } GeoTri_570;
typedef struct { int f0; GeoTri_570 *tris; char pad[0x78]; unsigned short matBase; } Geo_570;
typedef struct { char pad[0x10]; Material_570 **list; } Mats_570;
typedef struct {
  int f0;
  float f4;
  int f8;
  int idx;                 /* 0x0c */
  TriPos_570 v;            /* 0x10 */
  Geo_570 *geo;            /* 0x34 */
  int f38;
  Mats_570 *mats;          /* 0x3c */
} Entry_570;
typedef struct {
  Tri_570 tri[64];         /* 0x000 */
  void *tex[4];            /* 0xf00 */
  float f10, f14, f18, f1c, f20, f24;
  unsigned char col[4];    /* 0xf28 */
  int f2c;
  int count;               /* 0xf30 */
  int f34;
  Obj38_570 *f38;          /* 0xf38 */
  float f3c;
} Rec_570;
#define PACK_570(c) (((unsigned int)(c).a << 24) | ((unsigned int)(c).b << 16) | ((unsigned int)(c).g << 8) | (c).r)
MGLOBALA(Rec_570, DAT_006403e8, 4, 0x006403e8);
MGLOBALA(V3_570, DAT_006403b8, 4, 0x006403b8);
unsigned int FUN_00472500(int k);
void FUN_00477e60(float *plane, V3_570 *a, V3_570 *b, V3_570 *c);
int FUN_00477f50(float *plane, V3_570 *p, float *t);
int FUN_004b4ad0(int world, Sphere_570 *s, Entry_570 *buf, int max);
V3_570 *FUN_004c3d90(V3_570 *out, V3_570 *in, int n, Mat_570 *m);
// MATCH: 0x00422570 FUN_00422570
void FUN_00422570(int i, char *mtx, int world, float alpha)
{
  Rec_570 *r = &DAT_006403e8[i];
  V3_570 *pos;
  Mat_570 m;
  Sphere_570 s;
  Entry_570 buf[64];
  Entry_570 *e;
  float t;
  float dx, dy, dz;
  int n, k;

  r->col[3] = (unsigned char)(alpha * 153.0f);
  pos = (V3_570 *)(mtx + 0x30);
  m = r->f38->m;
  s.c = *pos;
  n = 0;
  s.r = 1.5f;
  dx = DAT_006403b8[i].x - s.c.x;
  dy = DAT_006403b8[i].y - s.c.y;
  dz = DAT_006403b8[i].z - s.c.z;
  if (dz * dz + (dx * dx + dy * dy) > 0.125) {
    DAT_006403b8[i] = s.c;
    r->count = 0;
    n = FUN_004b4ad0(world, &s, buf, 0x40);
    if (n == 0)
      return;
  }
  r->f3c = 0.01f;
  for (k = 0; k < n; k++) {
    {
      TriPos_570 *tp = &buf[k].v;
      RGBA_570 *c = &buf[k].mats->list[buf[k].geo->tris[buf[k].idx].mat + buf[k].geo->matBase]->color;
      unsigned int cv = PACK_570(*c);
      if (cv != 0xff010101 && cv != 0xff0000ff && cv != 0xffff0080 &&
          PACK_570(*c) != FUN_00472500(0) && PACK_570(*c) != FUN_00472500(1) &&
          PACK_570(*c) != FUN_00472500(2) && PACK_570(*c) != FUN_00472500(3) &&
          PACK_570(*c) != FUN_00472500(4) && !(buf[k].f4 < 0.75f)) {
        float plane[4];
        int res;
        FUN_00477e60(plane, tp->p, tp->p + 1, tp->p + 2);
        res = FUN_00477f50(plane, pos, &t);
        if (res == 0) {
          if (t > 1.0f)
            goto next;
        } else if (res == 2) {
          if (t > 0.1f)
            goto next;
        }
        {
          Tri_570 *d = &r->tri[r->count++];
          d->pos = *tp;
          d->pos.p[0].y = d->pos.p[0].y + r->f3c;
          d->pos.p[1].y = d->pos.p[1].y + r->f3c;
          d->pos.p[2].y = d->pos.p[2].y + r->f3c;
        }
      }
next:
      ;
    }
  }
  for (k = 0; k < r->count; k++) {
    V3_570 out[3];
    FUN_004c3d90(out, r->tri[k].pos.p, 3, &m);
    r->tri[k].uv[0][0] = out[0].x;
    r->tri[k].uv[0][1] = out[0].y;
    r->tri[k].uv[1][0] = out[1].x;
    r->tri[k].uv[1][1] = out[1].y;
    r->tri[k].uv[2][0] = out[2].x;
    r->tri[k].uv[2][1] = out[2].y;
  }
}
