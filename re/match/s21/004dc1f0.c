// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004dc1f0 (h3): best 14.0% (plain C form; raw converter 11.7% packed float bits through CONCAT44). The original is an SSE inline-assembly body in an ebp
   frame: test eax,0xf selects movups/movaps (0x004dc1fb..0x004dc217), shufps 0xa4 / mulps / addps per vertex,
   movlps+movss stores, params read through [ebp+8..0x14]. The ebp frame and the hand register choice
   (xmm3-5 for the matrix rows) are __asm traits; the brief forbids __asm. Class (c): inline asm. */
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
// MATCH: 0x004dc1f0 FUN_004dc1f0
/* C form of the SSE body: out = z*row2 + (y*row1 + x*row0), rows at matrix +0/+0x10/+0x20, count >= 1.
   The original computes in single-precision SSE; this C form runs through x87. */
float *FUN_004dc1f0(float *param_1, float *param_2, int param_3, float *param_4)
{
  float *o = param_1;
  float x, y, z;
  do {
    x = param_2[0];
    y = param_2[1];
    z = param_2[2];
    param_2 += 3;
    o[0] = z * param_4[8] + (y * param_4[4] + x * param_4[0]);
    o[1] = z * param_4[9] + (y * param_4[5] + x * param_4[1]);
    o[2] = z * param_4[10] + (y * param_4[6] + x * param_4[2]);
    o += 3;
  } while (--param_3 != 0);
  return param_1;
}
