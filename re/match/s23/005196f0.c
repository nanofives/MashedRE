// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x005196f0: libpng 1.0.12 pngrtran.c png_do_unpack. Official source, png_row_info layout.
   100%: `row` renamed row_arg and copied into a local at entry (h2 trick), so the compiler packs the
   spilled local into the row arg slot as the original does (97% without it). */
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
// MATCH: 0x005196f0 FUN_005196f0
void FUN_005196f0(png_row_infop row_info, png_bytep row_arg)
{
   png_bytep row = row_arg;
   if (row_info->bit_depth < 8)
   {
      png_uint_32 i;
      png_uint_32 row_width=row_info->width;

      switch (row_info->bit_depth)
      {
         case 1:
         {
            png_bytep sp = row + (png_size_t)((row_width - 1) >> 3);
            png_bytep dp = row + (png_size_t)row_width - 1;
            png_uint_32 shift = 7 - (int)((row_width + 7) & 0x07);
            for (i = 0; i < row_width; i++)
            {
               *dp = (png_byte)((*sp >> shift) & 0x01);
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

            png_bytep sp = row + (png_size_t)((row_width - 1) >> 2);
            png_bytep dp = row + (png_size_t)row_width - 1;
            png_uint_32 shift = (int)((3 - ((row_width + 3) & 0x03)) << 1);
            for (i = 0; i < row_width; i++)
            {
               *dp = (png_byte)((*sp >> shift) & 0x03);
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
            png_bytep sp = row + (png_size_t)((row_width - 1) >> 1);
            png_bytep dp = row + (png_size_t)row_width - 1;
            png_uint_32 shift = (int)((1 - ((row_width + 1) & 0x01)) << 2);
            for (i = 0; i < row_width; i++)
            {
               *dp = (png_byte)((*sp >> shift) & 0x0f);
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
      row_info->pixel_depth = (png_byte)(8 * row_info->channels);
      row_info->rowbytes = row_width * row_info->channels;
   }
}
