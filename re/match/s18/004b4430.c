// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b4430 (helper h1): 81.9% (regs 63.9%, ops 83.3%), 72/72 ins. Typed as a look-at on an RwMatrix-shaped pointer: at = target - pos, right = at x up-param, up = right x at, three normalizes (FUN_004c39b0), update (FUN_004c45f0). Pointer locals right/up/at give the original's esi/ebx/edi bases. Raw g2s 37.5%. Remaining: x87 operand order inside 4 of the 12 products (fld/fmul mirrored) and fstp st(0) after the first normalize scheduled after the param_3 load. Tried: greedy operand swap of all 12 products (no product order moves the code: the compiler canonicalizes), all 6 local declaration orders, member access without pointer locals (34.7%), Ghidra float* indexing (37.5%), permuter 237 variants. Same residual pattern as 0x004b42c0. Class (b). */
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
typedef struct { float x; float y; float z; } RwV3d_;
typedef struct { RwV3d_ right; uint flags; RwV3d_ up; uint pad1; RwV3d_ at; uint pad2; RwV3d_ pos; uint pad3; } RwMatrix_;
float FUN_004c39b0(RwV3d_ *p0, RwV3d_ *p1);
void FUN_004c45f0(RwMatrix_ *p0);
// MATCH: 0x004b4430 FUN_004b4430
void FUN_004b4430(RwMatrix_ *param_1,RwV3d_ *param_2,RwV3d_ *param_3)
{
  RwV3d_ *right;
  RwV3d_ *up;
  RwV3d_ *at;

  right = &param_1->right;
  up = &param_1->up;
  at = &param_1->at;
  at->x = param_2->x - param_1->pos.x;
  at->y = param_2->y - param_1->pos.y;
  at->z = param_2->z - param_1->pos.z;
  FUN_004c39b0(at,at);
  right->x = at->z * param_3->y - param_3->z * at->y;
  right->y = at->x * param_3->z - param_3->x * at->z;
  right->z = param_3->x * at->y - at->x * param_3->y;
  FUN_004c39b0(right,right);
  up->x = right->z * at->y - right->y * at->z;
  up->y = right->x * at->z - at->x * right->z;
  up->z = at->x * right->y - right->x * at->y;
  FUN_004c39b0(up,up);
  FUN_004c45f0(param_1);
}
