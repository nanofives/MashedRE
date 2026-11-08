// FLAGS: /O1
/* MATCH-S21 0x004ec915 (h4): 100% (regs 100%) under /O1. Vec2 Hermite (D3DX row; Ghidra's comment says Catmull-Rom): h1 = 2s^3 - 3s^2 + 1, h2 = s^3 - 2s^2 + s, h3 = 3s^2 - 2s^3, h4 = s^3 - s^2. x is summed V1, T1, V2, T2 and y in reverse order. What moved the last instruction: float literals 3.0f / 1.0f (equal to 0x5cc31c / 0x5cc320) in place of the globals. 9176 CSEs the literal s^2 * 3 product with the original's operand order. Raw /O1 50.8%. Residual: none. */
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
// MATCH: 0x004ec915 FUN_004ec915
D3DXVECTOR2 * __stdcall FUN_004ec915(D3DXVECTOR2 *pOut, const D3DXVECTOR2 *pV1, const D3DXVECTOR2 *pT1,
                                     const D3DXVECTOR2 *pV2, const D3DXVECTOR2 *pT2, float s)
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
  return pOut;
}
