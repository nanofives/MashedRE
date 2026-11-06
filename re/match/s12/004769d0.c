// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x004769d0 (h2): 100%, raw 88.9%. Destination globals 0x006924dc/e0 are file-static (pilot
   finding 2): only then does cl load both source words before the stores. static under C, MGLOBAL under C++.
   Residual: none. */
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
#ifndef __cplusplus
static unsigned int DAT_006924dc;
static unsigned int DAT_006924e0;
#else
MGLOBAL(unsigned int, DAT_006924dc, 0x006924dc);
MGLOBAL(unsigned int, DAT_006924e0, 0x006924e0);
#endif
MGLOBALA(unsigned int, DAT_00613288, 2, 0x00613288);
// MATCH: 0x004769d0 FUN_004769d0
void FUN_004769d0(unsigned int *param_1)
{
  if (param_1 == 0) {
    param_1 = DAT_00613288;
  }
  DAT_006924dc = param_1[0];
  DAT_006924e0 = param_1[1];
}
