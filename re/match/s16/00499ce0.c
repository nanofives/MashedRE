// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00499ce0 (h6): sets the 4x4 float matrix at 0x00773928 to identity. The store order is
   the chained-assignment form of the d3dx9math.inl D3DXMatrixIdentity body (right-to-left evaluation).
   By hand: 100% exact (regs 100%), relocheck OK. Residual: none. */
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
typedef struct _D3DMATRIX { float m[4][4]; } D3DMATRIX;
MGLOBAL(D3DMATRIX, DAT_00773928, 0x00773928);
// MATCH: 0x00499ce0 FUN_00499ce0
int FUN_00499ce0(void)
{
  DAT_00773928.m[0][1] = DAT_00773928.m[0][2] = DAT_00773928.m[0][3] =
  DAT_00773928.m[1][0] = DAT_00773928.m[1][2] = DAT_00773928.m[1][3] =
  DAT_00773928.m[2][0] = DAT_00773928.m[2][1] = DAT_00773928.m[2][3] =
  DAT_00773928.m[3][0] = DAT_00773928.m[3][1] = DAT_00773928.m[3][2] = 0.0f;
  DAT_00773928.m[0][0] = DAT_00773928.m[1][1] = DAT_00773928.m[2][2] = DAT_00773928.m[3][3] = 1.0f;
  return 1;
}
