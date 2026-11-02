// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00475f30: 100% (regs 100%), relocheck OK. By hand (raw 21.4%): the output pointer arrives in EAX, so the
   function is file-static (pilot finding 1) and the file holds a small stand-in caller (not the real TU; the real
   caller is 0x00475f90) that makes the compiler pick EAX. Source order 0x613258 * 0x613278 gives the original's
   load order fld [0x613278] / fmul [0x613258]. */
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
MGLOBAL(float, DAT_00613278, 0x00613278);
MGLOBAL(float, DAT_00613258, 0x00613258);
MGLOBAL(float, DAT_00613274, 0x00613274);
// MATCH: 0x00475f30 FUN_00475f30
static void FUN_00475f30(float *out, float param_1, float param_2)
{
  float f;
  f = DAT_00613258 * DAT_00613278 * param_1 * param_2 + DAT_00613274;
  out[1] = f;
  out[0] = f;
}
/* stand-in caller (not the real TU): makes the compiler pass `out` in EAX */
void FUN_00475f30_standin(float *o, float a, float b)
{
  FUN_00475f30(o, a, b);
  FUN_00475f30(o + 2, b, a);
}
