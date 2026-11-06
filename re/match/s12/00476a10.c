// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00476a10 (h2): 100%, raw 20.0%. 64-byte struct copy (rep movsd x16) from param or default
   0x00692558 into 0x006924e8. Residual: none. */
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
typedef struct { unsigned int v[16]; } S40;
MGLOBAL(S40, DAT_006924e8, 0x006924e8);
MGLOBAL(S40, DAT_00692558, 0x00692558);
// MATCH: 0x00476a10 FUN_00476a10
void FUN_00476a10(S40 *param_1)
{
  if (param_1 == 0) {
    param_1 = &DAT_00692558;
  }
  DAT_006924e8 = *param_1;
}
