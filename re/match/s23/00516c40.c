// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00516c40: libpng 1.0.12 png.c png_sig_cmp. Official source, png_memcmp = intrinsic memcmp
   (repe cmpsb). Best 79.5% (regs 79.5%). Residual (a): the original keeps two separate `return 0` exits and
   places the `num_to_check < 1` test after the `start > 7` exit. Our compiler tail-merges both exits and keeps
   the test inline. Tried: nested if/else (decomp form, 72.7), plain if, ==0, test order swapped, a named
   return var, /GS sets (55.1), permuter (85 compiles, no gain). */
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
#ifdef __cplusplus
extern "C"
#endif
int __cdecl memcmp(const void *a, const void *b, unsigned int n);
#pragma intrinsic(memcmp)
// MATCH: 0x00516c40 FUN_00516c40
int FUN_00516c40(unsigned char *sig, unsigned int start, unsigned int num_to_check)
{
   unsigned char png_signature[8] = {137, 80, 78, 71, 13, 10, 26, 10};
   if (num_to_check > 8)
      num_to_check = 8;
   else if (num_to_check < 1)
      return (0);

   if (start > 7)
      return (0);

   if (start + num_to_check > 8)
      num_to_check = 8 - start;

   return ((int)(memcmp(&sig[start], &png_signature[start], num_to_check)));
}
