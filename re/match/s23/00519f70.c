// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00519f70: libpng 1.0.12 pngrtran.c png_do_rgb_to_gray. Official source, png_row_info layout, except: png_ptr and
   row copied from *_arg locals (arg-slot packing, 84.0 -> 98.4), and both 16-bit branches written with the
   test inverted (no-gamma loop first, `if (gamma_16_to_1 == NULL || gamma_16_from_1 == NULL)`), found by
   match_permute (98.4 -> 100). PP(T, off) reads png_struct at the game offsets. */
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
// MATCH: 0x00519f70 FUN_00519f70
#define PP(T, off) (*(T *)((char *)png_ptr + (off)))
int FUN_00519f70(void *png_ptr_arg, png_row_infop row_info, png_bytep row_arg)
{
   void *png_ptr = png_ptr_arg;
   png_bytep row = row_arg;
   png_uint_32 i;

   png_uint_32 row_width = row_info->width;
   int rgb_error = 0;

   if (
      (row_info->color_type & 2))
   {
      png_uint_32 rc = PP(png_uint_16, 0x1da);
      png_uint_32 gc = PP(png_uint_16, 0x1dc);
      png_uint_32 bc = PP(png_uint_16, 0x1de);

      if (row_info->color_type == 2)
      {
         if (row_info->bit_depth == 8)
         {
            if (PP(png_bytep, 0x168) != 0 && PP(png_bytep, 0x16c) != 0)
            {
               png_bytep sp = row;
               png_bytep dp = row;

               for (i = 0; i < row_width; i++)
               {
                  png_byte red   = PP(png_bytep, 0x16c)[*(sp++)];
                  png_byte green = PP(png_bytep, 0x16c)[*(sp++)];
                  png_byte blue  = PP(png_bytep, 0x16c)[*(sp++)];
                  if(red != green || red != blue)
                  {
                     rgb_error |= 1;
                     *(dp++) = PP(png_bytep, 0x168)[
                       (rc*red+gc*green+bc*blue)>>15];
                  }
                  else
                     *(dp++) = *(sp-1);
               }
            }
            else
            {
               png_bytep sp = row;
               png_bytep dp = row;
               for (i = 0; i < row_width; i++)
               {
                  png_byte red   = *(sp++);
                  png_byte green = *(sp++);
                  png_byte blue  = *(sp++);
                  if(red != green || red != blue)
                  {
                     rgb_error |= 1;
                     *(dp++) = (png_byte)((rc*red+gc*green+bc*blue)>>15);
                  }
                  else
                     *(dp++) = *(sp-1);
               }
            }
         }

         else /* RGB bit_depth == 16 */
         {
            if (PP(png_uint_16pp, 0x178) == 0 ||
                PP(png_uint_16pp, 0x174) == 0)
            {
               png_bytep sp = row;
               png_bytep dp = row;
               for (i = 0; i < row_width; i++)
               {
                  png_uint_16 red, green, blue, gray16;

                  red   = (png_uint_16)(((*(sp))<<8) | *(sp+1)); sp+=2;
                  green = (png_uint_16)(((*(sp))<<8) | *(sp+1)); sp+=2;
                  blue  = (png_uint_16)(((*(sp))<<8) | *(sp+1)); sp+=2;

                  if(red != green || red != blue)
                     rgb_error |= 1;
                  gray16  = (png_uint_16)((rc*red + gc*green + bc*blue)>>15);
                  *(dp++) = (png_byte)((gray16>>8) & 0xff);
                  *(dp++) = (png_byte)(gray16 & 0xff);
               }
            }
            else
            {
               png_bytep sp = row;
               png_bytep dp = row;
               for (i = 0; i < row_width; i++)
               {
                  png_uint_16 red, green, blue, w;

                  red   = (png_uint_16)(((*(sp))<<8) | *(sp+1)); sp+=2;
                  green = (png_uint_16)(((*(sp))<<8) | *(sp+1)); sp+=2;
                  blue  = (png_uint_16)(((*(sp))<<8) | *(sp+1)); sp+=2;

                  if(red == green && red == blue)
                     w = red;
                  else
                  {
                     png_uint_16 red_1   = PP(png_uint_16pp, 0x178)[(red&0xff) >>
                                  PP(int, 0x158)][red>>8];
                     png_uint_16 green_1 = PP(png_uint_16pp, 0x178)[(green&0xff) >>
                                  PP(int, 0x158)][green>>8];
                     png_uint_16 blue_1  = PP(png_uint_16pp, 0x178)[(blue&0xff) >>
                                  PP(int, 0x158)][blue>>8];
                     png_uint_16 gray16  = (png_uint_16)((rc*red_1 + gc*green_1
                                  + bc*blue_1)>>15);
                     w = PP(png_uint_16pp, 0x174)[(gray16&0xff) >>
                         PP(int, 0x158)][gray16 >> 8];
                     rgb_error |= 1;
                  }

                  *(dp++) = (png_byte)((w>>8) & 0xff);
                  *(dp++) = (png_byte)(w & 0xff);
               }
            }
         }
      }
      if (row_info->color_type == 6)
      {
         if (row_info->bit_depth == 8)
         {
            if (PP(png_bytep, 0x168) != 0 && PP(png_bytep, 0x16c) != 0)
            {
               png_bytep sp = row;
               png_bytep dp = row;
               for (i = 0; i < row_width; i++)
               {
                  png_byte red   = PP(png_bytep, 0x16c)[*(sp++)];
                  png_byte green = PP(png_bytep, 0x16c)[*(sp++)];
                  png_byte blue  = PP(png_bytep, 0x16c)[*(sp++)];
                  if(red != green || red != blue)
                     rgb_error |= 1;
                  *(dp++) =  PP(png_bytep, 0x168)
                             [(rc*red + gc*green + bc*blue)>>15];
                  *(dp++) = *(sp++);  /* alpha */
               }
            }
            else
            {
               png_bytep sp = row;
               png_bytep dp = row;
               for (i = 0; i < row_width; i++)
               {
                  png_byte red   = *(sp++);
                  png_byte green = *(sp++);
                  png_byte blue  = *(sp++);
                  if(red != green || red != blue)
                     rgb_error |= 1;
                  *(dp++) =  (png_byte)((gc*red + gc*green + bc*blue)>>8);
                  *(dp++) = *(sp++);  /* alpha */
               }
            }
         }
         else /* RGBA bit_depth == 16 */
         {
            if (PP(png_uint_16pp, 0x178) == 0 ||
                PP(png_uint_16pp, 0x174) == 0)
            {
               png_bytep sp = row;
               png_bytep dp = row;
               for (i = 0; i < row_width; i++)
               {
                  png_uint_16 red, green, blue, gray16;
                  red   = (png_uint_16)((*(sp)<<8) | *(sp+1)); sp+=2;
                  green = (png_uint_16)((*(sp)<<8) | *(sp+1)); sp+=2;
                  blue  = (png_uint_16)((*(sp)<<8) | *(sp+1)); sp+=2;
                  if(red != green || red != blue)
                     rgb_error |= 1;
                  gray16  = (png_uint_16)((rc*red + gc*green + bc*blue)>>15);
                  *(dp++) = (png_byte)((gray16>>8) & 0xff);
                  *(dp++) = (png_byte)(gray16 & 0xff);
                  *(dp++) = *(sp++);  /* alpha */
                  *(dp++) = *(sp++);
               }
            }
            else
            {
               png_bytep sp = row;
               png_bytep dp = row;
               for (i = 0; i < row_width; i++)
               {
                  png_uint_16 red, green, blue, w;

                  red   = (png_uint_16)(((*(sp))<<8) | *(sp+1)); sp+=2;
                  green = (png_uint_16)(((*(sp))<<8) | *(sp+1)); sp+=2;
                  blue  = (png_uint_16)(((*(sp))<<8) | *(sp+1)); sp+=2;

                  if(red == green && red == blue)
                     w = red;
                  else
                  {
                     png_uint_16 red_1   = PP(png_uint_16pp, 0x178)[(red&0xff) >>
                                  PP(int, 0x158)][red>>8];
                     png_uint_16 green_1 = PP(png_uint_16pp, 0x178)[(green&0xff) >>
                                  PP(int, 0x158)][green>>8];
                     png_uint_16 blue_1  = PP(png_uint_16pp, 0x178)[(blue&0xff) >>
                                  PP(int, 0x158)][blue>>8];
                     png_uint_16 gray16  = (png_uint_16)((rc * red_1
                                  + gc * green_1 + bc * blue_1)>>15);
                     w = PP(png_uint_16pp, 0x174)[(gray16&0xff) >>
                         PP(int, 0x158)][gray16 >> 8];
                     rgb_error |= 1;
                  }

                  *(dp++) = (png_byte)((w>>8) & 0xff);
                  *(dp++) = (png_byte)(w & 0xff);
                  *(dp++) = *(sp++);  /* alpha */
                  *(dp++) = *(sp++);
               }
            }
         }
      }
   row_info->channels -= (png_byte)2;
      row_info->color_type &= ~2;
      row_info->pixel_depth = (png_byte)(row_info->channels *
         row_info->bit_depth);
      row_info->rowbytes = ((row_width *
         row_info->pixel_depth + 7) >> 3);
   }
   return rgb_error;
}
