// FLAGS: /O1
/* MATCH-S21 0x004ecb97 (h4): 100% (regs 100%) under /O1. Vec3 Hermite (D3DX row; Ghidra's comment says Catmull-Rom), the 3-component twin of 0x004ec915. x is summed V1, T1, V2, T2 and y, z in reverse order. Float literals 3.0f / 1.0f. Raw /O1 46.6%. Residual: none. */
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
typedef struct D3DXVECTOR3 { float x, y, z; } D3DXVECTOR3;
// MATCH: 0x004ecb97 FUN_004ecb97
D3DXVECTOR3 * __stdcall FUN_004ecb97(D3DXVECTOR3 *pOut, const D3DXVECTOR3 *pV1, const D3DXVECTOR3 *pT1,
                                     const D3DXVECTOR3 *pV2, const D3DXVECTOR3 *pT2, float s)
{
  float h1, h2, h3, h4;
  float s2 = s * s;
  float s3 = s * s2;
  h1 = s3 + s3 - s2 * 3.0f + 1.0f;
  h2 = s3 - (s2 + s2) + s;
  h3 = s2 * 3.0f - (s3 + s3);
  h4 = s3 - s2;
  pOut->x = h1 * pV1->x + h2 * pT1->x + h3 * pV2->x + h4 * pT2->x;
  pOut->y = h4 * pT2->y + h3 * pV2->y + h2 * pT1->y + h1 * pV1->y;
  pOut->z = h4 * pT2->z + h3 * pV2->z + h2 * pT1->z + h1 * pV1->z;
  return pOut;
}
