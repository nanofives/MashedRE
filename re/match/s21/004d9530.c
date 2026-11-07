// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d9530 (h3): best 25.8% (regs 40.7%, ops 90.3%, 399/403 ins). Raw 16.5% (514 ins once restructured).
   Median-cut palette build over the 16-way tree. State struct: box[256] {min[4], max[4]} at +0, score[256] at
   +0x2000, stat[256] (0x1c each, err float at +0x14, flag byte at +0x18) at +0x2400, root at +0x4000. The file
   holds 0x004da430 and 0x004da480 as recursive __forceinline copies (the original inlines one level of each:
   the 16-child loops at 0x004d95b4 and 0x004d9612), no MATCH lines here (scored in 004da430.c / 004da480.c),
   and a file-local static __inline s_Stats9530 (stat clear + zero origin + FUN_004d9d00) used three times:
   that took 448 -> 399 ins and ops 79 -> 90. do/while for the score search and axis loops stops 8x/4x
   unrolling. Residual: frame 0x74 vs 0x68, the state pointer in ebp (original esi), and the first inlined
   loop rereads DAT_00618430 per child where the original keeps depth-1 in a stack slot and the counter in
   ebp. Tried param-copying inline bodies, declaration order, permuter 300 s (1928 compiles, only
   semantically wrong gains). Class (b). */
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
typedef struct RGBA9530 { unsigned char red, green, blue, alpha; } RGBA9530;
typedef struct Node9530 { char pad[0x18]; unsigned char val; char pad2[3]; struct Node9530 *child[16]; } Node9530;
typedef struct Box9530 { int min[4]; int max[4]; } Box9530;
typedef struct Stat9530 { int v[5]; float err; unsigned char flag; char pad[3]; } Stat9530;
typedef struct Q9530 { Box9530 box[256]; float score[256]; Stat9530 stat[256]; Node9530 *root; } Q9530;
MGLOBAL(int, DAT_00618430, 0x00618430);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
void FUN_004da270(Node9530 *root, RGBA9530 *col, int bits);
void FUN_004d9d00(Node9530 *root, int *origin, int bits, Box9530 *box, Stat9530 *st);
float FUN_004da000(Node9530 *root, Box9530 *box, int axis, unsigned char *cut, Stat9530 *st);
void FUN_004d9ba0(Node9530 *root, int *origin, int bits, Box9530 *box, int val);
void FUN_004d9a60(RGBA9530 *out, void *src);
/* 0x004da430 FUN_004da430 and 0x004da480 FUN_004da480: real callees, held here as recursive __forceinline so
   one level inlines; scored in their own files (no MATCH lines here) */
__forceinline int FUN_004da430(Node9530 *param_1, int param_2)
{
  int count = 0;
  int i;
  if (param_1 != 0) {
    if (param_2 > 0) {
      for (i = 0; i < 16; i++) {
        count += FUN_004da430(param_1->child[i], param_2 - 1);
      }
    } else {
      return 1;
    }
  }
  return count;
}
__forceinline int FUN_004da480(Node9530 *n, RGBA9530 *pal, int idx, int depth)
{
  int i;
  if (n) {
    if (depth > 0) {
      for (i = 0; i < 16; i++) idx = FUN_004da480(n->child[i], pal, idx, depth - 1);
    } else {
      FUN_004d9a60(&pal[idx], n);
      n->val = (unsigned char)idx;
      idx++;
    }
  }
  return idx;
}
/* file-local helper (no address of its own: inlined at its three call sites in 0x004d9530) */
static __inline void s_Stats9530(Node9530 *root, Box9530 *box, Stat9530 *st)
{
  int origin[4];
  st->flag = 0;
  st->v[0] = 0;
  st->v[1] = 0;
  st->v[2] = 0;
  st->v[3] = 0;
  st->v[4] = 0;
  st->err = 0;
  origin[0] = 0;
  origin[1] = 0;
  origin[2] = 0;
  origin[3] = 0;
  FUN_004d9d00(root, origin, DAT_00618430, box, st);
}
// MATCH: 0x004d9530 FUN_004d9530
int FUN_004d9530(RGBA9530 *param_1, int param_2, Q9530 *param_3)
{
  RGBA9530 col;
  int i;
  int count;
  int nb;
  int best;
  int axis;
  float maxv;
  float gain[4];
  unsigned char cut[16];
  int o2[4];
  col.red = 0;
  col.green = 0;
  col.blue = 0;
  col.alpha = 0;
  FUN_004da270(param_3->root, &col, DAT_00618430);
  for (i = 0; i < param_2; i++) {
    param_1[i].red = 0;
    param_1[i].green = 0;
    param_1[i].blue = 0;
    param_1[i].alpha = 0;
  }
  count = FUN_004da430(param_3->root, DAT_00618430);
  if (count <= param_2) {
    FUN_004da480(param_3->root, param_1, 0, DAT_00618430);
    return count;
  }
  param_3->box[0].min[0] = 0;
  param_3->box[0].min[1] = 0;
  param_3->box[0].min[2] = 0;
  param_3->box[0].min[3] = 0;
  param_3->box[0].max[0] = 1 << DAT_00618430;
  param_3->box[0].max[1] = 1 << DAT_00618430;
  param_3->box[0].max[2] = 1 << DAT_00618430;
  param_3->box[0].max[3] = 1 << DAT_00618430;
  s_Stats9530(param_3->root, &param_3->box[0], &param_3->stat[0]);
  param_3->score[0] = param_3->stat[0].err;
  for (nb = 1; nb < param_2; nb++) {
    best = -1;
    maxv = DAT_005d757c;
    i = 0;
    if (nb > 0) {
      do {
        if (maxv < param_3->score[i]) {
          maxv = param_3->score[i];
          best = i;
        }
        i++;
      } while (i < nb);
    }
    if (best == -1) break;
    gain[0] = FUN_004da000(param_3->root, &param_3->box[best], 0, cut, &param_3->stat[best]);
    gain[1] = FUN_004da000(param_3->root, &param_3->box[best], 1, cut, &param_3->stat[best]);
    gain[2] = FUN_004da000(param_3->root, &param_3->box[best], 2, cut, &param_3->stat[best]);
    gain[3] = FUN_004da000(param_3->root, &param_3->box[best], 3, cut, &param_3->stat[best]);
    axis = 0;
    i = 0;
    do {
      if (gain[i] < gain[axis]) {
        axis = i;
      }
      i++;
    } while (i < 4);
    if (gain[axis] < param_3->stat[best].err) {
      param_3->box[nb] = param_3->box[best];
      param_3->box[nb].min[axis] = cut[axis * 4];
      param_3->box[best].max[axis] = cut[axis * 4];
      s_Stats9530(param_3->root, &param_3->box[best], &param_3->stat[best]);
      s_Stats9530(param_3->root, &param_3->box[nb], &param_3->stat[nb]);
      param_3->score[best] = param_3->stat[best].err;
      param_3->score[nb] = param_3->stat[nb].err;
    } else {
      param_3->score[best] = 0;
      nb--;
    }
  }
  for (i = 0; i < param_2; i++) {
    o2[0] = 0;
    o2[1] = 0;
    o2[2] = 0;
    o2[3] = 0;
    FUN_004d9ba0(param_3->root, o2, DAT_00618430, &param_3->box[i], i);
    FUN_004d9a60(&param_1[i], &param_3->stat[i]);
  }
  return param_2;
}
