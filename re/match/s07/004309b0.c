// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x004309b0: 100% by hand (raw 88.2%): switch on DAT_0067e9fc - 2 with the case bodies in source
   order 0,1,2,(4..7 -> 3),3 -> 5,8; default returns the subtracted value. Jump table at 0x004309e8 checked by hand
   against the original (entry 3 -> ret 5, entries 4..7 -> ret 3). */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0067e9fc, 0x0067e9fc);
// MATCH: 0x004309b0 FUN_004309b0
int FUN_004309b0(void)
{
  int v;
  v = DAT_0067e9fc - 2;
  switch (v) {
  case 0:
    return 0;
  case 1:
    return 1;
  case 2:
    return 2;
  case 4:
  case 5:
  case 6:
  case 7:
    return 3;
  case 3:
    return 5;
  case 8:
    v = 0xb;
  }
  return v;
}
