// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004d3e90 (h2): 82.8% (regs 86.6%). Raw 30.0%. Pipeline node destroy shape: unlocked path (node terms,
   def refcount, RwFree through RwGlobals+0x10c, memset intrinsic of the 40-byte node); locked path shifts the
   0x80-byte blocks, the 12-byte blocks (the original's base has no idx term: kept), the node structs (struct
   assignment, outputs -= 0x20, topSortData--), then fixes output indices. Moves: dst/src pointer pairs for the two
   block shifts (+16.6). Residual (b): the original pushes ebp at entry; ours pushes it inside the locked path; the
   index-fix loop picks eax/ecx the other way round. Tried 4 loop-body forms, permuter (no gain). */
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
typedef struct NodeDef NodeDef;
typedef struct Node Node;
typedef struct { uint a, b, c; } TopSort;
struct NodeDef {
  char *name; void *nodeBody; void *nodeInit; void (*nodeTerm)(NodeDef *); void *pipelineNodeInit;
  void (*pipelineNodeTerm)(Node *); void *pipelineNodeConfig; void *configMsgHandler;
  int io[5]; int privateDataSize; int editable; int InputPipesCnt;
};
struct Node {
  NodeDef *nodeDef; uint numOutputs; uint *outputs; void *slotClusterRefs; void *slotsContinue;
  void *privateData; void *inputToClusterSlot; TopSort *topSortData; void *initializationData;
  uint initializationDataSize;
};
typedef struct { int locked; uint numNodes; Node *nodes; } Pipeline;
typedef struct { uint d[32]; } Blk80;
typedef void (*freefn_t)(void *);
void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ae0, 0x00911ae0);
// MATCH: 0x004d3e90 FUN_004d3e90
Node *FUN_004d3e90(Node *param_1,Pipeline *param_2)
{
  uint idx;
  uint i;
  uint j;
  Blk80 *blk;
  Blk80 *nblk;
  TopSort *ts;
  TopSort *nts;

  if (!param_2->locked) {
    if (param_1->nodeDef->pipelineNodeTerm) {
      param_1->nodeDef->pipelineNodeTerm(param_1);
    }
    param_1->nodeDef->InputPipesCnt--;
    if (param_1->nodeDef->InputPipesCnt == 0) {
      if (param_1->nodeDef->nodeTerm) {
        param_1->nodeDef->nodeTerm(param_1->nodeDef);
      }
      if (param_1->nodeDef->editable) {
        (*(freefn_t *)(DAT_007d3ff8 + 0x10c))(param_1->nodeDef);
        param_1->nodeDef = 0;
      }
    }
    if (param_1->initializationData) {
      (*(freefn_t *)(DAT_007d3ff8 + 0x10c))(param_1->initializationData);
      param_1->initializationData = 0;
      param_1->initializationDataSize = 0;
    }
    memset(param_1, 0, sizeof(Node));
    param_2->numNodes--;
    return param_1;
  }
  if (param_1->initializationData) {
    (*(freefn_t *)(DAT_007d3ff8 + 0x10c))(param_1->initializationData);
    param_1->initializationData = 0;
    param_1->initializationDataSize = 0;
  }
  if (param_1->nodeDef->InputPipesCnt == 0 && param_1->nodeDef->editable) {
    (*(freefn_t *)(DAT_007d3ff8 + 0x10c))(param_1->nodeDef);
    param_1->nodeDef = 0;
  }
  idx = param_1 - param_2->nodes;
  if (idx < param_2->numNodes - 1) {
    blk = (Blk80 *)(param_2->nodes + *(int *)(DAT_007d3ff8 + DAT_00911ae0 + 0x38)) + idx;
    nblk = blk + 1;
    for (i = idx; i < param_2->numNodes - 1; i++) {
      *blk = *nblk;
      blk = nblk;
      nblk++;
    }
    ts = (TopSort *)((char *)param_2->nodes + *(int *)(DAT_007d3ff8 + DAT_00911ae0 + 0x38) * 0xa8);
    nts = ts + 1;
    for (i = idx; i < param_2->numNodes - 1; i++) {
      *ts = *nts;
      ts = nts;
      nts++;
    }
    for (i = idx; i < param_2->numNodes - 1; i++) {
      param_2->nodes[i] = param_2->nodes[i + 1];
      param_2->nodes[i].outputs -= 0x20;
      param_2->nodes[i].topSortData--;
    }
    for (i = 0; i < param_2->numNodes - 1; i++) {
      for (j = 0; j < param_2->nodes[i].numOutputs; j++) {
        uint *o = &param_2->nodes[i].outputs[j];
        if (*o >= idx) {
          if (*o == idx) *o = (uint)-1;
          else *o = *o - 1;
        }
      }
    }
  }
  param_2->numNodes--;
  return param_1;
}
