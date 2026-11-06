// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c3880 (h3): best 51.1% (regs 51.1%). RwV3dTransformVector C fallback (twin of 0x004c3730). One float reused for in.y then in.z gives
   the arg-slot spill. Tried struct vOut (32.6%), field stores (31.1%), operand-order flips (no change), permuter
   (no gain). Residual: the original computes right.y*x as fld m; fmul st(2) keeping x on the stack; 9176 dups x
   (fld st(0)) and fxch. Class (b). */
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
typedef struct RwMatrix { RwV3d right; unsigned int flags; RwV3d up; unsigned int pad1; RwV3d at; unsigned int pad2; RwV3d pos; unsigned int pad3; } RwMatrix;

// MATCH: 0x004c3880 FUN_004c3880
RwV3d *FUN_004c3880(RwV3d *vectorOut, const RwV3d *vectorIn, const RwMatrix *matrix)
{
  float x = vectorIn->x;
  float vx, vy, vz;
  vx = x * matrix->right.x;
  vy = x * matrix->right.y;
  vz = x * matrix->right.z;
  x = vectorIn->y;
  vx += x * matrix->up.x;
  vy += x * matrix->up.y;
  vz += x * matrix->up.z;
  x = vectorIn->z;
  vectorOut->x = vx + x * matrix->at.x;
  vectorOut->y = vy + x * matrix->at.y;
  vectorOut->z = vz + x * matrix->at.z;
  return vectorOut;
}
