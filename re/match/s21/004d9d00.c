// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d9d00 (h1, by hand): best 73.5% exact, regs 72.9% (raw 24.7%). Recursive 16-way (4-D) tree walk:
   node box = pos .. pos + (1 << level); returns unless it overlaps the query box param_4[0..7]; a node fully
   inside calls 0x004d9ee0(param_5, param_5, node); else recurses into the 16 children at node+0x1c with level-1.
   Moves: whole body inside `if (node)` (pushes before the je), pos copied to locals with w first, nested ifs
   (24.7 -> 60.8), max[2] store order from the permuter (73.5). Residual: stack-slot assignment. The original
   spills w, box[4] and box[5] into the dead argument homes of param_2/param_3/param_4 (frame 0x28) and 9178 gives
   them fresh slots (frame 0x24), which shifts every [esp+k] operand. Tried: early-return form, register copies of
   the three pointer/level arguments (35.5), permuter (304 compiles). Class (a). C++ ok. */
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
typedef struct FUN_004d9d00_node FUN_004d9d00_node;
struct FUN_004d9d00_node {
  int pad[7];
  FUN_004d9d00_node *child[16];
};
void FUN_004d9ee0(int p0, int p1, FUN_004d9d00_node *node);
// MATCH: 0x004d9d00 FUN_004d9d00
void FUN_004d9d00(FUN_004d9d00_node *param_1,int *param_2,int param_3,int *param_4,int param_5)

{
  int x, y, z, w;
  int max[4];
  int sub[4];
  int size;
  int i;

  if (param_1 != 0) {
    w = param_2[3];
    x = param_2[0];
    y = param_2[1];
    z = param_2[2];
    size = 1 << param_3;
    max[0] = size + x;
    max[1] = size + y;
    max[3] = size + w;
    max[2] = size + z;
    if (max[0] - param_4[0] > 0 && max[1] - param_4[1] > 0 && max[2] - param_4[2] > 0 &&
        max[3] - param_4[3] > 0 && x - param_4[4] < 0 && y - param_4[5] < 0 &&
        z - param_4[6] < 0 && w - param_4[7] < 0) {
      if (x - param_4[0] >= 0 && y - param_4[1] >= 0 && z - param_4[2] >= 0 &&
          w - param_4[3] >= 0 && param_4[4] - max[0] >= 0 && param_4[5] - max[1] >= 0 &&
          param_4[6] - max[2] >= 0 && param_4[7] - max[3] >= 0) {
        FUN_004d9ee0(param_5, param_5, param_1);
        return;
      }
      if (param_3 > 0) {
        param_3--;
        for (i = 0; i < 16; i++) {
          sub[0] = (((i >> 3) & 1) << param_3) + param_2[0];
          sub[1] = (((i >> 2) & 1) << param_3) + param_2[1];
          sub[2] = (((i >> 1) & 1) << param_3) + param_2[2];
          sub[3] = ((i & 1) << param_3) + param_2[3];
          FUN_004d9d00(param_1->child[i], sub, param_3, param_4, param_5);
        }
      }
    }
  }
}
