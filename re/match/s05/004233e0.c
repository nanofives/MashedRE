// FLAGS: /O2 /GS /Ob1
/* 0x004233e0 (157 B). Best 100.0% exact (regs 100, ops 100).
   Raw g2s 16.9%. Float literals for the pooled constants (0.0 @0x005d757c, 180.0 @0x005cd09c, 360.0 @0x005ccac4,
   270.0 @0x005cd324, 90.0 @0x005ccad0) and 57.2958f promoted to the double @0x005ccae0 (= (double)57.2958f)
   by the double atan; intrinsic atan gives fld1/fpatan. Negate-then-subtract as two statements (fchs; fsubr
   dword). The y == 0 arm sits after the ret and jumps back past the 360 wrap because the wrap is AFTER the
   if/else in source: with constant 270/90 in that arm cl threads the jump over the wrap test. Spelled with
   the wrap inside the atan arm: 72.7% (arm inline). `x < 0 ? 270 : 90` gives the test ah,5 / jp form.
   Residual: none. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
#ifdef __cplusplus
extern "C" double atan(double);
#else
double atan(double);
#endif
#pragma intrinsic(atan)

// MATCH: 0x004233e0 FUN_004233e0
float FUN_004233e0(float x, float y)
{
  float a;
  if (y != 0.0f) {
    a = -(atan(x / y) * 57.2958f);
    a = 180.0f - a;
    if (y > 0.0f) a += 180.0f;
  } else
    a = (x < 0.0f) ? 270.0f : 90.0f;
  if (a >= 360.0f) a -= 360.0f;
  if (x == 0.0f && y == 0.0f) a = 0.0f;
  return a;
}
