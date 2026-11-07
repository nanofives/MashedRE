// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 h2 0x004b4650 (91 B): by hand, 100%. Lerp: out = b - a; out = t * out; out = out + a, as three
   separate per-component statement groups (the compiler spills t*z into the param_4 slot, as the original). */
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
typedef struct { float x, y, z; } V3;
// MATCH: 0x004b4650 FUN_004b4650
void FUN_004b4650(V3 *param_1, V3 *param_2, V3 *param_3, float param_4)
{
  param_1->x = param_3->x - param_2->x;
  param_1->y = param_3->y - param_2->y;
  param_1->z = param_3->z - param_2->z;
  param_1->x = param_4 * param_1->x;
  param_1->y = param_4 * param_1->y;
  param_1->z = param_4 * param_1->z;
  param_1->x = param_1->x + param_2->x;
  param_1->y = param_1->y + param_2->y;
  param_1->z = param_1->z + param_2->z;
}
