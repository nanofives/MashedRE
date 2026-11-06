// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00472740 (h7): 100%. Ghidra showed 1 arg; real signature (out, a, b, t): t clamped to [0, 1] (0x5d757c /
   0x5cc320), four byte lerps through _ftol. Residual: none. */
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
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
// MATCH: 0x00472740 FUN_00472740
void FUN_00472740(unsigned char *param_1,unsigned char *param_2,unsigned char *param_3,float param_4)
{
  if (param_4 < DAT_005d757c) {
    param_4 = DAT_005d757c;
  }
  else if (param_4 > DAT_005cc320) {
    param_4 = DAT_005cc320;
  }
  param_1[0] = (unsigned char)(int)((param_3[0] - param_2[0]) * param_4 + param_2[0]);
  param_1[1] = (unsigned char)(int)((param_3[1] - param_2[1]) * param_4 + param_2[1]);
  param_1[2] = (unsigned char)(int)((param_3[2] - param_2[2]) * param_4 + param_2[2]);
  param_1[3] = (unsigned char)(int)((param_3[3] - param_2[3]) * param_4 + param_2[3]);
}
