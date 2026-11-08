// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f1130 (h1): 100% under /O2 /Ob1 /G5. Raw 25.0% (void, ++ through a short gives inc word ptr).
   Post-increments the 16-bit counter at RwGlobals (DAT_007d3ff8) + DAT_00912118 and returns the old value: the
   original keeps the old value in ax and builds the new one in esi (lea esi,[eax+1]), which only the returned
   post-increment produces. */
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
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00912118, 0x00912118);
// MATCH: 0x004f1130 FUN_004f1130
unsigned short FUN_004f1130(void)
{
  return (*(unsigned short *)(DAT_007d3ff8 + DAT_00912118))++;
}
