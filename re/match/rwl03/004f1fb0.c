// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWC-NEAR-D (2026-11-12): 100%, BYTE-IDENTICAL under rw9044 (VC6 SP5 cl 12.00.8804 + PP c2 13.00.9044, L-131).
   This file had no TOOLCHAIN line, so earlier scores ran under the DDK default. Three edits from 83.1%:
   (1) the strip list "strips" is a function-scope variable (address-taken, so it pins the bmesh parameter slot for the
   whole function: the run-split counter then gets its own local and the frame becomes 0x1c, 83.1 -> 93.8 ddk / 95.5 rw9044);
   (2) the per-run loop is counted by "i" (for i < numRuns, reversed by the compiler to a count-down), which turns the
   loop-entry test into jbe; (3) the mesh allocation size goes through the function-scope "size" that the header pass
   also uses (fixes the eax/edx swap at 0x004f21f8 and the header slots: idx in the param_2 slot, the bound in param_3,
   the result in the local at frame +0x10). Under ddk the same source scores 91.9%. */
/* MATCH-RWC-NEAR-B (2026-11-12): rewritten from the raw g2s body as typed C (mesh/run/strip structs, the RW memory
   macros through RwGlobals 0x007d3ff8 +0x108/+0x10c/+0x11c, the mesh-module globals at DAT_00912118). 37.0% -> 83.1% ddk and rw9044,
   355/355 instructions (81.7% by hand, then match_permute.py 1964 compiles: 83.4% with the three RwFree calls reordered;
   the original order sorted/meshes/runs is kept, 83.1%). Levers: run table written by index runs[numRuns] (the original strength-reduces from &runs[0].num),
   both n >= 2 guards spelled >= 2 (cmp 2 / jb). Residual: stack-slot assignment (original frame 0x1c, this 0x18) and
   loop pointer offsets in the header copy loop. Not 100%, not promoted. */
/* MATCH-RWL03 0x004f1fb0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(int, DAT_00912118, 0x00912118);
typedef void *(*MallocFn)(unsigned int, unsigned int);
typedef void (*FreeFn)(void *);
typedef void (*FLFreeFn)(void *, void *);
#define RwMalloc(s,h) ((*(MallocFn *)(DAT_007d3ff8 + 0x108))((s),(h)))
#define RwFree(p) ((*(FreeFn *)(DAT_007d3ff8 + 0x10c))((p)))
#define RwFreeListFree(fl,p) ((*(FLFreeFn *)(DAT_007d3ff8 + 0x11c))((fl),(p)))
typedef struct { unsigned short nextSerialNum; unsigned short pad; void *freeList; } MeshGlobals;
#define MESHGLOBAL(v) (((MeshGlobals *)((char *)DAT_007d3ff8 + DAT_00912118))->v)
typedef struct { int v0; int v1; int mat; int v3; int v4; } BTri;
typedef struct { int unk; unsigned int numTriangles; BTri *triangles; } BMesh;
typedef struct { int unk; unsigned int num; int mat; } BRun;
typedef struct Strip { unsigned short *indices; unsigned int num; int unk; struct Strip *next; } Strip;
typedef struct { unsigned short *indices; unsigned int numIndices; int material; } Mesh;
typedef struct { unsigned int flags; unsigned short numMeshes; unsigned short serialNum; unsigned int totalIndicesInMesh; unsigned int firstMeshOffset; } MeshHeader;
void *FUN_004cc7f0(int p0, int p1, int p2, int p3);
int FUN_004cc9f0(void *p0);
void *FUN_004f0970(unsigned int p0);
int FUN_004f2420(const void *a, const void *b);
int FUN_004f2570(BTri *p0, unsigned int p1, Strip **p2, void *p3);
int FUN_004f3440(Strip **p0, unsigned int p1);
void FUN_005c24e0(void *p0, unsigned int p1, unsigned int p2, int (*p3)(const void *, const void *));
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
// MATCH: 0x004f1fb0 FUN_004f1fb0
MeshHeader *FUN_004f1fb0(BMesh *bmesh, void *param_2, unsigned int param_3)
{
  unsigned int n;
  BTri **sorted;
  BRun *runs;
  MeshHeader *result;
  Mesh **meshes;
  BRun *run;
  unsigned int i;
  unsigned short numMeshes;
  unsigned int numRuns;
  int cur;
  unsigned int size;
  Strip *strips;

  numMeshes = 0;
  sorted = (BTri **)RwMalloc(bmesh->numTriangles * sizeof(BTri *), 0x10502);
  result = 0;
  if (sorted) {
    for (i = 0; i < bmesh->numTriangles; i++) {
      sorted[i] = &bmesh->triangles[i];
    }
    FUN_005c24e0(sorted, bmesh->numTriangles, sizeof(BTri *), FUN_004f2420);
    n = bmesh->numTriangles;
    numRuns = 1;
    if (n >= 2) {
      cur = sorted[0]->mat;
      i = 1;
      while (i < n) {
        if (sorted[i]->mat != cur) {
          cur = sorted[i]->mat;
          numRuns++;
        }
                i++;
      }
    }
    meshes = (Mesh **)RwMalloc(numRuns * sizeof(Mesh *), 0x1010502);
    runs = (BRun *)RwMalloc(numRuns * sizeof(BRun), 0x10502);
    runs[0].mat = sorted[0]->mat;
    runs[0].unk = 0;
    numRuns = 1;
    runs[0].num = 0;
    if (bmesh->numTriangles >= 2) {
      for (i = 0; i < bmesh->numTriangles - 1; i++) {
        if (sorted[i]->mat != sorted[i + 1]->mat) {
          runs[numRuns].mat = sorted[i + 1]->mat;
          runs[numRuns].num = i + 1;
          runs[numRuns - 1].num = i - runs[numRuns - 1].num + 1;
          runs[numRuns].unk = 0;
          numRuns += 1;
        }
      }
    }
    runs[numRuns - 1].num = bmesh->numTriangles - runs[numRuns - 1].num;
    MESHGLOBAL(freeList) = FUN_004cc7f0(sizeof(Strip), bmesh->numTriangles / 10 + 5, 4, 0x10502);
    {
      BTri **sp;
      Strip *s;
      strips = 0;
      sp = sorted;
      run = runs;
      for (i = 0; i < numRuns; i++) {
        BTri *tris;
        unsigned int j;
        tris = (BTri *)RwMalloc(run->num * sizeof(BTri), 0x10502);
        for (j = 0; j < run->num; j++) {
          tris[j] = **sp;
          sp++;
        }
        FUN_004f2570(tris, run->num, &strips, param_2);
        FUN_004f3440(&strips, param_3);
        for (s = strips; s; s = s->next) {
          Mesh *m;
          size = sizeof(Mesh) + s->num * sizeof(unsigned short);
          m = (Mesh *)RwMalloc(size, 0x10502);
          m->material = run->mat;
          m->numIndices = s->num;
          m->indices = (unsigned short *)(1 + m);
          for (j = 0; j < m->numIndices; j++) {
            m->indices[j] = s->indices[j];
          }
          meshes[numMeshes++] = m;
        }
        while (strips) {
          s = strips;
          strips = s->next;
          RwFree(s->indices);
          s->indices = 0;
          RwFreeListFree(MESHGLOBAL(freeList), s);
        }
        RwFree(tris);
        run++;
      }
    }
    FUN_004cc9f0(MESHGLOBAL(freeList));
    MESHGLOBAL(freeList) = 0;
    {
      int total;
      Mesh *dst;
      unsigned short *idx;
      total = 0;
      size = sizeof(MeshHeader);
      for (i = 0; i < numMeshes; i++) {
        total += meshes[i]->numIndices;
        size += sizeof(Mesh) + meshes[i]->numIndices * sizeof(unsigned short);
      }
      result = (MeshHeader *)FUN_004f0970(size);
      result->flags = 1;
      result->numMeshes = numMeshes;
      result->serialNum = MESHGLOBAL(nextSerialNum);
      result->firstMeshOffset = 0;
      result->totalIndicesInMesh = total;
      MESHGLOBAL(nextSerialNum)++;
      dst = (Mesh *)(result + 1);
      idx = (unsigned short *)(dst + numMeshes);
      for (i = 0; i < numMeshes; i++) {
        dst->indices = idx;
        dst->numIndices = meshes[i]->numIndices;
        dst->material = meshes[i]->material;
        memcpy(idx, meshes[i]->indices, meshes[i]->numIndices * sizeof(unsigned short));
        idx += dst->numIndices;
        dst += 1;
        RwFree(meshes[i]);
        meshes[i] = 0;
      }
    }
    RwFree(sorted);
    RwFree(meshes);
    RwFree(runs);
  }
  return result;
}
