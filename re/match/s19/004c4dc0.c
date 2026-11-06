// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c4dc0 (h5): 91.2% (regs 91.2%). RwMatrixInvert: identity shortcut (module word & flags & 0x20000 ->
   struct copy), orthonormal case ((flags & 3) == 3: transpose + pos = -dot(pos, row), flags = 3), else
   FUN_004c4eb0(dst, matrix) (general invert, 2 stack args). FLAGS: /G5 (RW core build flag, h5 finding).
   Residual: x87 operand order inside the three pos dot products (the original's term/operand order differs per row:
   row 1 z term right.z first, row 2 x term first, row 3 y term pos first). Tried: 3 DOT bodies x 8 argument orders
   (best: inline `z*z + (x*x + y*y)`, DOT(pos,right), DOT(pos,up), DOT(at,pos)), a greedy row-by-row search over
   explicit expressions (288 forms: term permutation x paren x operand order, none better), permuter 184. Class (b). */
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
typedef struct RwV3d { float x; float y; float z; } RwV3d;
typedef struct RwMatrix { RwV3d right; unsigned int flags; RwV3d up; unsigned int pad1; RwV3d at; unsigned int pad2; RwV3d pos; unsigned int pad3; } RwMatrix;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4028, 0x007d4028);
RwMatrix *FUN_004c4eb0(RwMatrix *dst, const RwMatrix *matrix);
static __inline float DOT(const RwV3d *a, const RwV3d *b) { return a->z * b->z + (a->x * b->x + a->y * b->y); }

// MATCH: 0x004c4dc0 FUN_004c4dc0
RwMatrix *FUN_004c4dc0(RwMatrix *dst, const RwMatrix *matrix)
{
  if (*(unsigned int *)(DAT_007d3ff8 + DAT_007d4028 + 4) & matrix->flags & 0x20000) {
    *dst = *matrix;
    return dst;
  }
  if ((matrix->flags & 3) == 3) {
    dst->right.x = matrix->right.x;
    dst->right.y = matrix->up.x;
    dst->right.z = matrix->at.x;
    dst->up.x = matrix->right.y;
    dst->up.y = matrix->up.y;
    dst->up.z = matrix->at.y;
    dst->at.x = matrix->right.z;
    dst->at.y = matrix->up.z;
    dst->at.z = matrix->at.z;
    dst->pos.x = -DOT(&matrix->pos, &matrix->right);
    dst->pos.y = -DOT(&matrix->pos, &matrix->up);
    dst->pos.z = -DOT(&matrix->at, &matrix->pos);
    dst->flags = 3;
    return dst;
  }
  FUN_004c4eb0(dst, matrix);
  return dst;
}
