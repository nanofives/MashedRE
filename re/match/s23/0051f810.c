// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051f810: libpng 1.0.12 pngwutil.c png_do_write_interlace (no PNG_USELESS_TESTS), png_memcpy =
   intrinsic memcpy, png_pass_start / png_pass_inc as image globals.
   100.0% (regs 100, ops 100). Official body; key: row_info and row copied from *_arg params so the loop spills land in
   their arg slots (66.5 without, 96.9 if pass is copied too). Residual: none. */
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
extern "C" void *__cdecl memcpy(void *d, const void *s, unsigned int n);
#else
void *__cdecl memcpy(void *d, const void *s, unsigned int n);
#endif
#pragma intrinsic(memcpy)
typedef struct
{
   unsigned int width;
   unsigned int rowbytes;
   unsigned char color_type;
   unsigned char bit_depth;
   unsigned char channels;
   unsigned char pixel_depth;
} png_row_info;

MGLOBALA(int, DAT_005e32c4, 7, 0x005e32c4);
MGLOBALA(int, DAT_005e32e0, 7, 0x005e32e0);

// MATCH: 0x0051f810 FUN_0051f810
void FUN_0051f810(png_row_info *row_info_arg, unsigned char *row_arg, int pass)
{
   png_row_info *row_info = row_info_arg;
   unsigned char *row = row_arg;

   /* we don't have to do anything on the last pass (6) */
   if (pass < 6)
   {
      /* each pixel depth is handled separately */
      switch (row_info->pixel_depth)
      {
         case 1:
         {
            unsigned char * sp;
            unsigned char * dp;
            int shift;
            int d;
            int value;
            unsigned int i;
            unsigned int row_width = row_info->width;

            dp = row;
            d = 0;
            shift = 7;
            for (i = DAT_005e32c4[pass]; i < row_width;
               i += DAT_005e32e0[pass])
            {
               sp = row + (unsigned int)(i >> 3);
               value = (int)(*sp >> (7 - (int)(i & 0x07))) & 0x01;
               d |= (value << shift);

               if (shift == 0)
               {
                  shift = 7;
                  *dp++ = (unsigned char)d;
                  d = 0;
               }
               else
                  shift--;

            }
            if (shift != 7)
               *dp = (unsigned char)d;
            break;
         }
         case 2:
         {
            unsigned char * sp;
            unsigned char * dp;
            int shift;
            int d;
            int value;
            unsigned int i;
            unsigned int row_width = row_info->width;

            dp = row;
            shift = 6;
            d = 0;
            for (i = DAT_005e32c4[pass]; i < row_width;
               i += DAT_005e32e0[pass])
            {
               sp = row + (unsigned int)(i >> 2);
               value = (*sp >> ((3 - (int)(i & 0x03)) << 1)) & 0x03;
               d |= (value << shift);

               if (shift == 0)
               {
                  shift = 6;
                  *dp++ = (unsigned char)d;
                  d = 0;
               }
               else
                  shift -= 2;
            }
            if (shift != 6)
                   *dp = (unsigned char)d;
            break;
         }
         case 4:
         {
            unsigned char * sp;
            unsigned char * dp;
            int shift;
            int d;
            int value;
            unsigned int i;
            unsigned int row_width = row_info->width;

            dp = row;
            shift = 4;
            d = 0;
            for (i = DAT_005e32c4[pass]; i < row_width;
               i += DAT_005e32e0[pass])
            {
               sp = row + (unsigned int)(i >> 1);
               value = (*sp >> ((1 - (int)(i & 0x01)) << 2)) & 0x0f;
               d |= (value << shift);

               if (shift == 0)
               {
                  shift = 4;
                  *dp++ = (unsigned char)d;
                  d = 0;
               }
               else
                  shift -= 4;
            }
            if (shift != 4)
               *dp = (unsigned char)d;
            break;
         }
         default:
         {
            unsigned char * sp;
            unsigned char * dp;
            unsigned int i;
            unsigned int row_width = row_info->width;
            unsigned int pixel_bytes;

            /* start at the beginning */
            dp = row;
            /* find out how many bytes each pixel takes up */
            pixel_bytes = (row_info->pixel_depth >> 3);
            /* loop through the row, only looking at the pixels that
               matter */
            for (i = DAT_005e32c4[pass]; i < row_width;
               i += DAT_005e32e0[pass])
            {
               /* find out where the original pixel is */
               sp = row + (unsigned int)i * pixel_bytes;
               /* move the pixel */
               if (dp != sp)
                  memcpy(dp, sp, pixel_bytes);
               /* next pixel */
               dp += pixel_bytes;
            }
            break;
         }
      }
      /* set new row width */
      row_info->width = (row_info->width +
         DAT_005e32e0[pass] - 1 -
         DAT_005e32c4[pass]) /
         DAT_005e32e0[pass];
         row_info->rowbytes = ((row_info->width *
            row_info->pixel_depth + 7) >> 3);
   }
}
