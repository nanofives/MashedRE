// FLAGS: /O2 /Ob1 /G5
// h2 hand-matched. 90.7% (regs 92.6%, ops 96.3%). KD-tree bbox walk (Box = 6 floats, node stack + box stack). Residual: scheduling of the hp lea vs the first rep movsd, [ecx+eax] vs [eax+ecx] operand order, and one extra reload of node->axis before the lo store. Tried axis local (spills), pointer temporaries (worse). Class (b).
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

typedef struct { float v[6]; } Box;
typedef struct Node { int axis; int f4; struct Node *left; struct Node *right; float lo; float hi; char pad[0x30]; Box box; } Node;
// MATCH: 0x004e51e0 FUN_004e51e0
int FUN_004e51e0(int param_1)
{
  Node *node = *(Node **)(param_1 + 0x1c);
  int sp = 0;
  Box cur;
  Node *nstack[64];
  Box bstack[64];
  Box *bp = bstack;
  char *hp;

  cur = *(Box *)(param_1 + 0x50);
  hp = (char *)bstack + 0xc;
  do {
    if (node->axis < 0) {
      node->box = cur;
      node = nstack[sp];
      sp--;
      cur = *bp;
      bp--;
      hp -= 0x18;
    } else {
      sp++;
      nstack[sp] = node->right;
      bp++;
      *bp = cur;
      hp += 0x18;
      {
        float hi = node->hi;
        *(float *)(hp + node->axis) = hi;
      }
      {
        float lo = node->lo;
        *(float *)((char *)&cur + node->axis) = lo;
      }
      node = node->left;
    }
  } while (sp >= 0);
  return param_1;
}
