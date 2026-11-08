// FLAGS: /O1
/* MATCH-S21 0x004ec839 (h4): 100% (regs 100%) under /O1. Half-to-float array conversion (D3DX row) written from odis: uint h widened from the ushort, the literal masks 0xffff7c00 / 0xffff8000 / 0xffff8080 / 0xfffffbff, the mantissa test on the ushort, denormal branch before zero, and a separate int exponent (e - 15 + 127) << 23. Without that local 9176 folds the shift into (h & 0x7c00) + 0x1c000. Raw /O1 82.3%. Residual: none. */
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
// MATCH: 0x004ec839 FUN_004ec839
unsigned int * __stdcall FUN_004ec839(unsigned int *pOut, const unsigned short *pIn, unsigned int n)
{
  unsigned int i;
  for (i = 0; i < n; i++) {
    unsigned short w = pIn[i];
    unsigned int h = w;
    unsigned int r;
    if ((h & 0xffff7c00) == 0) {
      if (w & 0x3ff) {
        int e = -14;
        unsigned int m = h & 0x3ff;
        while ((m & 0x400) == 0) {
          e--;
          m <<= 1;
        }
        r = (((h & 0xffff8080) << 3 | m) & 0xfffffbff) << 13 | (e + 127) << 23;
      }
      else {
        r = (h & 0xffff8000) << 16;
      }
    }
    else {
      int e = (int)((h >> 10) & 0x1f) - 15;
      r = ((h & 0xffff8000) << 3 | (h & 0x3ff)) << 13 | (e + 127) << 23;
    }
    pOut[i] = r;
  }
  return pOut;
}
