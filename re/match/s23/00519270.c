// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00519270: libpng 1.0.12 pngrtran.c png_do_read_transformations. png_do_chop (defined later in
   the file) is inlined in the original: written inline here as a block. png_struct offsets from the decomp
   (transformations +0x70, row_buf +0xec, row_info +0x100). */
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
MGLOBALA(char, s_png_do_dither_returned_rowbytes__006215fc, 34, 0x006215fc);
MGLOBALA(char, s_png_do_rgb_to_gray_found_nongray_00621620, 39, 0x00621620);
MGLOBALA(char, s_NULL_row_buffer_00621648, 16, 0x00621648);
typedef unsigned int png_uint_32;
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
#define PP(T, off) (*(T *)((char *)png_ptr + (off)))
#define ROW_INFO ((png_row_infop)((char *)png_ptr + 0x100))
#define ROW_BUF (PP(png_bytep, 0xec))
typedef void (*png_user_transform_ptr)(void *, png_row_infop, png_bytep);
void FUN_00517250(void *png_ptr, const char *msg);
void FUN_00517270(void *png_ptr, const char *msg);
void FUN_0051bb40(png_row_infop row_info, png_bytep row, void *palette, png_bytep trans, int num_trans);
void FUN_0051bd70(png_row_infop row_info, png_bytep row, void *trans_value);
void FUN_005157c0(png_row_infop row_info, png_bytep row, png_uint_32 flags);
int FUN_00519f70(void *png_ptr, png_row_infop row_info, png_bytep row);
void FUN_00519e30(png_row_infop row_info, png_bytep row);
void FUN_0051a6c0(png_row_infop row_info, png_bytep row, void *trans_values, void *background,
   void *background_1, png_bytep gamma_table, png_bytep gamma_from_1, png_bytep gamma_to_1,
   void *gamma_16, void *gamma_16_from_1, void *gamma_16_to_1, int gamma_shift);
void FUN_0051b750(png_row_infop row_info, png_bytep row, png_bytep gamma_table, void *gamma_16_table,
   int gamma_shift);
void FUN_0051c100(png_row_infop row_info, png_bytep row, png_bytep palette_lookup, png_bytep dither_lookup);
void FUN_00515710(png_row_infop row_info, png_bytep row);
void FUN_00519820(png_row_infop row_info, png_bytep row, void *sig_bits);
void FUN_005196f0(png_row_infop row_info, png_bytep row);
void FUN_005159d0(png_row_infop row_info, png_bytep row);
void FUN_00515770(png_row_infop row_info, png_bytep row);
void FUN_00519bc0(png_row_infop row_info, png_bytep row, png_uint_32 filler, png_uint_32 flags);
void FUN_00519af0(png_row_infop row_info, png_bytep row);
void FUN_005199f0(png_row_infop row_info, png_bytep row);
void FUN_00515740(png_row_infop row_info, png_bytep row);
// MATCH: 0x00519270 FUN_00519270
void FUN_00519270(void *png_ptr)
{
   if (ROW_BUF == 0)
   {
      FUN_00517250(png_ptr, s_NULL_row_buffer_00621648);
   }

   if (PP(png_uint_32, 0x70) & 0x1000)
   {
      if (ROW_INFO->color_type == 3)
      {
         FUN_0051bb40(ROW_INFO, ROW_BUF + 1,
            PP(void *, 0x114), PP(png_bytep, 0x188), PP(unsigned short, 0x11a));
      }
      else
      {
         if (PP(unsigned short, 0x11a))
            FUN_0051bd70(ROW_INFO, ROW_BUF + 1,
               (char *)png_ptr + 0x18c);
         else
            FUN_0051bd70(ROW_INFO, ROW_BUF + 1,
               0);
      }
   }

   if (PP(png_uint_32, 0x70) & 0x40000)
      FUN_005157c0(ROW_INFO, ROW_BUF + 1,
         0x0080);

   if (PP(png_uint_32, 0x70) & 0x600000)
   {
      int rgb_error =
         FUN_00519f70(png_ptr, ROW_INFO, ROW_BUF + 1);
      if(rgb_error)
      {
         PP(png_byte, 0x1d8)=1;
         if(PP(png_uint_32, 0x70) == 0x400000)
            FUN_00517270(png_ptr, s_png_do_rgb_to_gray_found_nongray_00621620);
         if(PP(png_uint_32, 0x70) == 0x200000)
            FUN_00517250(png_ptr, s_png_do_rgb_to_gray_found_nongray_00621620);
      }
   }

   if ((PP(png_uint_32, 0x70) & 0x4000) &&
       !(PP(png_uint_32, 0x68) & 0x800))
      FUN_00519e30(ROW_INFO, ROW_BUF + 1);

   if ((PP(png_uint_32, 0x70) & 0x0080) &&
      ((PP(unsigned short, 0x11a) != 0 ) ||
      (PP(png_byte, 0x126) & 4)))
      FUN_0051a6c0(ROW_INFO, ROW_BUF + 1,
         (char *)png_ptr + 0x18c, (char *)png_ptr + 0x138,
         (char *)png_ptr + 0x142,
         PP(png_bytep, 0x164), PP(png_bytep, 0x168),
         PP(png_bytep, 0x16c), PP(void *, 0x170),
         PP(void *, 0x174), PP(void *, 0x178),
         PP(int, 0x158));

   if ((PP(png_uint_32, 0x70) & 0x2000) &&
      !((PP(png_uint_32, 0x70) & 0x0080) &&
      ((PP(unsigned short, 0x11a) != 0) ||
      (PP(png_byte, 0x126) & 4))) &&
      (PP(png_byte, 0x126) != 3))
      FUN_0051b750(ROW_INFO, ROW_BUF + 1,
         PP(png_bytep, 0x164), PP(void *, 0x170),
         PP(int, 0x158));

   if (PP(png_uint_32, 0x70) & 0x0400)
   {
      png_row_infop row_info = ROW_INFO;
      png_bytep row = ROW_BUF + 1;
      if (row_info->bit_depth == 16)
      {
         png_bytep sp = row;
         png_bytep dp = row;
         png_uint_32 i;
         png_uint_32 istop = row_info->width * row_info->channels;

         for (i = 0; i<istop; i++, sp += 2, dp++)
         {
            *dp = *sp;
         }
         row_info->bit_depth = 8;
         row_info->pixel_depth = (png_byte)(8 * row_info->channels);
         row_info->rowbytes = row_info->width * row_info->channels;
      }
   }

   if (PP(png_uint_32, 0x70) & 0x0040)
   {
      FUN_0051c100(ROW_INFO, ROW_BUF + 1,
         PP(png_bytep, 0x1a0), PP(png_bytep, 0x1a4));
      if(ROW_INFO->rowbytes == (png_uint_32)0)
         FUN_00517250(png_ptr, s_png_do_dither_returned_rowbytes__006215fc);
   }

   if (PP(png_uint_32, 0x70) & 0x0020)
      FUN_00515710(ROW_INFO, ROW_BUF + 1);

   if (PP(png_uint_32, 0x70) & 0x0008)
      FUN_00519820(ROW_INFO, ROW_BUF + 1,
         (char *)png_ptr + 0x181);

   if (PP(png_uint_32, 0x70) & 0x0004)
      FUN_005196f0(ROW_INFO, ROW_BUF + 1);

   if (PP(png_uint_32, 0x70) & 0x0001)
      FUN_005159d0(ROW_INFO, ROW_BUF + 1);

   if (PP(png_uint_32, 0x70) & 0x10000)
      FUN_00515770(ROW_INFO, ROW_BUF + 1);

   if ((PP(png_uint_32, 0x70) & 0x4000) &&
       (PP(png_uint_32, 0x68) & 0x800))
      FUN_00519e30(ROW_INFO, ROW_BUF + 1);

   if (PP(png_uint_32, 0x70) & 0x8000)
      FUN_00519bc0(ROW_INFO, ROW_BUF + 1,
         (png_uint_32)PP(unsigned short, 0x12e), PP(png_uint_32, 0x6c));

   if (PP(png_uint_32, 0x70) & 0x80000)
      FUN_00519af0(ROW_INFO, ROW_BUF + 1);

   if (PP(png_uint_32, 0x70) & 0x20000)
      FUN_005199f0(ROW_INFO, ROW_BUF + 1);

   if (PP(png_uint_32, 0x70) & 0x0010)
      FUN_00515740(ROW_INFO, ROW_BUF + 1);

   if (PP(png_uint_32, 0x70) & 0x100000)
    {
      if(PP(png_user_transform_ptr, 0x58) != 0)
        (*(PP(png_user_transform_ptr, 0x58)))
          (png_ptr,
           ROW_INFO,
           ROW_BUF + 1);
      if(PP(png_byte, 0x64))
         ROW_INFO->bit_depth = PP(png_byte, 0x64);
      if(PP(png_byte, 0x65))
         ROW_INFO->channels = PP(png_byte, 0x65);
      ROW_INFO->pixel_depth = (png_byte)(ROW_INFO->bit_depth *
         ROW_INFO->channels);
      ROW_INFO->rowbytes = (ROW_INFO->width *
         ROW_INFO->pixel_depth+7)>>3;
   }

}
