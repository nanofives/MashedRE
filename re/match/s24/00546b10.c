// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00546b10 (h1): 86.0% (regs 89.5%). RtQuatConvertFromMatrix shape: result = (q && m), if (result)
   {...}, return result (keeps 1 in ebx); the trace sum written twice (no trace local) so the x87 sum is reused in
   place by `fadd [1.0]`; a trace local gave `fld c; fadd st(1)` (81.6%). Function-pointer pick for the three
   off-diagonal cases. Residual (a): ours pushes ebx after the NULL checks (shrink-wrapped past the early return),
   the original pushes it in the prologue; every other instruction is equal. Tried: early return + result = 1,
   result = 0 then 1 in the branch, double trace, permuter (151 compiles, 0 gains on an earlier 74.4% form),
   game flags (77.9), /O1 (61.6), /O1 /Oy- (59.3). */
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
typedef struct RwV3d { float x, y, z; } RwV3d;
typedef struct RwMatrix { RwV3d right; int flags; RwV3d up; int pad1; RwV3d at; int pad2; RwV3d pos; int pad3; } RwMatrix;
typedef struct RtQuat { RwV3d imag; float real; } RtQuat;
typedef void (*QuatFromMatFn)(RtQuat *q, const RwMatrix *m);
float FUN_004c3b30(float x);
void FUN_00546bf0(RtQuat *q, const RwMatrix *m);
void FUN_00546c50(RtQuat *q, const RwMatrix *m);
void FUN_00546cb0(RtQuat *q, const RwMatrix *m);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);

// MATCH: 0x00546b10 FUN_00546b10
int FUN_00546b10(RtQuat *param_1, const RwMatrix *param_2)
{
  int result;
  float trace;
  float s;
  QuatFromMatFn fp;

  result = (param_1 != 0 && param_2 != 0);
  if (result) {
    if (param_2->right.x + param_2->up.y + param_2->at.z > DAT_005d757c) {
      s = FUN_004c3b30(param_2->right.x + param_2->up.y + param_2->at.z + DAT_005cc320);
      param_1->real = s * DAT_005cc32c;
      s = DAT_005cc32c / s;
      param_1->imag.x = (param_2->up.z - param_2->at.y) * s;
      param_1->imag.y = (param_2->at.x - param_2->right.z) * s;
      param_1->imag.z = (param_2->right.y - param_2->up.x) * s;
    }
    else {
      if (param_2->right.x > param_2->up.y) {
        if (param_2->right.x > param_2->at.z) {
          fp = FUN_00546bf0;
        }
        else {
          fp = FUN_00546cb0;
        }
      }
      else {
        if (param_2->up.y > param_2->at.z) {
          fp = FUN_00546c50;
        }
        else {
          fp = FUN_00546cb0;
        }
      }
      fp(param_1, param_2);
    }
  }
  return result;
}
