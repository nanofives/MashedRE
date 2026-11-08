// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00516bf0: libpng 1.0.12 pngrio.c png_set_read_fn (game variant: 4th arg stored at +0x14c;
   libpair hint png_set_progressive_read_fn). Written by hand from the decomp. */
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
MGLOBALA(char, s_the_same_structure__Resetting_re_00621404, 53, 0x00621404);
MGLOBALA(char, s_Attempted_to_set_both_read_data__0062143c, 56, 0x0062143c);
void FUN_00517270(int png_ptr, char *msg);
// MATCH: 0x00516bf0 FUN_00516bf0
void FUN_00516bf0(int png_ptr, int io_ptr, int read_data_fn, int p4)
{
  *(int *)(png_ptr + 0x54) = io_ptr;
  *(int *)(png_ptr + 0x4c) = read_data_fn;
  *(int *)(png_ptr + 0x14c) = p4;
  if (*(int *)(png_ptr + 0x50) != 0) {
    *(int *)(png_ptr + 0x50) = 0;
    FUN_00517270(png_ptr, s_Attempted_to_set_both_read_data__0062143c);
    FUN_00517270(png_ptr, s_the_same_structure__Resetting_re_00621404);
  }
}
