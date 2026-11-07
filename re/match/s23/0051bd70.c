// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051bd70: libpng 1.0.12 pngrtran.c png_do_expand. Official source, png_row_info layout. 100% with `row` copied
   from row_arg (arg-slot packing; 74.9 without it). */
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
typedef struct png_color_struct
{
   png_byte red;
   png_byte green;
   png_byte blue;
} png_color;
typedef png_color *png_colorp;
typedef png_uint_16 *png_uint_16p;
typedef struct png_color_16_struct
{
   png_byte index;
   png_uint_16 red;
   png_uint_16 green;
   png_uint_16 blue;
   png_uint_16 gray;
} png_color_16;
typedef png_color_16 *png_color_16p;
typedef png_byte *png_bytep_;
typedef png_uint_16p *png_uint_16pp;
// MATCH: 0x0051bd70 FUN_0051bd70
void FUN_0051bd70(png_row_infop row_info, png_bytep row_arg, png_color_16p trans_value)
{
   png_bytep row = row_arg;
   int shift, value;
   png_bytep sp, dp;
   png_uint_32 i;
   png_uint_32 row_width=row_info->width;

   {
      if (row_info->color_type == 0)
      {
         png_uint_16 gray = (png_uint_16)(trans_value ? trans_value->gray : 0);

         if (row_info->bit_depth < 8)
         {
            switch (row_info->bit_depth)
            {
               case 1:
               {
                  gray = (png_uint_16)(gray*0xff);
                  sp = row + (png_size_t)((row_width - 1) >> 3);
                  dp = row + (png_size_t)row_width - 1;
                  shift = 7 - (int)((row_width + 7) & 0x07);
                  for (i = 0; i < row_width; i++)
                  {
                     if ((*sp >> shift) & 0x01)
                        *dp = 0xff;
                     else
                        *dp = 0;
                     if (shift == 7)
                     {
                        shift = 0;
                        sp--;
                     }
                     else
                        shift++;

                     dp--;
                  }
                  break;
               }
               case 2:
               {
                  gray = (png_uint_16)(gray*0x55);
                  sp = row + (png_size_t)((row_width - 1) >> 2);
                  dp = row + (png_size_t)row_width - 1;
                  shift = (int)((3 - ((row_width + 3) & 0x03)) << 1);
                  for (i = 0; i < row_width; i++)
                  {
                     value = (*sp >> shift) & 0x03;
                     *dp = (png_byte)(value | (value << 2) | (value << 4) |
                        (value << 6));
                     if (shift == 6)
                     {
                        shift = 0;
                        sp--;
                     }
                     else
                        shift += 2;

                     dp--;
                  }
                  break;
               }
               case 4:
               {
                  gray = (png_uint_16)(gray*0x11);
                  sp = row + (png_size_t)((row_width - 1) >> 1);
                  dp = row + (png_size_t)row_width - 1;
                  shift = (int)((1 - ((row_width + 1) & 0x01)) << 2);
                  for (i = 0; i < row_width; i++)
                  {
                     value = (*sp >> shift) & 0x0f;
                     *dp = (png_byte)(value | (value << 4));
                     if (shift == 4)
                     {
                        shift = 0;
                        sp--;
                     }
                     else
                        shift = 4;

                     dp--;
                  }
                  break;
               }
            }
            row_info->bit_depth = 8;
            row_info->pixel_depth = 8;
            row_info->rowbytes = row_width;
         }

         if (trans_value != 0)
         {
            if (row_info->bit_depth == 8)
            {
               sp = row + (png_size_t)row_width - 1;
               dp = row + (png_size_t)(row_width << 1) - 1;
               for (i = 0; i < row_width; i++)
               {
                  if (*sp == gray)
                     *dp-- = 0;
                  else
                     *dp-- = 0xff;
                  *dp-- = *sp--;
               }
            }
            else if (row_info->bit_depth == 16)
            {
               sp = row + row_info->rowbytes - 1;
               dp = row + (row_info->rowbytes << 1) - 1;
               for (i = 0; i < row_width; i++)
               {
                  if (((png_uint_16)*(sp) |
                     ((png_uint_16)*(sp - 1) << 8)) == gray)
                  {
                     *dp-- = 0;
                     *dp-- = 0;
                  }
                  else
                  {
                     *dp-- = 0xff;
                     *dp-- = 0xff;
                  }
                  *dp-- = *sp--;
                  *dp-- = *sp--;
               }
            }
            row_info->color_type = 4;
            row_info->channels = 2;
            row_info->pixel_depth = (png_byte)(row_info->bit_depth << 1);
            row_info->rowbytes =
               ((row_width * row_info->pixel_depth) >> 3);
         }
      }
      else if (row_info->color_type == 2 && trans_value)
      {
         if (row_info->bit_depth == 8)
         {
            sp = row + (png_size_t)row_info->rowbytes - 1;
            dp = row + (png_size_t)(row_width << 2) - 1;
            for (i = 0; i < row_width; i++)
            {
               if (*(sp - 2) == trans_value->red &&
                  *(sp - 1) == trans_value->green &&
                  *(sp - 0) == trans_value->blue)
                  *dp-- = 0;
               else
                  *dp-- = 0xff;
               *dp-- = *sp--;
               *dp-- = *sp--;
               *dp-- = *sp--;
            }
         }
         else if (row_info->bit_depth == 16)
         {
            sp = row + row_info->rowbytes - 1;
            dp = row + (png_size_t)(row_width << 3) - 1;
            for (i = 0; i < row_width; i++)
            {
               if ((((png_uint_16)*(sp - 4) |
                  ((png_uint_16)*(sp - 5) << 8)) == trans_value->red) &&
                  (((png_uint_16)*(sp - 2) |
                  ((png_uint_16)*(sp - 3) << 8)) == trans_value->green) &&
                  (((png_uint_16)*(sp - 0) |
                  ((png_uint_16)*(sp - 1) << 8)) == trans_value->blue))
               {
                  *dp-- = 0;
                  *dp-- = 0;
               }
               else
               {
                  *dp-- = 0xff;
                  *dp-- = 0xff;
               }
               *dp-- = *sp--;
               *dp-- = *sp--;
               *dp-- = *sp--;
               *dp-- = *sp--;
               *dp-- = *sp--;
               *dp-- = *sp--;
            }
         }
         row_info->color_type = 6;
         row_info->channels = 4;
         row_info->pixel_depth = (png_byte)(row_info->bit_depth << 2);
         row_info->rowbytes =
            ((row_width * row_info->pixel_depth) >> 3);
      }
   }
}
