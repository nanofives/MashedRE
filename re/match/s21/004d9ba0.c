// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d9ba0 (h3): best 75.4% (regs 75.4%, ops 96.7%). Raw 25.4%. 16-way recursive walk: node box test
   (pos + (1 << depth) - q.min > 0, pos - q.max < 0, four axes), leaf stores the byte at +0x18, children at +0x1c.
   Copying pos[0..3] to locals before computing size gives the original's op order. Residual is stack layout
   only: frame 0x24 against ours 0x18 (the original keeps e1/e2 spill slots apart from the child-position array
   and spills pos[2] into param_2's home; ours overlaps them). Tried scalar/array/struct forms of the extents,
   one 8-int box local, block scopes for both arrays, declaration order, permuter 240 s (only an invalid gain).
   Class (b): stack-slot assignment from a source form not found. */
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
typedef struct Node9ba0 { char pad[0x18]; unsigned char val; char pad2[3]; struct Node9ba0 *child[16]; } Node9ba0;
void FUN_004d9ba0(Node9ba0 *param_1, int *param_2, int param_3, int *param_4, int param_5);
// MATCH: 0x004d9ba0 FUN_004d9ba0
void FUN_004d9ba0(Node9ba0 *param_1, int *param_2, int param_3, int *param_4, int param_5)
{
  int size;
  int x, y, z, w;
  int e[4];
  int i;
  int c[4];
  if (param_1 != 0) {
    x = param_2[0]; y = param_2[1]; z = param_2[2]; w = param_2[3];
    size = 1 << param_3;
    e[0] = x + size;
    e[1] = y + size;
    e[2] = z + size;
    e[3] = w + size;
    if (e[0] - param_4[0] > 0 && e[1] - param_4[1] > 0 && e[2] - param_4[2] > 0 && e[3] - param_4[3] > 0 &&
        x - param_4[4] < 0 && y - param_4[5] < 0 && z - param_4[6] < 0 &&
        w - param_4[7] < 0) {
      if (param_3 == 0) {
        param_1->val = (unsigned char)param_5;
        return;
      }
      param_3--;
      for (i = 0; i < 16; i++) {
        c[0] = (((i >> 3) & 1) << param_3) + param_2[0];
        c[1] = (((i >> 2) & 1) << param_3) + param_2[1];
        c[2] = (((i >> 1) & 1) << param_3) + param_2[2];
        c[3] = ((i & 1) << param_3) + param_2[3];
        FUN_004d9ba0(param_1->child[i], c, param_3, param_4, param_5);
      }
    }
  }
}
