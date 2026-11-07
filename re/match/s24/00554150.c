// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00554150 (h1): 100% (regs 100%). Written as Ghidra shows it, with a PathNode struct (data, count, child at +0x14). */
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
typedef struct PathNode {
  void *data;
  int count;
  int f8;
  int fc;
  int f10;
  struct PathNode *child;
} PathNode;
typedef void (*FreeFn)(void *p);
typedef void (*FreeListFreeFn)(void *list, void *p);
int FUN_00553e80(PathNode *node);
void FUN_005c4d50(void *p);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(void *, DAT_00912a24, 0x00912a24);

// MATCH: 0x00554150 FUN_00554150
void FUN_00554150(PathNode *param_1)
{
  PathNode *child;
  PathNode *next;

  if (param_1->count == -1) {
    FUN_005c4d50(param_1->data);
  }
  else if (param_1->data != 0) {
    (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(param_1->data);
  }
  param_1->data = 0;
  param_1->count = 0;
  if (param_1->child != 0) {
    FUN_00554150(param_1->child);
    child = param_1->child;
    if (child != 0) {
      next = child->child;
      if (child->data != 0) {
        if (child->count == -1) {
          FUN_005c4d50(child->data);
        }
        else {
          (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(child->data);
        }
      }
      child->data = 0;
      (*(FreeListFreeFn *)(DAT_007d3ff8 + 0x11c))(DAT_00912a24, child);
      FUN_00553e80(next);
    }
    param_1->child = 0;
  }
}
