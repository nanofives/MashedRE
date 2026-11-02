// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00443d10: 100% by hand (h7). Raw 50.0%. Scaled values in float locals fz, fx (reusing the params 93.5%). Two (int)(v*4 +- 0.5) roundings through _ftol (0x004a2c48),
   a 128x128 short grid at 0x007f1a9c and 8x8 byte cells at 0x007f9a9c. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
MGLOBALA(short, DAT_007f1a9c, 0x4000, 0x007f1a9c);
MGLOBALA(uchar, DAT_007f9a9c, 0x8000, 0x007f9a9c);
// MATCH: 0x00443d10 FUN_00443d10
uchar FUN_00443d10(float param_1, float param_2)
{
  int iz;
  int ix;
  short s;
  float fx;
  float fz;

  fz = param_2 * 4.0f;
  fx = param_1 * 4.0f;
  iz = (int)(fz > 0.0f ? fz + 0.5f : fz - 0.5f);
  ix = (int)(fx > 0.0f ? fx + 0.5f : fx - 0.5f);
  s = DAT_007f1a9c[((iz + 0x1f0) >> 3) * 0x80 + ((ix + 0x1f0) >> 3)];
  if (s > 0 && s < 0x200) {
    return DAT_007f9a9c[(s * 8 + (ix & 7)) * 8 + (iz & 7)];
  }
  return 0xff;
}
