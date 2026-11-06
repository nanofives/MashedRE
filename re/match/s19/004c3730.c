// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c3730 (h3): best 40.4% (regs 40.4%). Tried: scalar x/y/z locals (11.5%), RwV3d vIn copy (28.8%), one reused scalar for y then z
   (34.6%, permuter 38.5%), __inline V3Scale/V3IncScaled/V3Add helpers (23.1%). The original keeps in.x on the x87
   stack and spills in.y/in.z through one arg slot, vOut.y/z to memory; 9176 never chose that schedule. Class (b). */
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

// MATCH: 0x004c3730 FUN_004c3730
RwV3d *FUN_004c3730(RwV3d *pointOut, const RwV3d *pointIn, const RwMatrix *matrix)
{
  RwV3d vOut;
  vOut.x = matrix->right.x * pointIn->x;
  vOut.y = matrix->right.y * pointIn->x;
  vOut.z = matrix->right.z * pointIn->x;
  vOut.x += matrix->up.x * pointIn->y;
  vOut.y += matrix->up.y * pointIn->y;
  vOut.z += matrix->up.z * pointIn->y;
  vOut.x += matrix->at.x * pointIn->z;
  vOut.y += matrix->at.y * pointIn->z;
  vOut.z += matrix->at.z * pointIn->z;
  pointOut->x = vOut.x + matrix->pos.x;
  pointOut->y = vOut.y + matrix->pos.y;
  pointOut->z = vOut.z + matrix->pos.z;
  return pointOut;
}
