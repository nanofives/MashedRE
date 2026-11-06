// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c4eb0 (h5): 47.7% (regs 63.3%). General RwMatrix invert (cofactors, det, recip = det ? 1/det : 1,
   adjugate * recip, pos = -(pos . columns), flags = 0); called by RwMatrixInvert 0x004c4dc0. Ghidra typed it
   void(float *, float *). FLAGS: /G5 (RW core build flag, h5 finding).
   Residual: the original tests det != 0 as an int through the src param slot (`fstp [esp+8]; mov edx,[esp+8];
   test edx,edx`, RwSplitBits-shaped) and keeps the scaled right.x on the x87 stack for pos.x while spilling the
   scaled right.y; ours compares with fcomp and spills differently. A RwSplitBits det (volatile or plain) gives the
   int test but swaps the dst/src registers (20.6%); greedy commutation of every product (no gain); permuter 892
   compiles reached 32.8% only by reordering statements into a semantically wrong order (rejected). Class (b). */
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

// MATCH: 0x004c4eb0 FUN_004c4eb0
void FUN_004c4eb0(RwMatrix *dst, const RwMatrix *src)
{
  float det, recip;
  dst->right.x = src->at.z * src->up.y - src->at.y * src->up.z;
  dst->right.y = -(src->at.z * src->right.y - src->at.y * src->right.z);
  dst->right.z = src->up.z * src->right.y - src->up.y * src->right.z;
  det = src->right.x * dst->right.x + src->up.x * dst->right.y + src->at.x * dst->right.z;
  recip = 1.0f;
  if (det != 0.0f) {
    recip = 1.0f / det;
  }
  dst->right.x *= recip;
  dst->right.y *= recip;
  dst->right.z *= recip;
  dst->up.x = -((src->at.z * src->up.x - src->at.x * src->up.z) * recip);
  dst->up.y = (src->at.z * src->right.x - src->at.x * src->right.z) * recip;
  dst->up.z = -((src->up.z * src->right.x - src->up.x * src->right.z) * recip);
  dst->at.x = (src->at.y * src->up.x - src->at.x * src->up.y) * recip;
  dst->at.y = -((src->at.y * src->right.x - src->at.x * src->right.y) * recip);
  dst->at.z = (src->up.y * src->right.x - src->up.x * src->right.y) * recip;
  dst->pos.x = -(src->pos.x * dst->right.x + src->pos.y * dst->up.x + src->pos.z * dst->at.x);
  dst->pos.y = -(src->pos.x * dst->right.y + src->pos.y * dst->up.y + src->pos.z * dst->at.y);
  dst->pos.z = -(src->pos.x * dst->right.z + src->pos.y * dst->up.z + src->pos.z * dst->at.z);
  dst->flags = 0;
}
