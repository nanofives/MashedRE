// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005aee20 (h3), PORT-TOP-AUDIO-5AA3D0-STOPS: BYTE-IDENTICAL 28 B. The original returns the bit index in AL (xor al,al 0x005aee24, inc al 0x005aee34, the loop leaves with al = index or 0x20); the old source (78.6%) returned void and masked the shift count with 0x1f (the original shifts by cl with no mask, shl esi,cl 0x005aee2e). Every caller uses the result only as a shift count (1 << FUN_005aee20(0x28)). */
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
// MATCH: 0x005aee20 FUN_005aee20
unsigned char FUN_005aee20(uint param_1)
{
  unsigned char bVar1;

  bVar1 = 0;
  do {
    if ((param_1 & 1 << bVar1) != 0) break;
    bVar1 = bVar1 + 1;
  } while (bVar1 < 0x20);
  return bVar1;
}
