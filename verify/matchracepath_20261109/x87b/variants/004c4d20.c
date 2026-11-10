// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c4d20 (h5): 90.9% (regs 90.9%). RwMatrixRotate: angle *= pi/180 (0x005cd7a8), axis normalised with
   _rwInvSqrt (FUN_004c3b90) of the squared length, sin/cos intrinsics, then FUN_004c4a50(matrix, &unitAxis,
   1 - cos, sin, combineOp) (5 stack args). The squared length goes through a RwSplitBits union so the argument is
   copied through eax (`fstp [slot]; mov eax,[slot]; push eax`); dot written `z*z + (x*x + y*y)`; scaled values
   through float locals before the stores.
   FLAGS: /G5 (RW core build flag, h5 finding); plain /O2 /GS /Ob1 gives 81.8%.
   Residual: the original keeps recip live on the x87 stack through the scale (`fld [esi+4]; fmul st(1)` for y and z,
   final `fstp st(0)`); ours copies it (`fld st(0); fmul [esi+4]`) and consumes it on z. Tried all 8 operand orders,
   float locals for axis or results, inline scale / normalise helpers (void and value-returning), an inline invsqrt
   wrapper, permuter (208 + 116 compiles). Class (b). */
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
typedef union RwSplitBits { float nReal; volatile int nInt; volatile unsigned int nUInt; } RwSplitBits;
double sin(double);
double cos(double);
#pragma intrinsic(sin, cos)
float FUN_004c3b90(float num);
RwMatrix *FUN_004c4a50(RwMatrix *matrix, const RwV3d *unitAxis, float oneMinusCosine, float sine, int combineOp);

// MATCH: 0x004c4d20 FUN_004c4d20
RwMatrix *FUN_004c4d20(RwMatrix *matrix, const RwV3d *axis, float angle, int combineOp)
{
  RwV3d unitAxis;
  double recip;
  RwSplitBits s;
  angle *= 0.017453292f;
  s.nReal = axis->z * axis->z + (axis->x * axis->x + axis->y * axis->y);
  recip = FUN_004c3b90(s.nReal);
  { float ux, uy, uz; ux = (float)recip * axis->x; uy = axis->y * recip; uz = (float)recip * axis->z; unitAxis.x = ux; unitAxis.y = uy; unitAxis.z = uz; }
  FUN_004c4a50(matrix, &unitAxis, 1.0f - (float)cos(angle), (float)sin(angle), combineOp);
  return matrix;
}
