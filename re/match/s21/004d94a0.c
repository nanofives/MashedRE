// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d94a0 (h4): 59.6% (regs 78.8%, ops 84.6%). Struct form of the raw body (16-way node, child[16] at +0x1c, alloc through RwGlobals+0x118 with param_1+0x4004 and 0x30411, rep stosd of the children, 6 dwords plus a byte zeroed at depth 1). Residual is register allocation. The original keeps node and slot in esi (lea esi,[esi+..]; cmp [esi],0; mov esi,[esi]), copies key through ecx before shr, reloads param_1 and RwGlobals into ecx/eax inside the alloc branch, and returns param_2 from the stack on depth==0. Ours keeps node in eax. Tried for/while/do-while with an early return, a separate cur local, key>>=4 hoisted, permuter 140 compiles 0 gains. Class (b). */
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
typedef unsigned int uint;
typedef struct Node { int v[6]; unsigned char flag; char pad[3]; struct Node *child[16]; } Node;
typedef void *(*AllocFn)(void *freeList, uint hint);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x004d94a0 FUN_004d94a0
Node *FUN_004d94a0(char *param_1, Node *node, uint key, int depth)
{
  while (depth) {
    Node **slot = &node->child[key & 0xf];
    if (*slot == 0) {
      Node *n = (Node *)(*(AllocFn *)(DAT_007d3ff8 + 0x118))(*(void **)(param_1 + 0x4004), 0x30411);
      int i;
      *slot = n;
      for (i = 0; i < 16; i++) n->child[i] = 0;
      if (depth == 1) {
        n->flag = 0;
        n->v[0] = 0;
        n->v[1] = 0;
        n->v[2] = 0;
        n->v[3] = 0;
        n->v[4] = 0;
        n->v[5] = 0;
      }
    }
    node = *slot;
    depth--;
    key >>= 4;
  }
  return node;
}
