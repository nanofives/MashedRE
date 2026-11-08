// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004daaf0 (h2): 100% (regs 100%). Frees the 16-way tree rooted at +0x4000: when the root is set,
   FUN_004dab70(obj, child[i], DAT_00618430 - 1) for the 16 children (only when DAT_00618430 > 0), then the
   RwGlobals free slot +0x11c (pool +0x4004, root); clears +0x4000 and tail-calls FUN_004cc9f0(+0x4004).
   What moved it: FUN_004cc9f0 takes the +0x4004 pointer (the original tail-jumps with it in the argument slot;
   Ghidra showed no argument) and DAT_00618430 read once into a local (raw form 46.2%). */
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
typedef struct Node { char pad[0x18]; unsigned char val; char pad2[3]; struct Node *child[16]; } Node;
typedef struct Pal { char pad[0x4000]; Node *root; void *freelist; } Pal;
typedef void (*RwFreeFn)(void *, void *);
MGLOBAL(int, DAT_00618430, 0x00618430);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_004cc9f0(void *p);
void FUN_004dab70(Pal *pal, Node *n, int depth);
// MATCH: 0x004daaf0 FUN_004daaf0
int FUN_004daaf0(Pal *pal)
{
  Node *root = pal->root;
  if (root) {
    int depth = DAT_00618430;
    if (depth > 0) {
      int i;
      for (i = 0; i < 16; i++) FUN_004dab70(pal, root->child[i], depth - 1);
    }
    (*(RwFreeFn *)(DAT_007d3ff8 + 0x11c))(pal->freelist, root);
  }
  pal->root = 0;
  return FUN_004cc9f0(pal->freelist);
}
