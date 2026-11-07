// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cb740 (h2): 100% (regs 100%). D3DLIGHT9-style compare: && chain of the first 4 dwords, then switch
   on type with case 1/2/3 each 'if (!(a && b ...)) return 0; break;' and a common return 1 (gives the dec/je chain
   and the original's block order). Raw 31.7%. Residual: none. relocheck OK. */
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
typedef unsigned int undefined4;
// MATCH: 0x004cb740 FUN_004cb740
undefined4 FUN_004cb740(int *param_1,int *param_2)
{
  if (param_1[0] == param_2[0] && param_1[1] == param_2[1] && param_1[2] == param_2[2] &&
      param_1[3] == param_2[3]) {
    switch (param_1[0]) {
    case 1:
      if (!(param_1[13] == param_2[13] && param_1[14] == param_2[14] && param_1[15] == param_2[15] &&
            param_1[19] == param_2[19])) return 0;
      break;
    case 2:
      if (!(param_1[13] == param_2[13] && param_1[14] == param_2[14] && param_1[15] == param_2[15] &&
            param_1[16] == param_2[16] && param_1[17] == param_2[17] && param_1[18] == param_2[18] &&
            param_1[19] == param_2[19] && param_1[24] == param_2[24] && param_1[25] == param_2[25])) return 0;
      break;
    case 3:
      if (!(param_1[16] == param_2[16] && param_1[17] == param_2[17] && param_1[18] == param_2[18])) return 0;
      break;
    }
    return 1;
  }
  return 0;
}
