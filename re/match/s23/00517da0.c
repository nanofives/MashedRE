// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00517da0: libpng 1.0.12 pngread.c png_read_image. Official source (height +0xcc,
   num_rows +0xd0). Best 97.1% (regs 97.1%). Residual (a): one spill store (pass counter into the png_ptr
   arg slot) is scheduled before `push edi` in ours, after it in the original. Tried loop direction,
   declaration order, *rp++ form, all flag sets, permuter 95 compiles: no move. */
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
int FUN_00515680(int png_ptr);
void FUN_00517940(int png_ptr, unsigned char *row, unsigned char *dsp_row);
// MATCH: 0x00517da0 FUN_00517da0
void FUN_00517da0(int png_ptr, unsigned char **image)
{
   unsigned int i,image_height;
   int pass, j;
   unsigned char **rp;

   pass = FUN_00515680(png_ptr);

   image_height=*(unsigned int *)(png_ptr + 0xcc);
   *(unsigned int *)(png_ptr + 0xd0) = image_height;

   for (j = 0; j < pass; j++)
   {
      rp = image;
      for (i = 0; i < image_height; i++)
      {
         FUN_00517940(png_ptr, *rp, 0);
         rp++;
      }
   }
}
