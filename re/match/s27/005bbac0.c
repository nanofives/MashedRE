// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005bbac0: RO-AUDIOINIT-45D460-REACH (2026-11-21), new source from the listing; 0x005bb850 stores its address at +0x18 (0x005bb8a2). */
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
/* listing 0x005bbac0..0x005bbb15 */
int FUN_005abe30(int p0, int p1, int p2, int p3);
int FUN_005bc0f0(int p0, int p1, int p2, int p3);
// MATCH: 0x005bbac0 FUN_005bbac0
int FUN_005bbac0(int param_1,int param_2,int param_3,int param_4)
{
  if ((*(byte *)(*(int *)(*(int *)(param_1 + 0x58) + 0x20) + 0x50) & 8) != 0) {
    if (FUN_005abe30(*(int *)(param_1 + 0x4c),param_2,param_3,param_4) == 0) {   /* 0x005bbae4 */
      return 0;
    }
  }
  else if (FUN_005bc0f0(param_1 + 0x5c,param_3,param_4,*(int *)(param_2 + 8)) == 0) {   /* 0x005bbb04 */
    return 0;
  }
  return param_1;
}
