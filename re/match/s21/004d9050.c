// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d9050 (h3): 100% (regs 100%). Raw 31.8%. The file holds 0x004d94a0 (the octree descend) as a recursive
   __forceinline function: the original inlines exactly one level of it into both pixel loops and calls
   0x004d94a0 for the rest, which a recursive body inlined once reproduces (plain __inline / static are not
   inlined, 23-68%). 0x004d94a0's own body from this form scores 59.6%, same as h4's loop form, so it carries no MATCH line here. Other moves:
   key = (((lut[r>>s] << 1) | lut[g>>s]) << 1 | ...) with '<< 1' (the '* 2' form 82.8%), 4/8-bit and 32-bit
   loops written alike. */
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
typedef struct Node9050 { int v[6]; unsigned char flag; char pad[3]; struct Node9050 *child[16]; } Node9050;
typedef struct Tree9050 { char pad[0x4000]; Node9050 *root; void *freeList; } Tree9050;
typedef struct Img9050 { int f0; int width; int height; int depth; int stride; unsigned char *pixels; unsigned char *palette; } Img9050;
typedef void *(*Alloc9050)(void *freeList, uint hint);
MGLOBAL(int, DAT_00618430, 0x00618430);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(uint, DAT_007d6c98, 256, 0x007d6c98);
void FUN_004d9360(Node9050 *acc, unsigned char *col, float weight);
/* 0x004d94a0 FUN_004d94a0: real callee, held here so it inlines; scored in h4's 004d94a0.c (no MATCH line here) */
__forceinline Node9050 *FUN_004d94a0(Tree9050 *param_1, Node9050 *node, uint key, int depth)
{
  Node9050 **slot;
  if (depth == 0) {
    return node;
  }
  slot = &node->child[key & 0xf];
  if (*slot == 0) {
    Node9050 *n = (Node9050 *)(*(Alloc9050 *)(DAT_007d3ff8 + 0x118))(param_1->freeList, 0x30411);
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
  return FUN_004d94a0(param_1, *slot, key >> 4, depth - 1);
}
// MATCH: 0x004d9050 FUN_004d9050
void FUN_004d9050(Tree9050 *param_1, Img9050 *param_2, float param_3)
{
  int stride = param_2->stride;
  unsigned char *pal = param_2->palette;
  unsigned char *row = param_2->pixels;
  int depth = param_2->depth;
  int y = param_2->height;
  int x;
  unsigned char *p;
  unsigned char *col;
  int sh;
  uint key;
  if (depth == 4 || depth == 8) {
    for (; y != 0; y--) {
      p = row;
      for (x = param_2->width; x != 0; x--) {
        col = pal + *p * 4;
        sh = 8 - DAT_00618430;
        key = (((DAT_007d6c98[col[0] >> sh] << 1) | DAT_007d6c98[col[1] >> sh]) << 1 | DAT_007d6c98[col[2] >> sh]) << 1 | DAT_007d6c98[col[3] >> sh];
        FUN_004d9360(FUN_004d94a0(param_1, param_1->root, key, DAT_00618430), col, param_3);
        p++;
      }
      row += stride;
    }
  } else if (depth == 32) {
    for (; y != 0; y--) {
      p = row;
      for (x = param_2->width; x != 0; x--) {
        sh = 8 - DAT_00618430;
        key = (((DAT_007d6c98[p[0] >> sh] << 1) | DAT_007d6c98[p[1] >> sh]) << 1 | DAT_007d6c98[p[2] >> sh]) << 1 | DAT_007d6c98[p[3] >> sh];
        FUN_004d9360(FUN_004d94a0(param_1, param_1->root, key, DAT_00618430), p, param_3);
        p += 4;
      }
      row += stride;
    }
  }
}
