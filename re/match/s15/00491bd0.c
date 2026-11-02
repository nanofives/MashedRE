// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h6 0x00491bd0: whole obj symbol BYTE-IDENTICAL (40 B, match.py --bytes); per-instruction 54.5% only because Ghidra's size (28 B) stops inside the loop. Hand: 0x20-byte element array indexed by count (char* + byte offset gave [ecx+eax] instead of [eax+ecx]). Residual: none. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef struct { int a; int b[7]; } V20;
MGLOBAL(V20 *, DAT_00869ca0, 0x00869ca0);
// MATCH: 0x00491bd0 FUN_00491bd0
void FUN_00491bd0(void)
{
  int i;
  for (i = 0; i < 0x200; i++) {
    DAT_00869ca0[i].a = 0;
  }
}
