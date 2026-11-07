// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x005185a0: libpng 1.0.12 pngrtran.c png_init_read_transformations. Official source with the
   game png_struct offsets (gamma / screen_gamma / background_gamma are floats at +0x15c / +0x160 / +0x134).
   Ghidra size 3242 ends at the switch tables.
   Best 27.3% (regs 46.2%, 606/816 ins). Residual (a): the original converts double->byte/short with an inline
   fnstcw / or ah,0xc / fldcw / fistp / fldcw sequence at 14 sites and keeps the doubles in an 8-aligned frame
   (and esp,-8); 9176 calls __ftol at each site and needs no aligned frame (same finding as h2 png_write_gAMA).
   Control flow, offsets and the integer paths are written; only one pass, no permuter (capped by the ftol class). */
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
typedef unsigned short png_uint_16;
typedef unsigned char png_byte;
typedef unsigned char *png_bytep;
typedef struct png_color_struct { png_byte red; png_byte green; png_byte blue; } png_color;
typedef png_color *png_colorp;
typedef struct png_color_16_struct { png_byte index; png_uint_16 red; png_uint_16 green; png_uint_16 blue;
   png_uint_16 gray; } png_color_16;
#define PP(T, off) (*(T *)((char *)png_ptr + (off)))
#define png_composite(composite, fg, alpha, bg)                            \
     { png_uint_16 temp = (png_uint_16)((png_uint_16)(fg) * (png_uint_16)(alpha) \
                        +        (png_uint_16)(bg)*(png_uint_16)(255 -       \
                        (png_uint_16)(alpha)) + (png_uint_16)128);           \
       (composite) = (png_byte)((temp + (temp >> 8)) >> 8); }

#define png_composite_16(composite, fg, alpha, bg)                         \
     { png_uint_32 temp = (png_uint_32)((png_uint_32)(fg) * (png_uint_32)(alpha) \
                        + (png_uint_32)(bg)*(png_uint_32)(65535L -           \
                        (png_uint_32)(alpha)) + (png_uint_32)32768L);        \
       (composite) = (png_uint_16)((temp + (temp >> 16)) >> 16); }
#ifdef __cplusplus
extern "C" {
#endif
double __cdecl fabs(double x);
double __cdecl pow(double x, double y);
#ifdef __cplusplus
}
#endif
#pragma intrinsic(fabs)
void FUN_0051c220(void *png_ptr);
// MATCH: 0x005185a0 FUN_005185a0
void FUN_005185a0(void *png_ptr)
{
  {
   int color_type = PP(png_byte, 0x126);

   if ((PP(png_uint_32, 0x70) & 0x0100) &&
       (PP(png_uint_32, 0x70) & 0x1000))
   {
      if (!(color_type & 2))  /* i.e., GRAY or GRAY_ALPHA */
      {
         /* expand background chunk. */
         switch (PP(png_byte, 0x127))
         {
            case 1:
               PP(png_uint_16, 0x140) *= (png_uint_16)0xff;
               PP(png_uint_16, 0x13a) = PP(png_uint_16, 0x13c) =
               PP(png_uint_16, 0x13e) = PP(png_uint_16, 0x140);
               break;
            case 2:
               PP(png_uint_16, 0x140) *= (png_uint_16)0x55;
               PP(png_uint_16, 0x13a) = PP(png_uint_16, 0x13c) =
               PP(png_uint_16, 0x13e) = PP(png_uint_16, 0x140);
               break;
            case 4:
               PP(png_uint_16, 0x140) *= (png_uint_16)0x11;
               PP(png_uint_16, 0x13a) = PP(png_uint_16, 0x13c) =
               PP(png_uint_16, 0x13e) = PP(png_uint_16, 0x140);
               break;
            case 8:
            case 16:
               PP(png_uint_16, 0x13a) = PP(png_uint_16, 0x13c) =
               PP(png_uint_16, 0x13e) = PP(png_uint_16, 0x140);
               break;
         }
      }
      else if (color_type == 3)
      {
         PP(png_uint_16, 0x13a)   =
            PP(png_colorp, 0x114)[PP(png_byte, 0x138)].red;
         PP(png_uint_16, 0x13c) =
            PP(png_colorp, 0x114)[PP(png_byte, 0x138)].green;
         PP(png_uint_16, 0x13e)  =
            PP(png_colorp, 0x114)[PP(png_byte, 0x138)].blue;

        if (PP(png_uint_32, 0x70) & 0x80000)
        {
           if (!(PP(png_uint_32, 0x70) & 0x1000))
           {
           /* invert the alpha channel (in tRNS) unless the pixels are
              going to be expanded, in which case leave it for later */
              int i,istop;
              istop=(int)PP(png_uint_16, 0x11a);
              for (i=0; i<istop; i++)
                 PP(png_bytep, 0x188)[i] = (png_byte)(255 - PP(png_bytep, 0x188)[i]);
           }
        }

      }
   }

   PP(png_color_16, 0x142) = PP(png_color_16, 0x138);
   if (PP(png_uint_32, 0x70) & (0x2000 | 0x600000))
   {
      FUN_0051c220(png_ptr);
      if (PP(png_uint_32, 0x70) & 0x0080)
      {
         if (color_type == 3)
         {
            png_color back, back_1;
            png_colorp palette = PP(png_colorp, 0x114);
            int num_palette = PP(png_uint_16, 0x118);
            int i;
            if (PP(png_byte, 0x130) == 2)
            {
               back.red = PP(png_bytep, 0x164)[PP(png_uint_16, 0x13a)];
               back.green = PP(png_bytep, 0x164)[PP(png_uint_16, 0x13c)];
               back.blue = PP(png_bytep, 0x164)[PP(png_uint_16, 0x13e)];

               back_1.red = PP(png_bytep, 0x16c)[PP(png_uint_16, 0x13a)];
               back_1.green = PP(png_bytep, 0x16c)[PP(png_uint_16, 0x13c)];
               back_1.blue = PP(png_bytep, 0x16c)[PP(png_uint_16, 0x13e)];
            }
            else
            {
               double g, gs;

               switch (PP(png_byte, 0x130))
               {
                  case 1:
                     g = (PP(float, 0x160));
                     gs = 1.0;
                     break;
                  case 2:
                     g = 1.0 / (PP(float, 0x15c));
                     gs = 1.0 / (PP(float, 0x15c) * PP(float, 0x160));
                     break;
                  case 3:
                     g = 1.0 / (PP(float, 0x134));
                     gs = 1.0 / (PP(float, 0x134) *
                                 PP(float, 0x160));
                     break;
                  default:
                     g = 1.0;    /* back_1 */
                     gs = 1.0;   /* back */
               }

               if ( fabs(gs - 1.0) < 0.05)
               {
                  back.red   = (png_byte)PP(png_uint_16, 0x13a);
                  back.green = (png_byte)PP(png_uint_16, 0x13c);
                  back.blue  = (png_byte)PP(png_uint_16, 0x13e);
               }
               else
               {
                  back.red = (png_byte)(pow(
                     (double)PP(png_uint_16, 0x13a)/255, gs) * 255.0 + .5);
                  back.green = (png_byte)(pow(
                     (double)PP(png_uint_16, 0x13c)/255, gs) * 255.0 + .5);
                  back.blue = (png_byte)(pow(
                     (double)PP(png_uint_16, 0x13e)/255, gs) * 255.0 + .5);
               }

               back_1.red = (png_byte)(pow(
                  (double)PP(png_uint_16, 0x13a)/255, g) * 255.0 + .5);
               back_1.green = (png_byte)(pow(
                  (double)PP(png_uint_16, 0x13c)/255, g) * 255.0 + .5);
               back_1.blue = (png_byte)(pow(
                  (double)PP(png_uint_16, 0x13e)/255, g) * 255.0 + .5);
            }
            for (i = 0; i < num_palette; i++)
            {
               if (i < (int)PP(png_uint_16, 0x11a) && PP(png_bytep, 0x188)[i] != 0xff)
               {
                  if (PP(png_bytep, 0x188)[i] == 0)
                  {
                     palette[i] = back;
                  }
                  else /* if (PP(png_bytep, 0x188)[i] != 0xff) */
                  {
                     png_byte v, w;

                     v = PP(png_bytep, 0x16c)[palette[i].red];
                     png_composite(w, v, PP(png_bytep, 0x188)[i], back_1.red);
                     palette[i].red = PP(png_bytep, 0x168)[w];

                     v = PP(png_bytep, 0x16c)[palette[i].green];
                     png_composite(w, v, PP(png_bytep, 0x188)[i], back_1.green);
                     palette[i].green = PP(png_bytep, 0x168)[w];

                     v = PP(png_bytep, 0x16c)[palette[i].blue];
                     png_composite(w, v, PP(png_bytep, 0x188)[i], back_1.blue);
                     palette[i].blue = PP(png_bytep, 0x168)[w];
                  }
               }
               else
               {
                  palette[i].red = PP(png_bytep, 0x164)[palette[i].red];
                  palette[i].green = PP(png_bytep, 0x164)[palette[i].green];
                  palette[i].blue = PP(png_bytep, 0x164)[palette[i].blue];
               }
            }
         }
         /* if (PP(png_byte, 0x130)!=PNG_BACKGROUND_GAMMA_UNKNOWN)*/
         else
         /* color_type != 3 */
         {
            double m = (double)(((png_uint_32)1 << PP(png_byte, 0x127)) - 1);
            double g = 1.0;
            double gs = 1.0;

            switch (PP(png_byte, 0x130))
            {
               case 1:
                  g = (PP(float, 0x160));
                  gs = 1.0;
                  break;
               case 2:
                  g = 1.0 / (PP(float, 0x15c));
                  gs = 1.0 / (PP(float, 0x15c) * PP(float, 0x160));
                  break;
               case 3:
                  g = 1.0 / (PP(float, 0x134));
                  gs = 1.0 / (PP(float, 0x134) *
                     PP(float, 0x160));
                  break;
            }

            if (color_type & 2)
            {
               /* RGB or RGBA */
               PP(png_uint_16, 0x144) = (png_uint_16)(pow(
                  (double)PP(png_uint_16, 0x13a) / m, g) * m + .5);
               PP(png_uint_16, 0x146) = (png_uint_16)(pow(
                  (double)PP(png_uint_16, 0x13c) / m, g) * m + .5);
               PP(png_uint_16, 0x148) = (png_uint_16)(pow(
                  (double)PP(png_uint_16, 0x13e) / m, g) * m + .5);
               PP(png_uint_16, 0x13a) = (png_uint_16)(pow(
                  (double)PP(png_uint_16, 0x13a) / m, gs) * m + .5);
               PP(png_uint_16, 0x13c) = (png_uint_16)(pow(
                  (double)PP(png_uint_16, 0x13c) / m, gs) * m + .5);
               PP(png_uint_16, 0x13e) = (png_uint_16)(pow(
                  (double)PP(png_uint_16, 0x13e) / m, gs) * m + .5);
            }
            else
            {
               /* GRAY or GRAY ALPHA */
               PP(png_uint_16, 0x14a) = (png_uint_16)(pow(
                  (double)PP(png_uint_16, 0x140) / m, g) * m + .5);
               PP(png_uint_16, 0x140) = (png_uint_16)(pow(
                  (double)PP(png_uint_16, 0x140) / m, gs) * m + .5);
            }
         }
      }
      else
      /* transformation does not include PNG_BACKGROUND */
      if (color_type == 3)
      {
         png_colorp palette = PP(png_colorp, 0x114);
         int num_palette = PP(png_uint_16, 0x118);
         int i;

         for (i = 0; i < num_palette; i++)
         {
            palette[i].red = PP(png_bytep, 0x164)[palette[i].red];
            palette[i].green = PP(png_bytep, 0x164)[palette[i].green];
            palette[i].blue = PP(png_bytep, 0x164)[palette[i].blue];
         }
      }
   }
   else
   /* No GAMMA transformation */
   if ((PP(png_uint_32, 0x70) & 0x0080) &&
       (color_type == 3))
   {
      int i;
      int istop = (int)PP(png_uint_16, 0x11a);
      png_color back;
      png_colorp palette = PP(png_colorp, 0x114);

      back.red   = (png_byte)PP(png_uint_16, 0x13a);
      back.green = (png_byte)PP(png_uint_16, 0x13c);
      back.blue  = (png_byte)PP(png_uint_16, 0x13e);

      for (i = 0; i < istop; i++)
      {
         if (PP(png_bytep, 0x188)[i] == 0)
         {
            palette[i] = back;
         }
         else if (PP(png_bytep, 0x188)[i] != 0xff)
         {
            /* The png_composite() macro is defined in png.h */
            png_composite(palette[i].red, palette[i].red,
               PP(png_bytep, 0x188)[i], back.red);
            png_composite(palette[i].green, palette[i].green,
               PP(png_bytep, 0x188)[i], back.green);
            png_composite(palette[i].blue, palette[i].blue,
               PP(png_bytep, 0x188)[i], back.blue);
         }
      }
   }

   if ((PP(png_uint_32, 0x70) & 0x0008) &&
      (color_type == 3))
   {
      png_uint_16 i;
      png_uint_16 istop = PP(png_uint_16, 0x118);
      int sr = 8 - PP(png_byte, 0x17c);
      int sg = 8 - PP(png_byte, 0x17d);
      int sb = 8 - PP(png_byte, 0x17e);

      if (sr < 0 || sr > 8)
         sr = 0;
      if (sg < 0 || sg > 8)
         sg = 0;
      if (sb < 0 || sb > 8)
         sb = 0;
      for (i = 0; i < istop; i++)
      {
         PP(png_colorp, 0x114)[i].red >>= sr;
         PP(png_colorp, 0x114)[i].green >>= sg;
         PP(png_colorp, 0x114)[i].blue >>= sb;
      }
   }
 }
}
