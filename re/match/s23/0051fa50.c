// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051fa50: libpng 1.0.12 pngwutil.c png_write_find_filter (PNG_WRITE_WEIGHTED_FILTER_SUPPORTED, no
   PNG_USE_ABS, no PNG_SLOW_PAETH), the official text run through the preprocessor, png_struct written as a
   padded struct with the game's offsets. In the
   second (non-weighted) Paeth loop c = *cp++ is read before b = *pp++ (permuter; independent loads).
   83.9% (regs 86.0, ops 98.3) (83.6% official order). Residual: register allocation in the filter loops. The
   original forms rp/dp start pointers with lea reg,[base+1] after loading the base into another register, ours
   loads straight into the target and incs; index operand order [edx+eax] vs [eax+edx]; the avg/paeth loops
   swap which of al/dl holds the two bytes. Tried: /GS (32.3%), permuter 1346 variants. Class (b). */
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
typedef unsigned char *png_bytep;
typedef unsigned int png_uint_32;
typedef int png_int_32;
typedef unsigned short png_uint_16;
typedef unsigned short *png_uint_16p;
typedef unsigned int png_size_t;
typedef struct png_row_info_struct { png_uint_32 width; png_uint_32 rowbytes; png_byte color_type; png_byte bit_depth;
   png_byte channels; png_byte pixel_depth; } png_row_info;
typedef png_row_info *png_row_infop;
typedef struct png_struct_def {
   char pad0[0xe8];
   png_bytep prev_row;          /* +0xe8 */
   png_bytep row_buf;           /* +0xec */
   png_bytep sub_row;           /* +0xf0 */
   png_bytep up_row;            /* +0xf4 */
   png_bytep avg_row;           /* +0xf8 */
   png_bytep paeth_row;         /* +0xfc */
   char pad1[0x125 - 0x100];
   png_byte do_filter;          /* +0x125 */
   char pad2[0x1ac - 0x126];
   png_byte heuristic_method;   /* +0x1ac */
   png_byte num_prev_filters;   /* +0x1ad */
   char pad3[2];
   png_bytep prev_filters;      /* +0x1b0 */
   png_uint_16p filter_weights; /* +0x1b4 */
   png_uint_16p inv_filter_weights; /* +0x1b8 */
   png_uint_16p filter_costs;   /* +0x1bc */
   png_uint_16p inv_filter_costs; /* +0x1c0 */
} png_struct;
typedef png_struct *png_structp;
void FUN_00520640(png_structp png_ptr, png_bytep filtered_row); /* png_write_filtered_row */
// MATCH: 0x0051fa50 FUN_0051fa50
void FUN_0051fa50(png_structp png_ptr, png_row_infop row_info)
{
   png_bytep prev_row, best_row, row_buf;
   png_uint_32 mins, bpp;
   png_byte filter_to_do = png_ptr->do_filter;
   png_uint_32 row_bytes = row_info->rowbytes;
   int num_p_filters = (int)png_ptr->num_prev_filters;
   ;
   bpp = (row_info->pixel_depth + 7) / 8;
   prev_row = png_ptr->prev_row;
   best_row = row_buf = png_ptr->row_buf;
   mins = (~((png_uint_32)0) >> 1);
   if ((filter_to_do & 0x08) &&
       filter_to_do != 0x08)
   {
      png_bytep rp;
      png_uint_32 sum = 0;
      png_uint_32 i;
      int v;
      for (i = 0, rp = row_buf + 1; i < row_bytes; i++, rp++)
      {
         v = *rp;
         sum += (v < 128) ? v : 256 - v;
      }
      if (png_ptr->heuristic_method == 2)
      {
         png_uint_32 sumhi, sumlo;
         int j;
         sumlo = sum & ((png_uint_32)0xffffL);
         sumhi = (sum >> 10) & ((png_uint_32)(~((png_uint_32)0xffffL) >> 10)); 
         for (j = 0; j < num_p_filters; j++)
         {
            if (png_ptr->prev_filters[j] == 0)
            {
               sumlo = (sumlo * png_ptr->filter_weights[j]) >>
                  8;
               sumhi = (sumhi * png_ptr->filter_weights[j]) >>
                  8;
            }
         }
         sumlo = (sumlo * png_ptr->filter_costs[0]) >>
            3;
         sumhi = (sumhi * png_ptr->filter_costs[0]) >>
            3;
         if (sumhi > ((png_uint_32)(~((png_uint_32)0xffffL) >> 10)))
            sum = (~((png_uint_32)0) >> 1);
         else
            sum = (sumhi << 10) + sumlo;
      }
      mins = sum;
   }
   if (filter_to_do == 0x10)
   {
      png_bytep rp, lp, dp;
      png_uint_32 i;
      for (i = 0, rp = row_buf + 1, dp = png_ptr->sub_row + 1; i < bpp;
           i++, rp++, dp++)
      {
         *dp = *rp;
      }
      for (lp = row_buf + 1; i < row_bytes;
         i++, rp++, lp++, dp++)
      {
         *dp = (png_byte)(((int)*rp - (int)*lp) & 0xff);
      }
      best_row = png_ptr->sub_row;
   }
   else if (filter_to_do & 0x10)
   {
      png_bytep rp, dp, lp;
      png_uint_32 sum = 0, lmins = mins;
      png_uint_32 i;
      int v;
      if (png_ptr->heuristic_method == 2)
      {
         int j;
         png_uint_32 lmhi, lmlo;
         lmlo = lmins & ((png_uint_32)0xffffL);
         lmhi = (lmins >> 10) & ((png_uint_32)(~((png_uint_32)0xffffL) >> 10));
         for (j = 0; j < num_p_filters; j++)
         {
            if (png_ptr->prev_filters[j] == 1)
            {
               lmlo = (lmlo * png_ptr->inv_filter_weights[j]) >>
                  8;
               lmhi = (lmhi * png_ptr->inv_filter_weights[j]) >>
                  8;
            }
         }
         lmlo = (lmlo * png_ptr->inv_filter_costs[1]) >>
            3;
         lmhi = (lmhi * png_ptr->inv_filter_costs[1]) >>
            3;
         if (lmhi > ((png_uint_32)(~((png_uint_32)0xffffL) >> 10)))
            lmins = (~((png_uint_32)0) >> 1);
         else
            lmins = (lmhi << 10) + lmlo;
      }
      for (i = 0, rp = row_buf + 1, dp = png_ptr->sub_row + 1; i < bpp;
           i++, rp++, dp++)
      {
         v = *dp = *rp;
         sum += (v < 128) ? v : 256 - v;
      }
      for (lp = row_buf + 1; i < row_info->rowbytes;
         i++, rp++, lp++, dp++)
      {
         v = *dp = (png_byte)(((int)*rp - (int)*lp) & 0xff);
         sum += (v < 128) ? v : 256 - v;
         if (sum > lmins)  
            break;
      }
      if (png_ptr->heuristic_method == 2)
      {
         int j;
         png_uint_32 sumhi, sumlo;
         sumlo = sum & ((png_uint_32)0xffffL);
         sumhi = (sum >> 10) & ((png_uint_32)(~((png_uint_32)0xffffL) >> 10));
         for (j = 0; j < num_p_filters; j++)
         {
            if (png_ptr->prev_filters[j] == 1)
            {
               sumlo = (sumlo * png_ptr->inv_filter_weights[j]) >>
                  8;
               sumhi = (sumhi * png_ptr->inv_filter_weights[j]) >>
                  8;
            }
         }
         sumlo = (sumlo * png_ptr->inv_filter_costs[1]) >>
            3;
         sumhi = (sumhi * png_ptr->inv_filter_costs[1]) >>
            3;
         if (sumhi > ((png_uint_32)(~((png_uint_32)0xffffL) >> 10)))
            sum = (~((png_uint_32)0) >> 1);
         else
            sum = (sumhi << 10) + sumlo;
      }
      if (sum < mins)
      {
         mins = sum;
         best_row = png_ptr->sub_row;
      }
   }
   if (filter_to_do == 0x20)
   {
      png_bytep rp, dp, pp;
      png_uint_32 i;
      for (i = 0, rp = row_buf + 1, dp = png_ptr->up_row + 1,
           pp = prev_row + 1; i < row_bytes;
           i++, rp++, pp++, dp++)
      {
         *dp = (png_byte)(((int)*rp - (int)*pp) & 0xff);
      }
      best_row = png_ptr->up_row;
   }
   else if (filter_to_do & 0x20)
   {
      png_bytep rp, dp, pp;
      png_uint_32 sum = 0, lmins = mins;
      png_uint_32 i;
      int v;
      if (png_ptr->heuristic_method == 2)
      {
         int j;
         png_uint_32 lmhi, lmlo;
         lmlo = lmins & ((png_uint_32)0xffffL);
         lmhi = (lmins >> 10) & ((png_uint_32)(~((png_uint_32)0xffffL) >> 10));
         for (j = 0; j < num_p_filters; j++)
         {
            if (png_ptr->prev_filters[j] == 2)
            {
               lmlo = (lmlo * png_ptr->inv_filter_weights[j]) >>
                  8;
               lmhi = (lmhi * png_ptr->inv_filter_weights[j]) >>
                  8;
            }
         }
         lmlo = (lmlo * png_ptr->inv_filter_costs[2]) >>
            3;
         lmhi = (lmhi * png_ptr->inv_filter_costs[2]) >>
            3;
         if (lmhi > ((png_uint_32)(~((png_uint_32)0xffffL) >> 10)))
            lmins = (~((png_uint_32)0) >> 1);
         else
            lmins = (lmhi << 10) + lmlo;
      }
      for (i = 0, rp = row_buf + 1, dp = png_ptr->up_row + 1,
           pp = prev_row + 1; i < row_bytes; i++)
      {
         v = *dp++ = (png_byte)(((int)*rp++ - (int)*pp++) & 0xff);
         sum += (v < 128) ? v : 256 - v;
         if (sum > lmins)  
            break;
      }
      if (png_ptr->heuristic_method == 2)
      {
         int j;
         png_uint_32 sumhi, sumlo;
         sumlo = sum & ((png_uint_32)0xffffL);
         sumhi = (sum >> 10) & ((png_uint_32)(~((png_uint_32)0xffffL) >> 10));
         for (j = 0; j < num_p_filters; j++)
         {
            if (png_ptr->prev_filters[j] == 2)
            {
               sumlo = (sumlo * png_ptr->filter_weights[j]) >>
                  8;
               sumhi = (sumhi * png_ptr->filter_weights[j]) >>
                  8;
            }
         }
         sumlo = (sumlo * png_ptr->filter_costs[2]) >>
            3;
         sumhi = (sumhi * png_ptr->filter_costs[2]) >>
            3;
         if (sumhi > ((png_uint_32)(~((png_uint_32)0xffffL) >> 10)))
            sum = (~((png_uint_32)0) >> 1);
         else
            sum = (sumhi << 10) + sumlo;
      }
      if (sum < mins)
      {
         mins = sum;
         best_row = png_ptr->up_row;
      }
   }
   if (filter_to_do == 0x40)
   {
      png_bytep rp, dp, pp, lp;
      png_uint_32 i;
      for (i = 0, rp = row_buf + 1, dp = png_ptr->avg_row + 1,
           pp = prev_row + 1; i < bpp; i++)
      {
         *dp++ = (png_byte)(((int)*rp++ - ((int)*pp++ / 2)) & 0xff);
      }
      for (lp = row_buf + 1; i < row_bytes; i++)
      {
         *dp++ = (png_byte)(((int)*rp++ - (((int)*pp++ + (int)*lp++) / 2))
                 & 0xff);
      }
      best_row = png_ptr->avg_row;
   }
   else if (filter_to_do & 0x40)
   {
      png_bytep rp, dp, pp, lp;
      png_uint_32 sum = 0, lmins = mins;
      png_uint_32 i;
      int v;
      if (png_ptr->heuristic_method == 2)
      {
         int j;
         png_uint_32 lmhi, lmlo;
         lmlo = lmins & ((png_uint_32)0xffffL);
         lmhi = (lmins >> 10) & ((png_uint_32)(~((png_uint_32)0xffffL) >> 10));
         for (j = 0; j < num_p_filters; j++)
         {
            if (png_ptr->prev_filters[j] == 3)
            {
               lmlo = (lmlo * png_ptr->inv_filter_weights[j]) >>
                  8;
               lmhi = (lmhi * png_ptr->inv_filter_weights[j]) >>
                  8;
            }
         }
         lmlo = (lmlo * png_ptr->inv_filter_costs[3]) >>
            3;
         lmhi = (lmhi * png_ptr->inv_filter_costs[3]) >>
            3;
         if (lmhi > ((png_uint_32)(~((png_uint_32)0xffffL) >> 10)))
            lmins = (~((png_uint_32)0) >> 1);
         else
            lmins = (lmhi << 10) + lmlo;
      }
      for (i = 0, rp = row_buf + 1, dp = png_ptr->avg_row + 1,
           pp = prev_row + 1; i < bpp; i++)
      {
         v = *dp++ = (png_byte)(((int)*rp++ - ((int)*pp++ / 2)) & 0xff);
         sum += (v < 128) ? v : 256 - v;
      }
      for (lp = row_buf + 1; i < row_bytes; i++)
      {
         v = *dp++ =
          (png_byte)(((int)*rp++ - (((int)*pp++ + (int)*lp++) / 2)) & 0xff);
         sum += (v < 128) ? v : 256 - v;
         if (sum > lmins)  
            break;
      }
      if (png_ptr->heuristic_method == 2)
      {
         int j;
         png_uint_32 sumhi, sumlo;
         sumlo = sum & ((png_uint_32)0xffffL);
         sumhi = (sum >> 10) & ((png_uint_32)(~((png_uint_32)0xffffL) >> 10));
         for (j = 0; j < num_p_filters; j++)
         {
            if (png_ptr->prev_filters[j] == 0)
            {
               sumlo = (sumlo * png_ptr->filter_weights[j]) >>
                  8;
               sumhi = (sumhi * png_ptr->filter_weights[j]) >>
                  8;
            }
         }
         sumlo = (sumlo * png_ptr->filter_costs[3]) >>
            3;
         sumhi = (sumhi * png_ptr->filter_costs[3]) >>
            3;
         if (sumhi > ((png_uint_32)(~((png_uint_32)0xffffL) >> 10)))
            sum = (~((png_uint_32)0) >> 1);
         else
            sum = (sumhi << 10) + sumlo;
      }
      if (sum < mins)
      {
         mins = sum;
         best_row = png_ptr->avg_row;
      }
   }
   if (filter_to_do == 0x80)
   {
      png_bytep rp, dp, pp, cp, lp;
      png_uint_32 i;
      for (i = 0, rp = row_buf + 1, dp = png_ptr->paeth_row + 1,
           pp = prev_row + 1; i < bpp; i++)
      {
         *dp++ = (png_byte)(((int)*rp++ - (int)*pp++) & 0xff);
      }
      for (lp = row_buf + 1, cp = prev_row + 1; i < row_bytes; i++)
      {
         int a, b, c, pa, pb, pc, p;
         b = *pp++;
         c = *cp++;
         a = *lp++;
         p = b - c;
         pc = a - c;
         pa = p < 0 ? -p : p;
         pb = pc < 0 ? -pc : pc;
         pc = (p + pc) < 0 ? -(p + pc) : p + pc;
         p = (pa <= pb && pa <=pc) ? a : (pb <= pc) ? b : c;
         *dp++ = (png_byte)(((int)*rp++ - p) & 0xff);
      }
      best_row = png_ptr->paeth_row;
   }
   else if (filter_to_do & 0x80)
   {
      png_bytep rp, dp, pp, cp, lp;
      png_uint_32 sum = 0, lmins = mins;
      png_uint_32 i;
      int v;
      if (png_ptr->heuristic_method == 2)
      {
         int j;
         png_uint_32 lmhi, lmlo;
         lmlo = lmins & ((png_uint_32)0xffffL);
         lmhi = (lmins >> 10) & ((png_uint_32)(~((png_uint_32)0xffffL) >> 10));
         for (j = 0; j < num_p_filters; j++)
         {
            if (png_ptr->prev_filters[j] == 4)
            {
               lmlo = (lmlo * png_ptr->inv_filter_weights[j]) >>
                  8;
               lmhi = (lmhi * png_ptr->inv_filter_weights[j]) >>
                  8;
            }
         }
         lmlo = (lmlo * png_ptr->inv_filter_costs[4]) >>
            3;
         lmhi = (lmhi * png_ptr->inv_filter_costs[4]) >>
            3;
         if (lmhi > ((png_uint_32)(~((png_uint_32)0xffffL) >> 10)))
            lmins = (~((png_uint_32)0) >> 1);
         else
            lmins = (lmhi << 10) + lmlo;
      }
      for (i = 0, rp = row_buf + 1, dp = png_ptr->paeth_row + 1,
           pp = prev_row + 1; i < bpp; i++)
      {
         v = *dp++ = (png_byte)(((int)*rp++ - (int)*pp++) & 0xff);
         sum += (v < 128) ? v : 256 - v;
      }
      for (lp = row_buf + 1, cp = prev_row + 1; i < row_bytes; i++)
      {
         int a, b, c, pa, pb, pc, p;
         c = *cp++;
         b = *pp++;
         a = *lp++;
         p = b - c;
         pc = a - c;
         pa = p < 0 ? -p : p;
         pb = pc < 0 ? -pc : pc;
         pc = (p + pc) < 0 ? -(p + pc) : p + pc;
         p = (pa <= pb && pa <=pc) ? a : (pb <= pc) ? b : c;
         v = *dp++ = (png_byte)(((int)*rp++ - p) & 0xff);
         sum += (v < 128) ? v : 256 - v;
         if (sum > lmins)  
            break;
      }
      if (png_ptr->heuristic_method == 2)
      {
         int j;
         png_uint_32 sumhi, sumlo;
         sumlo = sum & ((png_uint_32)0xffffL);
         sumhi = (sum >> 10) & ((png_uint_32)(~((png_uint_32)0xffffL) >> 10));
         for (j = 0; j < num_p_filters; j++)
         {
            if (png_ptr->prev_filters[j] == 4)
            {
               sumlo = (sumlo * png_ptr->filter_weights[j]) >>
                  8;
               sumhi = (sumhi * png_ptr->filter_weights[j]) >>
                  8;
            }
         }
         sumlo = (sumlo * png_ptr->filter_costs[4]) >>
            3;
         sumhi = (sumhi * png_ptr->filter_costs[4]) >>
            3;
         if (sumhi > ((png_uint_32)(~((png_uint_32)0xffffL) >> 10)))
            sum = (~((png_uint_32)0) >> 1);
         else
            sum = (sumhi << 10) + sumlo;
      }
      if (sum < mins)
      {
         best_row = png_ptr->paeth_row;
      }
   }
   FUN_00520640(png_ptr, best_row);
   if (png_ptr->num_prev_filters > 0)
   {
      int j;
      for (j = 1; j < num_p_filters; j++)
      {
         png_ptr->prev_filters[j] = png_ptr->prev_filters[j - 1];
      }
      png_ptr->prev_filters[j] = best_row[0];
   }
}
