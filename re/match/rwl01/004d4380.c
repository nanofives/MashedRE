// FLAGS: /O2 /Ob1 /G5
// TOOLCHAIN rw9044
/* RWC-MATCH-MOD40A 0x004d4380: typed rewrite, best 37.5% under rw9044 (was 17.2% raw Ghidra form, ddk). Checked against the original disassembly branch by branch (RESULT_RWC-MATCH-MOD40A.md). The topological-sort context passed to 0x004d4bf0 is a struct {pipeline, next = 0}, as the original builds it at 0x004d44f4 / 0x004d44f8 (esp+0x10 = ebx, esp+0x14 = edi = 0) and passes at 0x004d458e; the raw form wrote next to a separate local the compiler drops as dead. Residual: pipeline/zero/numNodes register rotation (original ebx/edi/esi), two error tails cross-jumped that the original keeps apart, scratch-size arithmetic order. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ae0, 0x00911ae0);
typedef struct Cnt4380 { int a; int b; int c; } Cnt4380;
typedef struct Ref4380 { uint id; int b; int c; } Ref4380;
typedef struct Node4380 Node4380;
typedef struct Pipe4380 Pipe4380;
typedef struct Def4380 {
  int f0; int f4;
  int (*init)(struct Def4380 *);                  /* +0x08 */
  void (*term)(struct Def4380 *);                 /* +0x0c */
  int (*nodeInit)(Node4380 *);                    /* +0x10 */
  void (*nodeTerm)(Node4380 *);                   /* +0x14 */
  int (*nodeConfig)(Node4380 *, Pipe4380 *);      /* +0x18 */
  int f1c;
  uint numRefs;                                   /* +0x20 */
  Ref4380 *refs;                                  /* +0x24 */
  int f28; int f2c; int f30;
  uint privSize;                                  /* +0x34 */
  int f38;
  int refCount;                                   /* +0x3c */
} Def4380;
struct Node4380 { Def4380 *nodeDef; uint numOutputs; uint *outputs; int pad[4]; Cnt4380 *topSort; int pad2[2]; };
struct Pipe4380 { int locked; uint numNodes; Node4380 *nodes; int pad[5]; char *mem; uint memSize; uint entry; };
typedef struct Sorter4380 { Pipe4380 *pipe; int next; } Sorter4380;
typedef struct Err4380 { int a; int b; } Err4380;
MGLOBAL(Cnt4380 *, DAT_007d4714, 0x007d4714);
MGLOBAL(uint *, DAT_007d4718, 0x007d4718);
int FUN_004d4910(Pipe4380 * p0, uint p1);
int FUN_004d4a20(Pipe4380 * p0, Pipe4380 * p1);
void FUN_004d4bf0(Sorter4380 * p0, uint p1);
int FUN_004d7ff0(int p0, ...);
void FUN_004d8480(Err4380 * p0);
int FUN_004e2090(Pipe4380 * p0);
#define ERR4380(code) { Err4380 err; err.a = 1; err.b = FUN_004d7ff0(code); FUN_004d8480(&err); }
#define MAXNODES4380 (*(uint *)(DAT_007d3ff8 + DAT_00911ae0 + 0x38))
// MATCH: 0x004d4380 FUN_004d4380
Pipe4380 * FUN_004d4380(Pipe4380 *pipeline)
{
  uint numNodes;
  uint numClusters;
  uint total;
  uint outTotal;
  uint i;
  int k;

  if (pipeline != 0) {
    if (pipeline->locked) {
    numNodes = pipeline->numNodes;
    if (numNodes != 0) {
    uint lockSize;
    uint size;
    uint maxNodes;
    uint id;
    outTotal = 0;
    if (pipeline->entry < numNodes && pipeline->nodes[pipeline->entry].nodeDef != 0) {
    /* count the distinct cluster ids referenced by the node definitions */
    numClusters = 0;
    id = 0;
    for (;;) {
      uint last = id;
      id = (uint)-1;
      if (numNodes > 0) {
        Node4380 *node = pipeline->nodes;
        uint n = numNodes;
        do {
          uint r = node->nodeDef->numRefs;
          if (r > 0) {
            Ref4380 *ref = node->nodeDef->refs;
            do {
              if (ref->id > last && ref->id < id) id = ref->id;
              ref++;
            } while (--r);
          }
          node++;
        } while (--n);
      }
      if (id == (uint)-1) break;
      numClusters++;
    }
    maxNodes = MAXNODES4380;
    lockSize = maxNodes * 0xb4;
    size = numNodes * sizeof(Node4380);
    if (numNodes > 0) size += pipeline->nodes->numOutputs * numNodes * sizeof(uint);
    size = maxNodes * 0xc + ((numClusters * 0x34 + 0x14) * numNodes + size);
    {
      uint extra = ((numNodes + 5) * numClusters + (numClusters + 1) * numNodes) * 4;
      if (numNodes > 0) {
        Node4380 *node = pipeline->nodes;
        uint n = numNodes;
        do {
          Def4380 *d = node->nodeDef;
          if (d->privSize) extra += d->privSize;
          node++;
          extra += d->numRefs * 4;
        } while (--n);
      }
      size = extra + numClusters * 0x1c + size + 0x14;
    }
    total = lockSize;
    if (size > lockSize) total = size;
    if (total > pipeline->memSize) {
      if (!FUN_004d4910(pipeline, total)) return 0;
    }
    {
    Sorter4380 sorter;
    DAT_007d4718 = 0;
    DAT_007d4714 = (Cnt4380 *)(pipeline->mem + total);
    sorter.pipe = pipeline;
    sorter.next = 0;
    {
    Node4380 *node = pipeline->nodes;
    for (i = 0; i < pipeline->numNodes; i++) {
      if (node->nodeDef != 0) {
        node->topSort->b = 0;
        node->topSort->a = 0;
      }
      node++;
    }
    }
    {
    Node4380 *node = pipeline->nodes;
    for (i = 0; i < pipeline->numNodes; i++) {
      if (node->nodeDef != 0 && node->numOutputs != 0) {
        uint *out = node->outputs;
        uint n = node->numOutputs;
        do {
          if (*out != (uint)-1) pipeline->nodes[*out].topSort->a++;
          out++;
        } while (--n);
      }
      node++;
    }
    }
    if (pipeline->nodes[pipeline->entry].topSort->a != 0) {
      ERR4380(0x24)
      return 0;
    }
    for (i = 0; i < pipeline->numNodes; i++) {
      if (i != pipeline->entry && pipeline->nodes[i].topSort->a == 0) {
        ERR4380(0x22)
        return 0;
      }
    }
    FUN_004d4bf0(&sorter, pipeline->entry);
    }
    for (i = 0; i < pipeline->numNodes; i++) {
      Cnt4380 *ts = pipeline->nodes[i].topSort;
      if (ts->a != ts->b) {
        ERR4380(0x1c)
        return 0;
      }
    }
    pipeline->entry = 0;
    {
      /* lock block = [maxNodes nodes][maxNodes x 32 outputs][maxNodes count blocks]:
         move the count blocks to the top of the scratch, then pack the outputs after the nodes */
      uint *srcOut;
      Cnt4380 *src;
      Cnt4380 *dst;
      uint *dstOut;
      maxNodes = MAXNODES4380;
      srcOut = (uint *)&pipeline->nodes[maxNodes];
      src = (Cnt4380 *)(srcOut + maxNodes * 32) + pipeline->numNodes - 1;
      dst = (Cnt4380 *)(pipeline->mem + total) - 1;
      for (k = (int)pipeline->numNodes - 1; k >= 0; k--) {
        *dst = *src;
        src--;
        pipeline->nodes[k].topSort = dst;
        dst--;
      }
      dstOut = (uint *)&pipeline->nodes[pipeline->numNodes];
      for (i = 0; i < pipeline->numNodes; i++) {
        uint n = pipeline->nodes[i].numOutputs;
        if (n == 0) {
          pipeline->nodes[i].outputs = 0;
        }
        else {
          uint *s = srcOut;
          uint *d = dstOut;
          for (; n != 0; n--) *d++ = *s++;
          pipeline->nodes[i].outputs = dstOut;
        }
        srcOut += 32;
        n = pipeline->nodes[i].numOutputs;
        outTotal += n;
        dstOut += n;
      }
      DAT_007d4714 = src;
      DAT_007d4718 = srcOut + outTotal;
    }
    if (FUN_004e2090(pipeline) != 0) return 0;
    if (!FUN_004d4910(pipeline, (char *)DAT_007d4718 - pipeline->mem)) goto fail;
    for (i = 0; i < pipeline->numNodes; i++) pipeline->nodes[i].topSort = 0;
    {
      uint done;
      uint j;
      for (k = (int)pipeline->numNodes - 1; k >= 0; k--) {
        Node4380 *node = &pipeline->nodes[k];
        Def4380 *d = node->nodeDef;
        if (d->refCount++ == 0 && d->init != 0 && !d->init(d)) {
          done = pipeline->numNodes - k - 1;
          goto undo;
        }
        if (d->nodeInit != 0 && !d->nodeInit(node)) {
          if (--d->refCount == 0 && d->term != 0) d->term(d);
          done = pipeline->numNodes - k - 1;
          goto undo;
        }
      }
      for (k = (int)pipeline->numNodes - 1; k >= 0; k--) {
        Node4380 *node = &pipeline->nodes[k];
        if (node->nodeDef->nodeConfig != 0 && !node->nodeDef->nodeConfig(node, pipeline)) {
          done = pipeline->numNodes;
          goto undo;
        }
      }
      goto ok;
undo:
      for (j = pipeline->numNodes - done; j < pipeline->numNodes; j++) {
        Node4380 *node = &pipeline->nodes[j];
        Def4380 *d = node->nodeDef;
        if (d->nodeTerm != 0) d->nodeTerm(node);
        if (--d->refCount == 0 && d->term != 0) d->term(d);
      }
fail:
      FUN_004d4a20(pipeline, pipeline);
      return 0;
    }
    }
    else {
      ERR4380(0x24)
      return 0;
    }
    }
ok:
    pipeline->locked = 0;
    return pipeline;
    }
    else ERR4380(0x34)
    return 0;
  }
  ERR4380(0x80000016)
  return 0;
}
