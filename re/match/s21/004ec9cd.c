// FLAGS: /O1
/* MATCH-S21 0x004ec9cd (h4): 100% (regs 100%) under /O1. Vec2 Catmull-Rom (D3DX row; Ghidra's comment says Hermite): 0.5 * (a*V0 + b*V1 + c*V2 + d*V3) with a = 2s^2 - s^3 - s, b = 3s^3 - 5s^2 + 2, c = 4s^2 - 3s^3 + s, d = s^3 - s^2. y is summed in reverse order. What moved it: float literals (2.0f, 3.0f, 5.0f, 4.0f, 0.5f) in place of the MGLOBAL constants. 9176 CSEs literal loads, so 2.0 and 0.5 stay on the x87 stack as in the original. The literals are __real constants equal to 0x5cc574/31c/358/35c/32c. Raw /O1 59.4%. Residual: none. */
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
// MATCH: 0x004ec9cd FUN_004ec9cd
D3DXVECTOR2 * __stdcall FUN_004ec9cd(D3DXVECTOR2 *pOut, const D3DXVECTOR2 *pV0, const D3DXVECTOR2 *pV1,
                                     const D3DXVECTOR2 *pV2, const D3DXVECTOR2 *pV3, float s)
{
  float ss, sss, a, b, c, d;
  ss = s * s;
  sss = s * ss;
  a = ss * 2.0f - sss - s;
  b = sss * 3.0f - ss * 5.0f + 2.0f;
  c = ss * 4.0f - sss * 3.0f + s;
  d = sss - ss;
  pOut->x = (a * pV0->x + b * pV1->x + c * pV2->x + d * pV3->x) * 0.5f;
  pOut->y = (d * pV3->y + c * pV2->y + b * pV1->y + a * pV0->y) * 0.5f;
  return pOut;
}
