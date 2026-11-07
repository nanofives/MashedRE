// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d9a60 (h2): not matchable, class (c). Every float->int conversion is an inlined inline-asm
   truncation sequence (value stored to a float temp, fnstcw, `or ah,0xc`, fldcw, fistp, fldcw restore) -- the
   form RenderWare's x86 RwInt32FromReal asm macro produces. 13.00 C emits `call __ftol` for an (int) cast (or a
   bare fistp under /QIfist), never this sequence, and the brief forbids __asm.
   Math (cited from the disassembly): s = *(float *)0x005cc320 / in[0]; r = in[1]*s*k84, g = in[2]*s,
   b = in[3]*s*k80, a = in[4]*s, k84 = 0x005d8d84, k80 = 0x005d8d80; each value * k7c (0x005d8d7c) is rounded to
   float, truncated to int and written to out[0..3] as min(v, 0xff) (signed compare, `jge` keeps 0xff).
   The body below is a C stand-in with that math ((int) truncates like RC=11). Score is the stand-in's;
   relocheck reports BAD only because the stand-in code is positioned differently (globals verified by address). */
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
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005d8d84, 0x005d8d84);
MGLOBAL(float, DAT_005d8d80, 0x005d8d80);
MGLOBAL(float, DAT_005d8d7c, 0x005d8d7c);
// MATCH: 0x004d9a60 FUN_004d9a60
void FUN_004d9a60(unsigned char *out, const float *in)
{
  float s = DAT_005cc320 / in[0];
  float r = in[1] * s * DAT_005d8d84;
  float g = in[2] * s;
  float b = in[3] * s * DAT_005d8d80;
  float a = in[4] * s;
  float t;
  int ir, ig, ib, ia;
  t = r * DAT_005d8d7c; ir = (int)t;
  t = g * DAT_005d8d7c; ig = (int)t;
  t = b * DAT_005d8d7c; ib = (int)t;
  t = a * DAT_005d8d7c; ia = (int)t;
  out[0] = 0xff; if (ir < 0xff) out[0] = (unsigned char)ir;
  out[1] = 0xff; if (ig < 0xff) out[1] = (unsigned char)ig;
  out[2] = 0xff; if (ib < 0xff) out[2] = (unsigned char)ib;
  out[3] = 0xff; if (ia < 0xff) out[3] = (unsigned char)ia;
}
