// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004dc280 (h2): not matchable, class (c). The body is SSE inline assembly in an ebp frame
   (movups/movaps on the 16-byte alignment test of the matrix, shufps 0xa4, mulps/addps, movlps/movhlps): a
   one-point RwV3d transform out = (z*at + y*up) + (x*right + pos), matrix rows at +0x00/+0x10/+0x20/+0x30,
   returns out. 13.00 C has no SSE intrinsics path that emits this and the brief forbids __asm.
   The body below is a C stand-in with the same operation order (SSE rounds each op to float; x87 C may keep
   wider intermediates, so results can differ in the last bit). Score is the stand-in's. */
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
typedef struct RwMatrix { RwV3d right; unsigned int flags; RwV3d up; unsigned int pad1; RwV3d at;
  unsigned int pad2; RwV3d pos; unsigned int pad3; } RwMatrix;
// MATCH: 0x004dc280 FUN_004dc280
RwV3d *FUN_004dc280(RwV3d *out, const RwV3d *in, const RwMatrix *m)
{
  float x = in->x, y = in->y, z = in->z;
  out->x = (z * m->at.x + y * m->up.x) + (x * m->right.x + m->pos.x);
  out->y = (z * m->at.y + y * m->up.y) + (x * m->right.y + m->pos.y);
  out->z = (z * m->at.z + y * m->up.z) + (x * m->right.z + m->pos.z);
  return out;
}
