// FLAGS: /O2 /Ob1 /G5
// TOOLCHAIN rw9044
/* RWC-MATCH-MOD40A 0x004d4f90: typed rewrite, best 47.8% under rw9044 (was 28.3% raw, ddk). Vararg node-list add: count the NULL-terminated list, create each node at nodes[oldNum + i] (0x004d5190), link it to the previous one through the inlined output / node / index checks, unwind with 0x004d3e90. Residual: the original keeps pipeline in ebp and spills the offset IV; this source gets the offset IV in ebp and pipeline in memory (stable under every variant tried, and flags). */
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
/* x86 stack varargs, written out so the source needs no system header under either compiler */
typedef char *Va4f90;
#define VA4F90_START(ap, v) ((ap) = (Va4f90)&(v) + sizeof(v))
#define VA4F90_ARG(ap, t) (*(t *)(((ap) += sizeof(t)) - sizeof(t)))
typedef struct Node4f90 { void *nodeDef; uint numOutputs; uint *outputs; int pad[7]; } Node4f90;
typedef struct Pipe4f90 { int locked; uint numNodes; Node4f90 *nodes; } Pipe4f90;
typedef struct Err4f90 { int a; int b; } Err4f90;
void FUN_004d3e90(Node4f90 * p0, Pipe4f90 * p1);
int FUN_004d5190(Pipe4f90 * p0, Node4f90 * p1, void *p2);
int FUN_004d7ff0(int p0, ...);
void FUN_004d8480(Err4f90 * p0);

static __inline uint *Out4f90(Node4f90 *node, uint n)
{
  if (node->nodeDef != 0 && n < node->numOutputs) return &node->outputs[n];
  return 0;
}

static __inline Node4f90 *Chk4f90(Node4f90 *node)
{
  if (node != 0 && node->nodeDef != 0) return node;
  return 0;
}

static __inline Pipe4f90 *Path4f90(Pipe4f90 *p, uint *out, Node4f90 *dest)
{
  if (p->locked && out != 0 && *out == (uint)-1 && dest != 0 && dest->nodeDef != 0) {
    uint idx = dest - p->nodes;
    if (&p->nodes[idx] == dest && idx < p->numNodes && idx != (uint)-1) {
      *out = idx;
      return p;
    }
  }
  return 0;
}

// MATCH: 0x004d4f90 FUN_004d4f90
Pipe4f90 * FUN_004d4f90(Pipe4f90 *pipeline, uint *firstIndex, void *nodeDef0, ...)
{
  if (pipeline != 0) {
    if (pipeline->locked) {
      uint numNew = 0;
      if (nodeDef0 != 0) {
        Va4f90 va;
        void *nd;
        VA4F90_START(va, nodeDef0);
        nd = nodeDef0;
        do {
          numNew++;
          nd = VA4F90_ARG(va, void *);
        } while (nd != 0);
      }
      if (numNew > 0) {
        uint oldNum = pipeline->numNodes;
        if (oldNum + numNew > *(uint *)(DAT_007d3ff8 + DAT_00911ae0 + 0x38)) {
          Err4f90 err;
          err.a = 1;
          err.b = FUN_004d7ff0(0x2a);
          FUN_004d8480(&err);
        }
        else {
          Va4f90 va;
          void *nd;
          uint i = 0;
          Node4f90 *last = 0;
          VA4F90_START(va, nodeDef0);
          nd = nodeDef0;
          do {
            Node4f90 *node = &pipeline->nodes[oldNum + i];
            if (!FUN_004d5190(pipeline, node, nd)) break;
            if (last != 0) {
              if (!Path4f90(pipeline, Out4f90(last, 0), Chk4f90(node))) {
                FUN_004d3e90(node, pipeline);
                break;
              }
            }
            i++;
            last = node;
            nd = VA4F90_ARG(va, void *);
          } while (nd != 0);
          if (i == numNew) {
            if (firstIndex != 0) *firstIndex = oldNum;
            return pipeline;
          }
          while (i > 0) {
            i--;
            FUN_004d3e90(&pipeline->nodes[oldNum + i], pipeline);
          }
          return 0;
        }
      }
    }
    else {
      Err4f90 err;
      err.a = 1;
      err.b = FUN_004d7ff0(0x34);
      FUN_004d8480(&err);
    }
    return 0;
  }
  {
    Err4f90 err;
    err.a = 1;
    err.b = FUN_004d7ff0(0x80000016);
    FUN_004d8480(&err);
  }
  return 0;
}
