// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c4360 (h5): 65.2% (regs 65.2%). Matrix identity error: right.x-1, up.y-1, at.z-1 and the other nine
   elements (pos included) as twelve float locals, returning the sum of all squares. Ghidra typed it void(void); it
   returns float in ST0. FLAGS: /G5 (RW core build flag, h5 finding).
   Residual: the original stores the seven memory-resident locals (via int moves) lazily, interleaved with the
   arithmetic, and keeps pos.y/pos.z on the x87 stack; ours stores them all up front. Group/term order in the sum is
   canonicalised (no effect); assignment order moves it (56.1 -> 65.2%); RwMatrix/RwV3d copies 16.7-37.9%;
   partial-sum form 57.6%; permuter 98 compiles no gain. Class (b). */
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

// MATCH: 0x004c4360 FUN_004c4360
float FUN_004c4360(const RwMatrix *matrix)
{
  float rx, ry, rz, ux, uy, uz, ax, ay, az, px, py, pz;
  rx = matrix->right.x - 1.0f; uy = matrix->up.y - 1.0f; az = matrix->at.z - 1.0f; pz = matrix->pos.z; py = matrix->pos.y;
  px = matrix->pos.x; rz = matrix->right.z; ry = matrix->right.y; uz = matrix->up.z; ux = matrix->up.x; ay = matrix->at.y; ax = matrix->at.x;
  return (rx * rx + ry * ry + rz * rz) + (px * px + py * py + pz * pz) + (ax * ax + ay * ay + az * az) + (ux * ux + uy * uy + uz * uz);
}
