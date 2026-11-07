// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004df980 (h3): 100% (regs 100%). Raw CONCAT11 form 30.8%. '(p[0]&0xf0|0xf00) * 16' keeps the or before the shift
   (written as << 4 the compiler folds the or past the shl); (uint)p[2] >> 4 gives the shr. */
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
typedef unsigned int uint;
typedef unsigned char byte;
// MATCH: 0x004df980 FUN_004df980
uint FUN_004df980(byte *param_1)

{
  return ((param_1[0] & 0xf0 | 0xf00) * 16) | (param_1[1] & 0xf0) | ((uint)param_1[2] >> 4);
}
