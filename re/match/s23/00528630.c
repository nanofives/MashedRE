// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00528630: 100% (regs 100%). libpng 1.0.12 pngrutil.c png_read_filter_row, official body (png_row_info
   layout is the official one: rowbytes +4, pixel_depth +0xb). What moved it (72.4 -> 100): png_ptr, row_info, row
   and prev_row copied from *_arg parameters into locals at the top; the copies let the allocator keep the Paeth
   loop's spills in the dead argument slots instead of opening a frame (h1's row_arg finding, generalized: all four
   copies are needed, any three give 72.4-73.4). Residual: none. */
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
typedef unsigned char png_byte;
typedef png_byte *png_bytep;
typedef unsigned long png_uint_32;
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
typedef struct png_struct_def *png_structp;

MGLOBALA(char, s_Ignoring_bad_adaptive_filter_typ_006227f0, 34, 0x006227f0);
void FUN_00517270(png_structp png_ptr, const char *message);

// MATCH: 0x00528630 FUN_00528630
void
FUN_00528630(png_structp png_ptr_arg, png_row_infop row_info_arg, png_bytep row_arg,
   png_bytep prev_row_arg, int filter)
{
   png_structp png_ptr = png_ptr_arg;
   png_row_infop row_info = row_info_arg;
   png_bytep row = row_arg;
   png_bytep prev_row = prev_row_arg;
   switch (filter)
   {
      case 0:
         break;
      case 1:
      {
         png_uint_32 i;
         png_uint_32 istop = row_info->rowbytes;
         png_uint_32 bpp = (row_info->pixel_depth + 7) >> 3;
         png_bytep rp = row + bpp;
         png_bytep lp = row;

         for (i = bpp; i < istop; i++)
         {
            *rp = (png_byte)(((int)(*rp) + (int)(*lp++)) & 0xff);
            rp++;
         }
         break;
      }
      case 2:
      {
         png_uint_32 i;
         png_uint_32 istop = row_info->rowbytes;
         png_bytep rp = row;
         png_bytep pp = prev_row;

         for (i = 0; i < istop; i++)
         {
            *rp = (png_byte)(((int)(*rp) + (int)(*pp++)) & 0xff);
            rp++;
         }
         break;
      }
      case 3:
      {
         png_uint_32 i;
         png_bytep rp = row;
         png_bytep pp = prev_row;
         png_bytep lp = row;
         png_uint_32 bpp = (row_info->pixel_depth + 7) >> 3;
         png_uint_32 istop = row_info->rowbytes - bpp;

         for (i = 0; i < bpp; i++)
         {
            *rp = (png_byte)(((int)(*rp) +
               ((int)(*pp++) / 2 )) & 0xff);
            rp++;
         }

         for (i = 0; i < istop; i++)
         {
            *rp = (png_byte)(((int)(*rp) +
               (int)(*pp++ + *lp++) / 2 ) & 0xff);
            rp++;
         }
         break;
      }
      case 4:
      {
         png_uint_32 i;
         png_bytep rp = row;
         png_bytep pp = prev_row;
         png_bytep lp = row;
         png_bytep cp = prev_row;
         png_uint_32 bpp = (row_info->pixel_depth + 7) >> 3;
         png_uint_32 istop=row_info->rowbytes - bpp;

         for (i = 0; i < bpp; i++)
         {
            *rp = (png_byte)(((int)(*rp) + (int)(*pp++)) & 0xff);
            rp++;
         }

         for (i = 0; i < istop; i++)   /* use leftover rp,pp */
         {
            int a, b, c, pa, pb, pc, p;

            a = *lp++;
            b = *pp++;
            c = *cp++;

            p = b - c;
            pc = a - c;

#ifdef PNG_USE_ABS
            pa = abs(p);
            pb = abs(pc);
            pc = abs(p + pc);
#else
            pa = p < 0 ? -p : p;
            pb = pc < 0 ? -pc : pc;
            pc = (p + pc) < 0 ? -(p + pc) : p + pc;
#endif

            /*
               if (pa <= pb && pa <= pc)
                  p = a;
               else if (pb <= pc)
                  p = b;
               else
                  p = c;
             */

            p = (pa <= pb && pa <=pc) ? a : (pb <= pc) ? b : c;

            *rp = (png_byte)(((int)(*rp) + p) & 0xff);
            rp++;
         }
         break;
      }
      default:
         FUN_00517270(png_ptr, s_Ignoring_bad_adaptive_filter_typ_006227f0);
         *row=0;
         break;
   }
}
