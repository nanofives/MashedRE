// FLAGS: /O2 /GS /Ob1
/* 0x0040ad40: FUN_0046d660(&v, h) reads a 3-float vector for handle h, scales each by param_2,
   FUN_0046bf50(&v, h) writes it back. Callees are cdecl (`ret`, odis). Vec3 struct local with correct
   callee arity (2): 100%, 74 B BYTE-IDENTICAL. Residual: none. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
typedef struct V3_0040ad40 {
  float x;
  float y;
  float z;
} V3_0040ad40;

undefined4 FUN_0046d660(V3_0040ad40 *param_1, uint param_2);
undefined4 FUN_0046bf50(V3_0040ad40 *param_1, uint param_2);

// MATCH: 0x0040ad40 FUN_0040ad40
void FUN_0040ad40(uint param_1, float param_2)
{
  V3_0040ad40 v;

  FUN_0046d660(&v, param_1);
  v.x = v.x * param_2;
  v.y = v.y * param_2;
  v.z = v.z * param_2;
  FUN_0046bf50(&v, param_1);
}
