// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047bf70 (h3): 100% (309 B, BYTE-IDENTICAL). This file also holds its three static callees (pilot finding 1, real callers in the file, no stand-in): 0x0047bc90 100% (36 B; register args ESI = edge a, EDX = edge b), 0x0047bcc0 BYTE-IDENTICAL (insn 98.7% only because Ghidra's size 431 stops 3 B before the end; --bytes 434 B, 0 differing; register arg EAX = triangle count, stack = out list, triangles), 0x0047be80 86.4% (regs 88.9%; register args ECX = normals out, EDX = triangles, EBX = vertices, stack = count). Raw: 9.9 / 45.5 / 26.1 / 14.7%. What moved them: 0x0047bcc0 triangles indexed tris[i*4+k] (a running pointer made the compiler pass it in a register and ntri in ECX), pair loop over &edges[i]/&edges[j]; 0x0047bf70 sector+4 read first and tris before verts in the declarations (67 -> 95%), and the edge-byte loop reading the low bytes of the Edge ints through an unsigned char pointer at &e->v1 (+0x14) and writing through &rec->edge[0].idx (+0xf2), which is the pointer bias the original shows. 0x0047be80: the vertex indices go through a small __inline helper into a local unsigned short[3] (that array is what /GS protects and is the dead word store at frame+4; 59 -> 86%). 0x0047be80 residual, class (b): the original tests the count before saving EBP/ESI/EDI (shrink-wrapped prologue, ours saves ESI first) and one fsub is scheduled one instruction later; tried for/do-while/while, down counters, pointer walk, struct copy of RpTriangle (6 and 8 B), declaration orders, permuter (171 compiles). */
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
#ifdef __cplusplus
#define MATCH_STATIC
#else
#define MATCH_STATIC static
#endif
typedef struct RwV3d { float x, y, z; } RwV3d;
typedef struct Edge {
  unsigned short *tri;
  int pair;
  int idx;
  int pairidx;
  int v0;
  int v1;
} Edge;
typedef struct RpTri { unsigned short v[3]; unsigned short mat; } RpTri;
typedef struct EdgeList { Edge *e; int n; } EdgeList;
typedef struct RwGlobalsT {
  char pad[0x108];
  void *(*malloc)(unsigned int size, unsigned int hint);
  void (*free)(void *p);
} RwGlobalsT;
typedef struct EdgeB { unsigned char v0, v1, idx, pairidx; } EdgeB;
typedef struct SectRec {
  RwV3d normal[12];
  int bound[0x18];
  EdgeB edge[18];
  int area[4];
  int nedge;
  int nvert;
  int ntri2;
  int ntri;
} SectRec;
MGLOBAL(RwGlobalsT *, DAT_007d3ff8, 0x007d3ff8);
void *memset(void *, int, unsigned int);
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memset, memcpy)
float FUN_004c39b0(RwV3d *out, RwV3d *in);
int FUN_004c0ed0(int p);
int FUN_004c3d90(void *a, void *b, int c, int d);
void FUN_004b53b0(void *a, void *b, int c);

// MATCH: 0x0047bc90 FUN_0047bc90
MATCH_STATIC int FUN_0047bc90(Edge *a, Edge *b)
{
  if ((a->v0 == b->v0 || a->v0 == b->v1) && (a->v1 == b->v0 || a->v1 == b->v1)) {
    return 1;
  }
  return 0;
}

// MATCH: 0x0047bcc0 FUN_0047bcc0
MATCH_STATIC void FUN_0047bcc0(int ntri, EdgeList *out, unsigned short *tris)
{
  int nedge = ntri * 3;
  int count = 0;
  Edge *edges;
  Edge *e;
  Edge *f;
  int i;
  int j;

  edges = (Edge *)DAT_007d3ff8->malloc(nedge * sizeof(Edge), 0);
  e = edges;
  memset(edges, 0, nedge * sizeof(Edge));
  for (i = 0; i < ntri; i++) {
    e->v0 = tris[i * 4];
    e->v1 = tris[i * 4 + 1];
    e->tri = &tris[i * 4];
    e->pair = 0;
    e->idx = i;
    e->pairidx = -1;
    e++;
    e->v0 = tris[i * 4 + 1];
    e->v1 = tris[i * 4 + 2];
    e->tri = &tris[i * 4];
    e->idx = i;
    e->pair = 0;
    e->pairidx = -1;
    e++;
    e->v0 = tris[i * 4 + 2];
    e->v1 = tris[i * 4];
    e->tri = &tris[i * 4];
    e->idx = i;
    e->pair = 0;
    e->pairidx = -1;
    e++;
  }
  for (i = 0; i < nedge - 1; i++) {
    e = &edges[i];
    if (e->tri != 0) {
      for (j = i + 1; j < nedge; j++) {
        f = &edges[j];
        if (f->tri != 0 && FUN_0047bc90(e, f)) {
          e->pair = (int)f->tri;
          e->pairidx = f->idx;
          f->tri = 0;
          f->idx = -1;
        }
      }
    }
  }
  for (i = 0; i < nedge; i++) {
    if (edges[i].tri != 0 && edges[i].pair != 0) count++;
  }
  out->e = (Edge *)DAT_007d3ff8->malloc(count * sizeof(Edge), 0);
  out->n = count;
  j = 0;
  for (i = 0; i < nedge; i++) {
    if (edges[i].tri != 0 && edges[i].pair != 0) {
      out->e[j] = edges[i];
      j++;
    }
  }
  DAT_007d3ff8->free(edges);
}

#ifndef MATCH_PORT
static __inline void TriIdx(unsigned short *tri, unsigned short *idx)
{
  idx[0] = tri[0];
  idx[1] = tri[1];
  idx[2] = tri[2];
}

// HELD: 0x0047be80 (body elsewhere; this copy exists only so the DDK compiles the caller as the original; not compiled in the port)
static void FUN_0047be80(RwV3d *out, unsigned short *tris, RwV3d *verts, int ntri)
{
  int i;
  unsigned short t[3];
  RwV3d a;
  RwV3d b;

  for (i = 0; i < ntri; i++) {
    TriIdx(&tris[i * 4], t);
    a.x = verts[t[2]].x - verts[t[1]].x;
    a.y = verts[t[2]].y - verts[t[1]].y;
    a.z = verts[t[2]].z - verts[t[1]].z;
    b.x = verts[t[0]].x - verts[t[1]].x;
    b.y = verts[t[0]].y - verts[t[1]].y;
    b.z = verts[t[0]].z - verts[t[1]].z;
    out->x = b.z * a.y - b.y * a.z;
    out->y = b.x * a.z - b.z * a.x;
    out->z = b.y * a.x - b.x * a.y;
    FUN_004c39b0(out, out);
    out++;
  }
}
#else
void FUN_0047be80(RwV3d *out, unsigned short *tris, RwV3d *verts, int ntri);
#endif

// MATCH: 0x0047bf70 FUN_0047bf70
void FUN_0047bf70(SectRec *rec, int sector)
{
  int sec4 = *(int *)(sector + 4);
  int geom = *(int *)(sector + 0x18);
  unsigned short *tris = *(unsigned short **)(geom + 0x2c);
  int verts = *(int *)(*(int *)(geom + 0x5c) + 0x14);
  int ntri = *(int *)(geom + 0x10);
  int nvert = *(int *)(geom + 0x14);
  EdgeList el;
  RwV3d *normals;
  int i;

  el.e = 0;
  el.n = 0;
  normals = (RwV3d *)DAT_007d3ff8->malloc(ntri * sizeof(RwV3d), 0);
  FUN_0047be80(normals, tris, (RwV3d *)verts, ntri);
  FUN_0047bcc0(ntri, &el, tris);
  rec->ntri = ntri;
  rec->nvert = nvert;
  FUN_004c3d90(rec->bound, (void *)verts, nvert, FUN_004c0ed0(sec4));
  rec->ntri2 = ntri;
  memcpy(rec->normal, normals, ntri * sizeof(RwV3d));
  rec->nedge = el.n;
  if (el.n > 0) {
    unsigned char *s = (unsigned char *)&el.e->v1;
    unsigned char *d = &rec->edge[0].idx;
    for (i = el.n; i != 0; i--) {
      d[-2] = s[-4];
      d[-1] = s[0];
      d[0] = s[-0xc];
      d[1] = s[-8];
      d += 4;
      s += sizeof(Edge);
    }
  }
  DAT_007d3ff8->free(normals);
  DAT_007d3ff8->free(el.e);
  FUN_004b53b0(rec->area, rec->bound, rec->nvert);
}
