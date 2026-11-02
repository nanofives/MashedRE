// FLAGS: /O2 /GS /Ob1
// h11: 76.8% (regs 77.4%, ops 81.8%), 544/452 ins. Hand from decomp: RW geometry builder (RpGeometryCreate 0x004e8c70, morph target verts/normals, texcoords, per-strip triangle fan), source mesh struct, byte-offset normal index (fixes the [edx+ecx] form).
/* MATCH-S13 0x00482140: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs13_20261102/results.tsv. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef struct { float x; float y; float z; } V3;
typedef struct { float u; float v; } UV;
typedef struct { float x; float y; float z; float radius; } Sphere;
typedef struct { unsigned short a; unsigned short b; unsigned short vert; unsigned short c; unsigned short d; unsigned short normal; } SrcTri;
typedef struct {
  int f0;
  V3 *verts;              /* +0x04 */
  unsigned short *polyStart;  /* +0x08 */
  V3 *normals;            /* +0x0c */
  unsigned short *stripStart; /* +0x10 */
  SrcTri *tris;           /* +0x14 */
  unsigned short *polyTris;   /* +0x18 */
  unsigned short numPolys;    /* +0x1c */
  unsigned short numStrips;   /* +0x1e */
  char pad[0x44 - 0x20];
  V3 center;              /* +0x44 */
} SrcMesh;
typedef struct { int f0; Sphere sphere; V3 *verts; V3 *normals; } MorphTarget;
typedef struct { char pad[0x2c]; int triangles; char pad2[4]; UV *texCoords; char pad3[0x5c - 0x38]; MorphTarget *morph; } Geom;
int FUN_004e67b0(void);
void FUN_00482030(float *p0, unsigned int p1, unsigned int p2);
void FUN_004820e0(float *p0, float *p1, float *p2);
void FUN_004c39b0(V3 *p0, V3 *p1);
void FUN_004e68a0(int p0, Geom *p1, int p2);
int FUN_004e7fa0(void);
void FUN_004e8030(int p0);
void FUN_004e8640(MorphTarget *p0, Sphere *p1);
void FUN_004e8910(Geom *p0, int p1, int p2, int p3, int p4);
void FUN_004e8940(Geom *p0, int p1, int p2);
void FUN_004e8a10(Geom *p0);
Geom *FUN_004e8c70(int p0, int p1, int p2);
void FUN_004e8ea0(Geom *p0);
// MATCH: 0x00482140 FUN_00482140
int FUN_00482140(SrcMesh *param_1, int param_2)
{
  unsigned int n;
  int numTris;
  unsigned short *ss;
  Geom *geom;
  MorphTarget *mt;
  V3 *verts;
  V3 *normals;
  UV *uv;
  int mat;
  unsigned int i;
  unsigned int j;
  int cnt;
  int idx;
  int tl;
  int k;
  int a;
  int b;
  int c;
  int base;
  Sphere sph;
  int atom;

  numTris = 0;
  n = param_1->numStrips;
  if (n > 0) {
    ss = param_1->stripStart;
    do {
      numTris += ss[1] - ss[0] - 2;
      ss++;
    } while (--n);
  }
  geom = FUN_004e8c70(param_2 ? param_1->numPolys : numTris * 3, numTris, 0x74);
  mt = geom->morph;
  verts = mt->verts;
  normals = mt->normals;
  uv = geom->texCoords;
  mat = FUN_004e7fa0();
  if (param_2 != 0) {
    for (i = 0; i < param_1->numPolys; i++) {
      verts[i] = param_1->verts[i];
      normals[i].x = 0.0f;
      normals[i].y = 0.0f;
      normals[i].z = 0.0f;
      FUN_004820e0(&uv[i].u, &param_1->center.x, &verts[i].x);
      cnt = param_1->polyStart[i + 1] - param_1->polyStart[i];
      for (j = 0; j < cnt; j++) {
        idx = 12 * param_1->tris[param_1->polyTris[param_1->polyStart[i] + j]].normal;
        normals[i].x = (*(V3 *)((char *)param_1->normals + idx)).x + normals[i].x;
        normals[i].y = (*(V3 *)((char *)param_1->normals + idx)).y + normals[i].y;
        normals[i].z = (*(V3 *)((char *)param_1->normals + idx)).z + normals[i].z;
      }
      FUN_004c39b0(&normals[i], &normals[i]);
    }
  }
  tl = geom->triangles;
  k = 0;
  for (i = 0; i < param_1->numStrips; i++) {
    cnt = param_1->stripStart[i + 1] - param_1->stripStart[i];
    for (j = 0; j < cnt - 2; j++) {
      FUN_004e8940(geom, tl, mat);
      base = param_1->stripStart[i];
      a = param_1->tris[base].vert;
      b = param_1->tris[base + j + 1].vert;
      c = param_1->tris[base + j + 2].vert;
      if (param_2 != 0) {
        FUN_004e8910(geom, tl, a, b, c);
      } else {
        FUN_004e8910(geom, tl, k, k + 1, k + 2);
        verts[k] = param_1->verts[a];
        verts[k + 1] = param_1->verts[b];
        verts[k + 2] = param_1->verts[c];
        normals[k] = param_1->normals[i];
        normals[k + 1] = param_1->normals[i];
        normals[k + 2] = param_1->normals[i];
        FUN_00482030(&uv[k].u, 0, cnt);
        FUN_00482030(&uv[k + 1].u, j + 1, cnt);
        FUN_00482030(&uv[k + 2].u, j + 2, cnt);
      }
      tl += 8;
      k += 3;
    }
  }
  FUN_004e8640(mt, &sph);
  mt->sphere = sph;
  FUN_004e8a10(geom);
  atom = FUN_004e67b0();
  FUN_004e68a0(atom, geom, 1);
  FUN_004e8030(mat);
  FUN_004e8ea0(geom);
  return atom;
}
