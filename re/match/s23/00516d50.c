// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00516d50: libpng 1.0.12 png.c png_calculate_crc. Official source (need_crc flag).
   Best 83.3% (regs 83.3%). Residual (a): the original hoists the one load of flags (+0x6c) above the
   chunk_name branch, ours loads it in each arm. A flags local gives the hoist but then loads chunk_name
   into cl instead of test-mem (62.5). Tried early returns (78.3), comma/local forms, byte test of +0x6d,
   all flag sets (83.3), permuter 89 compiles. */
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
unsigned int FUN_00522970(unsigned int crc, unsigned char *buf, unsigned int len);
// MATCH: 0x00516d50 FUN_00516d50
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length)
{
   int need_crc = 1;

   if (*(unsigned char *)(png_ptr + 0x11c) & 0x20)
   {
      if ((*(unsigned int *)(png_ptr + 0x6c) & 0x300) == 0x300)
         need_crc = 0;
   }
   else
   {
      if (*(unsigned int *)(png_ptr + 0x6c) & 0x800)
         need_crc = 0;
   }

   if (need_crc)
      *(unsigned int *)(png_ptr + 0x110) = FUN_00522970(*(unsigned int *)(png_ptr + 0x110), ptr, length);
}
