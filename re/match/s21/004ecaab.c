// FLAGS: /O1
/* MATCH-S21 0x004ecaab (h4): 100% (regs 100%) under /O1. Vec2 barycentric (D3DX row): V1 + (V2-V1)*f + (V3-V1)*g, in the original's term order (Ghidra printed the g term first). Raw /O1 39.3%. Residual: none. */
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
typedef struct D3DXVECTOR2 { float x, y; } D3DXVECTOR2;
// MATCH: 0x004ecaab FUN_004ecaab
D3DXVECTOR2 * __stdcall FUN_004ecaab(D3DXVECTOR2 *pOut, const D3DXVECTOR2 *pV1, const D3DXVECTOR2 *pV2,
                                     const D3DXVECTOR2 *pV3, float f, float g)
{
  pOut->x = (pV2->x - pV1->x) * f + (pV3->x - pV1->x) * g + pV1->x;
  pOut->y = (pV2->y - pV1->y) * f + (pV3->y - pV1->y) * g + pV1->y;
  return pOut;
}
