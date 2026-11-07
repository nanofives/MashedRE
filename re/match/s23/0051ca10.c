// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051ca10: libpng 1.0.12 pngwio.c png_set_read_fn (PNG_NO_STDIO form: read_data_fn stored as is)
   100.0% (regs 100, ops 100), first source-form pass. Residual: none. */
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
MGLOBALA(char, s_It_s_an_error_to_set_both_read_d_006216a8, 65, 0x006216a8);
MGLOBALA(char, s_same_structure__Resetting_write__00621674, 52, 0x00621674);
void FUN_00517270(int png_ptr, const char *msg);

// MATCH: 0x0051ca10 FUN_0051ca10
void FUN_0051ca10(int png_ptr, void *io_ptr, void *read_data_fn)
{
   *(void **)(png_ptr + 0x54) = io_ptr;
   *(void **)(png_ptr + 0x50) = read_data_fn;
   if (*(void **)(png_ptr + 0x4c) != 0)
   {
      *(void **)(png_ptr + 0x4c) = 0;
      FUN_00517270(png_ptr, s_It_s_an_error_to_set_both_read_d_006216a8);
      FUN_00517270(png_ptr, s_same_structure__Resetting_write__00621674);
   }
   *(void **)(png_ptr + 0x14c) = 0;
}
