// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c5010 (h5): 84.7% (regs 87.8%). RwMatrixScale: switch on combineOp: 0 = identity (inlined
   set-identity, flags |= 0x20003) then the scale diagonal, 1 = rows scaled by scale->x/y/z, 2 = columns (and pos)
   scaled, default = RWERROR(0x80000003, "Invalid combination type") and matrix = NULL (the original then clears the
   flags through NULL, reproduced); flags &= ~0x20003; returns matrix. FLAGS: /G5 (RW core build flag, h5 finding).
   Residual: x87 operand order of 18 of the 21 `fld; fmul` pairs (which of matrix/scale is loaded first). Source
   operand order has no effect at all (greedy commutation of every product: zero moves); a row-pointer local in
   case 1 fixed the identity stores (83.2 -> 84.7%); an inline row-scale helper 84.7% (worse regs). Class (a). */
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
typedef struct RwError { int pluginID; int errorCode; } RwError;
MGLOBALA(char, s_Invalid_combination_type_0061811c, 25, 0x0061811c);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);

// MATCH: 0x004c5010 FUN_004c5010
RwMatrix *FUN_004c5010(RwMatrix *matrix, const RwV3d *scale, int combineOp)
{
  switch (combineOp) {
  case 0:
    matrix->right.x = matrix->up.y = matrix->at.z = 1.0f;
    matrix->flags |= 0x20003;
    matrix->up.x = 0.0f;
    matrix->right.z = 0.0f;
    matrix->right.y = 0.0f;
    matrix->at.y = 0.0f;
    matrix->at.x = 0.0f;
    matrix->up.z = 0.0f;
    matrix->pos.z = 0.0f;
    matrix->pos.y = 0.0f;
    matrix->pos.x = 0.0f;
    matrix->right.x = scale->x;
    matrix->up.y = scale->y;
    matrix->at.z = scale->z;
    break;
  case 1:
    {
      RwV3d *v;
      v = &matrix->right; v->x = v->x * scale->x; v->y = v->y * scale->x; v->z = v->z * scale->x;
      v = &matrix->up; v->x = v->x * scale->y; v->y = v->y * scale->y; v->z = v->z * scale->y;
      v = &matrix->at; v->x = v->x * scale->z; v->y = v->y * scale->z; v->z = v->z * scale->z;
    }
    break;
  case 2:
    matrix->right.x = matrix->right.x * scale->x;
    matrix->right.y = matrix->right.y * scale->y;
    matrix->right.z = matrix->right.z * scale->z;
    matrix->up.x = matrix->up.x * scale->x;
    matrix->up.y = scale->y * matrix->up.y;
    matrix->up.z = matrix->up.z * scale->z;
    matrix->at.x = matrix->at.x * scale->x;
    matrix->at.y = matrix->at.y * scale->y;
    matrix->at.z = matrix->at.z * scale->z;
    matrix->pos.x = matrix->pos.x * scale->x;
    matrix->pos.y = scale->y * matrix->pos.y;
    matrix->pos.z = matrix->pos.z * scale->z;
    break;
  default:
    {
      RwError err;
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(0x80000003, s_Invalid_combination_type_0061811c);
      FUN_004d8480(&err);
    }
    matrix = 0;
    break;
  }
  matrix->flags &= ~0x20003;
  return matrix;
}
