// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 h2 0x004b4c80: by hand, 100%. (72 B) Same form as 0x004b4a80: FUN_00538c80(p1, p2, FUN_004b4bb0, &cb), cb = {p3, &result, &param_3, p1}. */
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
typedef struct { int p3; int *result; int *pp3; int p1; } Cb_4b4c80;
typedef void *(*CbFn_4b4c80)(void *, void *, void *);
void *FUN_004b4bb0(void *p0, void *p1, void *p2);
void *FUN_00538c80(int p0, void *p1, CbFn_4b4c80 p2, void *p3);
// MATCH: 0x004b4c80 FUN_004b4c80
int FUN_004b4c80(int param_1, void *param_2, int param_3)
{
  int result;
  Cb_4b4c80 cb;

  result = 0;
  cb.p3 = param_3;
  cb.result = &result;
  cb.pp3 = &param_3;
  cb.p1 = param_1;
  FUN_00538c80(param_1, param_2, FUN_004b4bb0, &cb);
  return result;
}
