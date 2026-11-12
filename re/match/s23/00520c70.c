// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00520c70: libpng 1.0.12 pngwtran.c png_do_shift, official body without the
   PNG_USELESS_TESTS NULL checks.
   52.3% (regs 60.5, ops 89.7). Residual: frame 0x34 vs 0x28. The original spills i / v / -j / channels into
   the dead argument slots and keeps value (16-bit branch) in ebx; ours uses new frame slots, so every stack
   offset and the loop register allocation differ. Tried: all block locals at function scope (34.9%), 5 flag sets,
   permuter 245 variants. Class (b) register allocation / stack packing. */
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
typedef struct png_row_info_s { uint width; uint rowbytes; uchar color_type; uchar bit_depth; uchar channels;
   uchar pixel_depth; } png_row_info;
typedef png_row_info *png_row_infop;
typedef struct png_color_8_s { uchar red; uchar green; uchar blue; uchar gray; uchar alpha; } png_color_8;
typedef png_color_8 *png_color_8p;
typedef uchar png_byte;
typedef uchar *png_bytep;
typedef uint png_uint_32;
typedef ushort png_uint_16;
// MATCH: 0x00520c70 FUN_00520c70
void FUN_00520c70(png_row_infop row_info, png_bytep row, png_color_8p bit_depth)
{
   if (
      row_info->color_type != 3)
   {
      int shift_start[4], shift_dec[4];
      int channels = 0;

      if (row_info->color_type & 2)
      {
         shift_start[channels] = row_info->bit_depth - bit_depth->red;
         shift_dec[channels] = bit_depth->red;
         channels++;
         shift_start[channels] = row_info->bit_depth - bit_depth->green;
         shift_dec[channels] = bit_depth->green;
         channels++;
         shift_start[channels] = row_info->bit_depth - bit_depth->blue;
         shift_dec[channels] = bit_depth->blue;
         channels++;
      }
      else
      {
         shift_start[channels] = row_info->bit_depth - bit_depth->gray;
         shift_dec[channels] = bit_depth->gray;
         channels++;
      }
      if (row_info->color_type & 4)
      {
         shift_start[channels] = row_info->bit_depth - bit_depth->alpha;
         shift_dec[channels] = bit_depth->alpha;
         channels++;
      }

      /* with low row depths, could only be grayscale, so one channel */
      if (row_info->bit_depth < 8)
      {
         png_bytep bp = row;
         png_uint_32 i;
         png_byte mask;
         png_uint_32 row_bytes = row_info->rowbytes;

         if (bit_depth->gray == 1 && row_info->bit_depth == 2)
            mask = 0x55;
         else if (row_info->bit_depth == 4 && bit_depth->gray == 3)
            mask = 0x11;
         else
            mask = 0xff;

         for (i = 0; i < row_bytes; i++, bp++)
         {
            png_uint_16 v;
            int j;

            v = *bp;
            *bp = 0;
            for (j = shift_start[0]; j > -shift_dec[0]; j -= shift_dec[0])
            {
               if (j > 0)
                  *bp |= (png_byte)((v << j) & 0xff);
               else
                  *bp |= (png_byte)((v >> (-j)) & mask);
            }
         }
      }
      else if (row_info->bit_depth == 8)
      {
         png_bytep bp = row;
         png_uint_32 i;
         png_uint_32 istop = channels * row_info->width;

         for (i = 0; i < istop; i++, bp++)
         {

            png_uint_16 v;
            int j;
            int c = (int)(i%channels);

            v = *bp;
            *bp = 0;
            for (j = shift_start[c]; j > -shift_dec[c]; j -= shift_dec[c])
            {
               if (j > 0)
                  *bp |= (png_byte)((v << j) & 0xff);
               else
                  *bp |= (png_byte)((v >> (-j)) & 0xff);
            }
         }
      }
      else
      {
         png_bytep bp;
         png_uint_32 i;
         png_uint_32 istop = channels * row_info->width;

         for (bp = row, i = 0; i < istop; i++)
         {
            int c = (int)(i%channels);
            png_uint_16 value, v;
            int j;

            v = (png_uint_16)(((png_uint_16)(*bp) << 8) + *(bp + 1));
            value = 0;
            for (j = shift_start[c]; j > -shift_dec[c]; j -= shift_dec[c])
            {
               if (j > 0)
                  value |= (png_uint_16)((v << j) & (png_uint_16)0xffff);
               else
                  value |= (png_uint_16)((v >> (-j)) & (png_uint_16)0xffff);
            }
            *bp++ = (png_byte)(value >> 8);
            *bp++ = (png_byte)(value & 0xff);
         }
      }
   }
}
