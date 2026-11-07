// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00519820: libpng 1.0.12 pngrtran.c png_do_unshift. Official source, png_row_info layout. Ghidra size 418
   ends at the switch table, the obj symbol is 455 B.
   Best 96.3% (regs 96.3%). Residual (a): the original pushes ebx/ebp/esi/edi before the palette early-out
   (interleaved with cmp/je), ours shrink-wraps the early-out above the last two pushes. Tried early-return
   form, if(value) nesting (86.7), int value, all flag sets, permuter 176 compiles. */
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
typedef unsigned int png_uint_32;
typedef unsigned int png_size_t;
typedef unsigned char png_byte;
typedef unsigned char *png_bytep;
typedef struct png_row_info_struct
{
   png_uint_32 width;
   png_uint_32 rowbytes;
   png_byte color_type;
   png_byte bit_depth;
   png_byte channels;
   png_byte pixel_depth;
} png_row_info;
typedef png_row_info *png_row_infop;
typedef unsigned short png_uint_16;
typedef struct png_color_8_struct
{
   png_byte red;
   png_byte green;
   png_byte blue;
   png_byte gray;
   png_byte alpha;
} png_color_8;
typedef png_color_8 *png_color_8p;
// MATCH: 0x00519820 FUN_00519820
void FUN_00519820(png_row_infop row_info, png_bytep row, png_color_8p sig_bits)
{
   if (
       row_info->color_type != 3)
   {
      int shift[4];
      int channels = 0;
      int c;
      png_uint_16 value = 0;
      png_uint_32 row_width = row_info->width;

      if (row_info->color_type & 2)
      {
         shift[channels++] = row_info->bit_depth - sig_bits->red;
         shift[channels++] = row_info->bit_depth - sig_bits->green;
         shift[channels++] = row_info->bit_depth - sig_bits->blue;
      }
      else
      {
         shift[channels++] = row_info->bit_depth - sig_bits->gray;
      }
      if (row_info->color_type & 4)
      {
         shift[channels++] = row_info->bit_depth - sig_bits->alpha;
      }

      for (c = 0; c < channels; c++)
      {
         if (shift[c] <= 0)
            shift[c] = 0;
         else
            value = 1;
      }

      if (!value)
         return;

      switch (row_info->bit_depth)
      {
         case 2:
         {
            png_bytep bp;
            png_uint_32 i;
            png_uint_32 istop = row_info->rowbytes;

            for (bp = row, i = 0; i < istop; i++)
            {
               *bp >>= 1;
               *bp++ &= 0x55;
            }
            break;
         }
         case 4:
         {
            png_bytep bp = row;
            png_uint_32 i;
            png_uint_32 istop = row_info->rowbytes;
            png_byte mask = (png_byte)((((int)0xf0 >> shift[0]) & (int)0xf0) |
               (png_byte)((int)0xf >> shift[0]));

            for (i = 0; i < istop; i++)
            {
               *bp >>= shift[0];
               *bp++ &= mask;
            }
            break;
         }
         case 8:
         {
            png_bytep bp = row;
            png_uint_32 i;
            png_uint_32 istop = row_width * channels;

            for (i = 0; i < istop; i++)
            {
               *bp++ >>= shift[i%channels];
            }
            break;
         }
         case 16:
         {
            png_bytep bp = row;
            png_uint_32 i;
            png_uint_32 istop = channels * row_width;

            for (i = 0; i < istop; i++)
            {
               value = (png_uint_16)((*bp << 8) + *(bp + 1));
               value >>= shift[i%channels];
               *bp++ = (png_byte)(value >> 8);
               *bp++ = (png_byte)(value & 0xff);
            }
            break;
         }
      }
   }
}
