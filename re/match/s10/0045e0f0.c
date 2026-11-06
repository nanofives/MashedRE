// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045e0f0 (h3): 100% (regs 100%), byte-identical. Raw converter 51.4%: clamp written as
   if (<0) 0 else if (>1) 1 on the float parameter itself; float callee argument. Residual: none. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0068f640, 0x0068f640);
MGLOBAL(float, DAT_0068f644, 0x0068f644);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);
// MATCH: 0x0045e0f0 FUN_0045e0f0
void FUN_0045e0f0(int param_1,float param_2)
{
  if (param_2 < 0.0f) {
    param_2 = 0.0f;
  }
  else if (param_2 > 1.0f) {
    param_2 = 1.0f;
  }
  *(float *)((&DAT_0068f640)[param_1 * 2] + 0x40) = param_2;
  if (*(int *)((&DAT_0068f640)[param_1 * 2] + 0x30) != 0) {
    FUN_005a6dc0(*(int *)((&DAT_0068f640)[param_1 * 2] + 0x30),5,1,param_2);
  }
  (&DAT_0068f644)[param_1 * 2] = param_2;
}
