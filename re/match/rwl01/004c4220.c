// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004c4220: 40.7% regs 40.7%. Triple product right.(up x at) over 16-byte-stride vectors. Original keeps the three cofactors on the x87 stack then m1*B, m0*A, m2*C; tried float/double locals, struct cross/dot inline, operand orders: compiler always schedules cofactor/multiply interleaved. class (b). */
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
typedef long double float10;
typedef struct { float x, y, z; } V3;
typedef struct { V3 right; float f0; V3 up; float f1; V3 at; float f2; } M3;
// MATCH: 0x004c4220 FUN_004c4220
float FUN_004c4220(M3 *m)
{
  float cx = m->at.z * m->up.y - m->up.z * m->at.y;
  float cy = m->up.z * m->at.x - m->at.z * m->up.x ;
  float cz = m->at.y*m->up.x - m->up.y * m->at.x;
  return m->right.x * cx + m->right.y * cy + m->right.z * cz;
}
