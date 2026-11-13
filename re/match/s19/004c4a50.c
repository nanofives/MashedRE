// TOOLCHAIN rw9044
// TOOLCHAIN rw9044
/* MATCH-RWT-B 2026-11-12: 77.8 -> 97.2% under rw9044 (toolchain line added; ddk stays 77.8): comma rvalue (0, unitAxis->f) on the crossed and scaled products gives the original field-first fld order. Remaining: c2 schedules the pos=0 / flags=3 stores and the combineOp load after the x87 chain, the original interleaves them (11 statement positions tried, permuter 1003 compiles no gain). */
// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c4a50 (h5): 77.8% (regs 77.8%). RwMatrixRotateOneMinusCosineSine: builds the rotation matrix from
   leading / crossed / scaled RwV3d temps (flags 3, pos 0), then switch (combineOp): 0 copy, 1 / 2 RwMatrixMultiply
   (mTmp, mRotate, matrix) / (mTmp, matrix, mRotate) INLINED (same body as 0x004c4600, here a `static
   __forceinline` helper: plain `__inline` is not inlined, 27.4%) then *matrix = mTmp, default RWERROR
   (0x80000003, "Invalid combination type") and NULL. Each case returns directly (break + common return: 63.2%).
   FLAGS: /G5 (RW core build flag, h5 finding).
   Residual: (1) the compiler tail-merges case 1's two identity-copy exits into case 2's identical ones (original
   keeps four separate copies + epilogues, 28 instructions); (2) pos = 0 / flags = 3 stores are hoisted to the top
   (original: after right.y); (3) x87 operand order of the crossed products and scaled.y/z. Tried: zero-store
   placement (3 spots, no effect), `*matrix = *Multiply(...)` (63.2%), permuter 283 compiles. Class (b). */
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
typedef void (*fp_mtxmul)(RwMatrix *, const RwMatrix *, const RwMatrix *);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4028, 0x007d4028);
MGLOBALA(char, s_Invalid_combination_type_0061811c, 25, 0x0061811c);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);

static __forceinline RwMatrix *MatrixMultiply(RwMatrix *matrixOut, const RwMatrix *matrixIn1, const RwMatrix *matrixIn2)
{
  unsigned int flag1 = matrixIn1->flags;
  unsigned int flag2 = matrixIn2->flags;
  unsigned int ident = *(unsigned int *)(DAT_007d3ff8 + DAT_007d4028 + 4) & 0x20000;
  if (flag1 & ident) {
    *matrixOut = *matrixIn2;
    return matrixOut;
  }
  if (flag2 & ident) {
    *matrixOut = *matrixIn1;
    return matrixOut;
  }
  (*(fp_mtxmul *)(DAT_007d3ff8 + DAT_007d4028 + 8))(matrixOut, matrixIn1, matrixIn2);
  matrixOut->flags = flag1 & flag2;
  return matrixOut;
}

// MATCH: 0x004c4a50 FUN_004c4a50

RwMatrix *FUN_004c4a50(RwMatrix *matrix, const RwV3d *unitAxis, float oneMinusCosine, float sine, int combineOp)
{
  RwMatrix mRotate;
  RwMatrix mTmp;
  RwV3d leading, crossed, scaled;

  leading.x = 1.0f - unitAxis->x * unitAxis->x;
  leading.y = 1.0f - unitAxis->y * unitAxis->y;
  leading.z = 1.0f - unitAxis->z * unitAxis->z;
  leading.x = leading.x * oneMinusCosine;
  leading.y = leading.y * oneMinusCosine;
  leading.z = leading.z * oneMinusCosine;
  crossed.x = (0, unitAxis->y) * unitAxis->z;
  crossed.y = (0, unitAxis->z) * unitAxis->x;
  crossed.z = unitAxis->x * (0, unitAxis->y);
  crossed.x = crossed.x * oneMinusCosine;
  crossed.y = crossed.y * oneMinusCosine;
  crossed.z = crossed.z * oneMinusCosine;
  scaled.x = unitAxis->x * sine;
  scaled.y = (0, unitAxis->y) * sine;
  scaled.z = (0, unitAxis->z) * sine;

  mRotate.right.x = 1.0f - leading.x;
  mRotate.right.y = crossed.z + scaled.z;
  mRotate.right.z = crossed.y - scaled.y;
  mRotate.up.x = crossed.z - scaled.z;
  mRotate.up.y = 1.0f - leading.y;
  mRotate.up.z = crossed.x + scaled.x;
  mRotate.at.x = crossed.y + scaled.y;
  mRotate.at.y = crossed.x - scaled.x;
  mRotate.at.z = 1.0f - leading.z;
  mRotate.pos.x = 0.0f;
  mRotate.pos.y = 0.0f;
  mRotate.pos.z = 0.0f;
  mRotate.flags = 3;

  switch (combineOp) {
  case 0:
    *matrix = mRotate;
    return matrix;
  case 1:
    MatrixMultiply(&mTmp, &mRotate, matrix);
    *matrix = mTmp;
    return matrix;
  case 2:
    MatrixMultiply(&mTmp, matrix, &mRotate);
    *matrix = mTmp;
    return matrix;
  default:
    {
      RwError err;
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(0x80000003, s_Invalid_combination_type_0061811c);
      FUN_004d8480(&err);
    }
    return 0;
  }
  return matrix;
}
