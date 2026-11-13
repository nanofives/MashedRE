// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* RWC-MATCH run 40: compiles (was C2115/C2069). FUN_0053e430, FUN_0053e730, FUN_0053e760 prototypes copied from their own
   sources in re/match/rwl03. 28.1% rw9044, 24.1% ddk, not worked further. */
/* MATCH-RWC-NEAR-B (2026-11-12): rewritten from the raw g2s body as typed C. Build-param struct (0x3c bytes) with the
   bbox as floats (the raw body converted them from uint, fild), RW GETCOORD byte-offset axes, the RW inline-asm
   RwInt32FromReal (fnstcw / or ah,0xc / fldcw / fistp / fldcw, 0x0053e125..0x0053e142), main body nested in
   `else if (depth < 0x20)` so the error tail is laid out last (0x0053e3db). 28.1% -> 71.9% rw9044 (35.5% ddk), 473/474 ins.
   match_permute.py reached 74.7% only by reading `hi` before it is set, rejected. Residual: stack slot assignment,
   the FUN_0053e690 float arguments (fld/fstp in the original, push here, 0x0053dfec). Not 100%, not promoted. */
/* MATCH-RWL03 0x0053dea0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(float, DAT_005cd03c, 0x005cd03c);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005ceac0, 0x005ceac0);
MGLOBAL(float, DAT_005cd120, 0x005cd120);
typedef void *(*MallocFn)(unsigned int, unsigned int);
typedef void (*FreeFn)(void *);
#define RwMalloc(s,h) ((*(MallocFn *)(DAT_007d3ff8 + 0x108))((s),(h)))
#define RwFree(p) ((*(FreeFn *)(DAT_007d3ff8 + 0x10c))((p)))
#define GETCOORD(v, o) (*(float *)((char *)&(v) + (o)))
typedef struct { float x, y, z; } V3;
typedef struct { V3 *pos; unsigned int flags; float key; } CVert;
typedef struct { unsigned short *idx; int unk; } CTri;
typedef struct { unsigned int numVerts; CVert *verts; CTri *tris; int unk; unsigned int depth; V3 sup; V3 inf;
                 unsigned int numTris; unsigned int firstTri; unsigned int numSorted; CVert **sorted; } BuildParam;
typedef struct { unsigned int numLeft; unsigned int numRight; float leftValue; float rightValue; } Split;
typedef struct { int type; unsigned short numTris; unsigned short firstTri; } Leaf;
typedef struct { int type; void *left; void *right; } Node;
typedef struct { int pluginID; int errorCode; } Err;
int FUN_004d7ff0(int p0);
void FUN_004d8480(Err *p0);
void FUN_0053de50(Node *p0);
void FUN_0053e430(BuildParam *p0, int p1, float p2, Split *p3);
float FUN_0053e5d0(BuildParam *p0, Split *p1, int p2);
float FUN_0053e690(float p0, float p1);
void FUN_0053e6f0(BuildParam *p0);
int FUN_0053e6d0(const void *a, const void *b);
void *FUN_0053e730(unsigned int p0, unsigned int p1);
Node *FUN_0053e760(int p0, float p1, float p2);
void FUN_005c24e0(void *p0, unsigned int p1, unsigned int p2, int (*p3)(const void *, const void *));
#define RwInt32FromReal(d, s) { float _t = (s); unsigned short _cw, _cw2; __asm { fnstcw _cw } __asm { fld _t } \
   __asm { mov ax, _cw } __asm { or ah, 0xc } __asm { mov _cw2, ax } __asm { fldcw _cw2 } __asm { fistp d } __asm { fldcw _cw } }
// MATCH: 0x0053dea0 FUN_0053dea0
void *FUN_0053dea0(BuildParam *p)
{
  unsigned int n;
  unsigned int i;
  unsigned int numSorted;
  CTri *tri;
  int axis;
  int bestAxis;
  float bestScore;
  float bestValue;
  float size[3];
  int maxAxis;
  float *maxSize;
  float minExtent;
  Split split2;
  Node *node;
  BuildParam child;

  n = p->numTris;
  if (n < 4) {
    unsigned int first;
    Leaf *leaf;
    first = p->firstTri;
    leaf = (Leaf *)RwMalloc(sizeof(Leaf), 0);
    if (leaf) {
      leaf->numTris = (unsigned short)n;
      leaf->firstTri = (unsigned short)first;
      leaf->type = -1;
      return leaf;
    }
    return 0;
  }
  if (p->depth < 0x20) {
  tri = &p->tris[p->firstTri];
  for (i = 0; i < p->numVerts; i++) {
    p->verts[i].flags = 0;
  }
  for (i = 0; i < p->numTris; i++) {
    int j;
    for (j = 0; j < 3; j++) {
      p->verts[tri->idx[j]].flags++;
    }
    tri++;
  }
  numSorted = 0;
  for (i = 0; i < p->numVerts; i++) {
    if (p->verts[i].flags) {
      p->sorted[numSorted++] = &p->verts[i];
    }
  }
  p->numSorted = numSorted;
  bestScore = 3.40282347e+38f;
  bestAxis = 0;
  bestValue = 0.0f;
  size[0] = p->sup.x - p->inf.x;
  size[1] = p->sup.y - p->inf.y;
  size[2] = p->sup.z - p->inf.z;
  maxAxis = 0;
  for (axis = 0; axis < 12; axis += 4) {
    if (GETCOORD(size[0], axis) > GETCOORD(size[0], maxAxis)) {
      maxAxis = axis;
    }
  }
  minExtent = GETCOORD(size[0], maxAxis) * DAT_005cd03c;
  maxSize = &GETCOORD(size[0], maxAxis);
  for (axis = 0; axis < 12; axis += 4) {
    float weight;
    float scale;
    float medianKey;
    unsigned int lo;
    unsigned int mid;
    unsigned int hi;
    unsigned int j;
    int last;
    int tries;
    weight = FUN_0053e690(GETCOORD(size[0], axis), *maxSize);
    for (i = 0; i < p->numSorted; i++) {
      p->sorted[i]->key = GETCOORD(*p->sorted[i]->pos, axis);
    }
    FUN_005c24e0(p->sorted, p->numSorted, sizeof(CVert *), FUN_0053e6d0);
    mid = i >> 1;
    lo = i >> 2;
    medianKey = p->sorted[mid]->key;
    hi = lo + mid;
    scale = p->sorted[hi]->key - p->sorted[lo]->key;
    if (scale < minExtent) {
      scale = 0.0f;
      hi = mid;
      lo = mid;
    } else {
      scale = DAT_005cd120 / scale;
      for (j = lo; j <= hi; j++) {
        CVert *v;
        float d;
        v = p->sorted[j];
        d = medianKey - v->key;
        if (d < DAT_005d757c) {
          d = -d;
        }
        v->key = d;
      }
      FUN_005c24e0(&p->sorted[lo], hi - lo + 1, sizeof(CVert *), FUN_0053e6d0);
    }
    last = -1;
    tries = 50;
    for (j = lo; j <= hi && tries > 0; j++) {
      float value;
      int q;
      value = GETCOORD(*p->sorted[j]->pos, axis);
      RwInt32FromReal(q, value * scale);
      if (q != last) {
        unsigned int k;
        Split split;
        for (k = 0; k < p->numSorted; k++) {
          float d;
          d = GETCOORD(*p->sorted[k]->pos, axis) - value;
          p->sorted[k]->flags = 0;
          if (d <= DAT_005d757c) {
            p->sorted[k]->flags |= 1;
          }
          if (d >= DAT_005d757c) {
            p->sorted[k]->flags |= 2;
          }
        }
        FUN_0053e430(p, axis, value, &split);
        if (split.numLeft && split.numRight) {
          float score;
          score = FUN_0053e5d0(p, &split, axis) * weight;
          if (score < bestScore) {
            bestAxis = axis;
            bestScore = score;
            bestValue = value;
          }
        }
        tries--;
        last = q;
      }
    }
  }
  if (bestScore < DAT_005ceac0) {
    unsigned int k;
    for (k = 0; k < p->numSorted; k++) {
      float d;
      d = GETCOORD(*p->sorted[k]->pos, bestAxis) - bestValue;
      p->sorted[k]->flags = 0;
      if (d <= DAT_005d757c) {
        p->sorted[k]->flags |= 1;
      }
      if (d >= DAT_005d757c) {
        p->sorted[k]->flags |= 2;
      }
    }
    FUN_0053e430(p, bestAxis, bestValue, &split2);
    FUN_0053e6f0(p);
  } else {
    n = p->numTris;
    if (n < 0xf0) {
      return FUN_0053e730(n, p->firstTri);
    }
    split2.leftValue = p->sup.x;
    split2.rightValue = p->inf.x;
    split2.numLeft = n >> 1;
    split2.numRight = n - split2.numLeft;
    bestAxis = 0;
  }
  p->depth++;
  node = FUN_0053e760(bestAxis, split2.leftValue, split2.rightValue);
  if (!node) {
    return 0;
  }
  child = *p;
  child.numTris = split2.numLeft;
  GETCOORD(child.sup, bestAxis) = split2.leftValue;
  node->left = FUN_0053dea0(&child);
  if (!node->left) {
    RwFree(node);
    return 0;
  }
  child = *p;
  child.numTris = split2.numRight;
  child.firstTri = p->firstTri + split2.numLeft;
  GETCOORD(child.inf, bestAxis) = split2.rightValue;
  node->right = FUN_0053dea0(&child);
  if (!node->right) {
    FUN_0053de50(node);
    return 0;
  }
  return node;
  }
  if (n < 0xf0) {
    return FUN_0053e730(n, p->firstTri);
  }
  {
    Err e;
    e.pluginID = 0x11d;
    e.errorCode = FUN_004d7ff0(0);
    FUN_004d8480(&e);
  }
  return 0;
}
