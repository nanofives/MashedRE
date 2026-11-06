// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c3c60 (h5): 97.3% (regs 97.3%). RwV2dNormalize: x*x + y*y, the RW table sqrt and inverse sqrt through
   two RwSplitBits unions (forms of h3's 0x004c3b30 / 0x004c3b90, volatile table pointers), out = in * recip, and
   RWERROR (pluginID 1, FUN_004d7ff0(0x19), FUN_004d8480(&err)) when length <= 0.0f (0x005d757c). Returns length.
   FLAGS: /G5 (RW core build flag, h5 finding).
   Residual: the one x87 operand order of out->y (original `fld in->y; fmul recip`, ours `fld recip; fmul in->y`).
   Tried: both operand orders on both lines, a float recip local, a pointer to the union member, a volatile-float
   union for recip, an inline scale helper (67.1%), permuter 134 compiles. Class (b). */
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
typedef struct RwV2d { float x; float y; } RwV2d;
typedef union RwSplitBits { float nReal; volatile int nInt; volatile unsigned int nUInt; } RwSplitBits;
typedef struct RwError { int pluginID; int errorCode; } RwError;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d3ffc, 0x007d3ffc);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
int FUN_004d7ff0(int code);
RwError *FUN_004d8480(RwError *err);

// MATCH: 0x004c3c60 FUN_004c3c60
float FUN_004c3c60(RwV2d *out, const RwV2d *in)
{
  RwSplitBits s;
  RwSplitBits u;
  float len2;
  len2 = in->x * in->x + in->y * in->y;
  s.nReal = len2;
  if (s.nInt) {
    const volatile unsigned int *tab = *(volatile unsigned int **)(DAT_007d3ff8 + DAT_007d3ffc);
    s.nUInt += 0x800;
    s.nUInt = tab[(s.nUInt >> 12) & 0xfff] + ((s.nUInt >> 1) & 0x3fc00000);
  }
  u.nReal = len2;
  if (u.nInt) {
    const volatile unsigned int *tab = *(volatile unsigned int **)(DAT_007d3ff8 + DAT_007d3ffc + 4);
    u.nUInt += 0x800;
    u.nUInt = tab[(u.nUInt >> 12) & 0xfff] + ((~u.nUInt >> 1) & 0x3fc00000);
  }
  out->x = u.nReal * in->x;
  out->y = in->y * u.nReal;
  if (s.nReal <= DAT_005d757c) {
    RwError err;
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x19);
    FUN_004d8480(&err);
  }
  return s.nReal;
}
