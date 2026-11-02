// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x004764e0 (h2): 100%, raw 41.7%. Register-arg function (ECX/EDX + one stack arg, plain ret):
   a static callee (pilot finding 1) with a stand-in caller built only outside MATCH_PORT. Its real caller is
   0x004764f0, whose file also holds this static. Residual: none. */
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
void FUN_00476320(float *p0, float *p1, float p2);
// MATCH: 0x004764e0 FUN_004764e0
static void FUN_004764e0(float *out, float *org, float ang)
{
  FUN_00476320(out, org, ang);
}
#ifndef MATCH_PORT
/* stand-in caller (not the real 0x004764f0): only makes cl give FUN_004764e0 its register convention */
void h2_standin_004764e0(float *a, float *b, float c)
{
  FUN_004764e0(a, b, c);
  FUN_004764e0(b, a, c);
}
#endif
