// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00497000 (h3): by hand, 100% (regs 100%), relocheck OK. D3D-style projection matrix from the
   camera (+0x70/+0x74 recip view window, +0x80 near, +0x84 far). `-q * near` matches; `-(q * near)` schedules
   the m[15] store after the fstp (96.6%). */
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
typedef struct Mat44 { float m[16]; } Mat44;
typedef struct CamProj { char pad0[0x70]; float rx; float ry; char pad78[8]; float nearPlane; float farPlane; } CamProj;
// MATCH: 0x00497000 FUN_00497000
void FUN_00497000(Mat44 *out, CamProj *cam)
{
  float q;
  out->m[0] = cam->rx;
  out->m[1] = 0.0f;
  out->m[2] = 0.0f;
  out->m[3] = 0.0f;
  out->m[4] = 0.0f;
  out->m[5] = cam->ry;
  out->m[6] = 0.0f;
  out->m[7] = 0.0f;
  out->m[8] = 0.0f;
  out->m[9] = 0.0f;
  q = cam->farPlane / (cam->farPlane - cam->nearPlane);
  out->m[10] = q;
  out->m[11] = -q * cam->nearPlane;
  out->m[12] = 0.0f;
  out->m[13] = 0.0f;
  out->m[14] = 1.0f;
  out->m[15] = 0.0f;
}
