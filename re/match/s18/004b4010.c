// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b4010 (helper h1): 100% (regs 100%). Fix: 8-byte ctx struct {key, out*}; result initialized AT its declaration (= 0 in the declarator). Assigning it as a statement let the compiler reuse param_2's slot (33%). Callback FUN_004b3f20 as a real function pointer. Residual: none. */
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
typedef struct { undefined4 key; undefined4 *out; } FindCtx_004b4010;
int FUN_004b3f20(int p0, void *p1);
int FUN_004e6760(int p0, int (*p1)(int, void *), void *p2);
// MATCH: 0x004b4010 FUN_004b4010
undefined4 FUN_004b4010(int param_1,undefined4 param_2)
{
  undefined4 result = 0;
  FindCtx_004b4010 ctx;

  ctx.key = param_2;
  ctx.out = &result;
  FUN_004e6760(param_1,FUN_004b3f20,&ctx);
  return result;
}
