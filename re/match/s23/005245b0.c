// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x005245b0: libpng 1.0.12 pngrutil.c png_handle_cHRM (floating + fixed point, sRGB check, no
   console io). png_crc_finish per call site as the original's compiler did: inlined with png_crc_error called
   (png_crc_finish_e) for the IDAT / Duplicate / length / white / red / green paths, called (0x00523170) for the
   blue / sRGB / final paths; png_crc_read and png_get_uint_32 inlined everywhere. white_x is computed after red_y (permuter; pure
   float arithmetic, same values).
   85.5% (regs 86.0, ops 99.4) (80.6% with the official order). Residual: frame 0x28 vs 0x20. The original
   keeps two of the eight png_fixed_point values in the dead png_ptr / length arg slots, ours gives them frame
   slots, so most [esp+N] operands differ by 8. Same stack-packing family as 0x00523360 / 0x00523e40. Tried:
   permuter 234 variants. Class (b). */
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
typedef unsigned int png_size_t;
typedef char *png_structp;
MGLOBALA(char, s_CRC_error_00621ce8, 10, 0x00621ce8);
void FUN_0051c9f0(png_structp png_ptr, png_bytep data, png_size_t length); /* png_read_data */
void FUN_00516d50(png_structp png_ptr, png_bytep ptr, png_size_t length); /* png_calculate_crc */
void FUN_00517380(png_structp png_ptr, const char *message); /* png_chunk_warning */
void FUN_005172b0(png_structp png_ptr, const char *message); /* png_chunk_error */

static __forceinline void png_crc_read(png_structp png_ptr, png_bytep buf, png_size_t length)
{
   FUN_0051c9f0(png_ptr, buf, length);
   FUN_00516d50(png_ptr, buf, length);
}

static __forceinline int png_crc_error(png_structp png_ptr)
{
   png_byte crc_bytes[4];
   png_uint_32 crc;
   int need_crc = 1;

   if (*(png_byte *)(png_ptr + 0x11c) & 0x20)
   {
      if ((*(png_uint_32 *)(png_ptr + 0x6c) & 0x300) == (0x100 | 0x200))
         need_crc = 0;
   }
   else
   {
      if (*(png_uint_32 *)(png_ptr + 0x6c) & 0x800)
         need_crc = 0;
   }

   FUN_0051c9f0(png_ptr, crc_bytes, 4);

   if (need_crc)
   {
      crc = ((png_uint_32)(*crc_bytes) << 24) +
         ((png_uint_32)(*(crc_bytes + 1)) << 16) +
         ((png_uint_32)(*(crc_bytes + 2)) << 8) +
         (png_uint_32)(*(crc_bytes + 3));
      return ((int)(crc != *(png_uint_32 *)(png_ptr + 0x110)));
   }
   else
      return (0);
}

static __forceinline int png_crc_finish(png_structp png_ptr, png_uint_32 skip)
{
   png_size_t i;
   png_size_t istop = *(png_size_t *)(png_ptr + 0xb0);

   for (i = (png_size_t)skip; i > istop; i -= istop)
   {
      png_crc_read(png_ptr, *(png_bytep *)(png_ptr + 0xac), *(png_size_t *)(png_ptr + 0xb0));
   }
   if (i)
   {
      png_crc_read(png_ptr, *(png_bytep *)(png_ptr + 0xac), i);
   }

   if (png_crc_error(png_ptr))
   {
      if (((*(png_byte *)(png_ptr + 0x11c) & 0x20) &&
           !(*(png_uint_32 *)(png_ptr + 0x6c) & 0x200)) ||
          (!(*(png_byte *)(png_ptr + 0x11c) & 0x20) &&
          (*(png_uint_32 *)(png_ptr + 0x6c) & 0x400)))
      {
         FUN_00517380(png_ptr, s_CRC_error_00621ce8);
      }
      else
      {
         FUN_005172b0(png_ptr, s_CRC_error_00621ce8);
      }
      return (1);
   }

   return (0);
}

int FUN_005232c0(png_structp png_ptr); /* png_crc_error */
png_uint_32 FUN_00523110(png_bytep buf); /* png_get_uint_32 */

static __forceinline int png_crc_error_g(png_structp png_ptr)
{
   png_byte crc_bytes[4];
   png_uint_32 crc;
   int need_crc = 1;

   if (*(png_byte *)(png_ptr + 0x11c) & 0x20)
   {
      if ((*(png_uint_32 *)(png_ptr + 0x6c) & 0x300) == (0x100 | 0x200))
         need_crc = 0;
   }
   else
   {
      if (*(png_uint_32 *)(png_ptr + 0x6c) & 0x800)
         need_crc = 0;
   }

   FUN_0051c9f0(png_ptr, crc_bytes, 4);

   if (need_crc)
   {
      crc = FUN_00523110(crc_bytes);
      return ((int)(crc != *(png_uint_32 *)(png_ptr + 0x110)));
   }
   else
      return (0);
}

static __forceinline int png_crc_finish_e(png_structp png_ptr, png_uint_32 skip)
{
   png_size_t i;
   png_size_t istop = *(png_size_t *)(png_ptr + 0xb0);

   for (i = (png_size_t)skip; i > istop; i -= istop)
   {
      png_crc_read(png_ptr, *(png_bytep *)(png_ptr + 0xac), *(png_size_t *)(png_ptr + 0xb0));
   }
   if (i)
   {
      png_crc_read(png_ptr, *(png_bytep *)(png_ptr + 0xac), i);
   }

   if (FUN_005232c0(png_ptr))
   {
      if (((*(png_byte *)(png_ptr + 0x11c) & 0x20) &&
           !(*(png_uint_32 *)(png_ptr + 0x6c) & 0x200)) ||
          (!(*(png_byte *)(png_ptr + 0x11c) & 0x20) &&
          (*(png_uint_32 *)(png_ptr + 0x6c) & 0x400)))
      {
         FUN_00517380(png_ptr, s_CRC_error_00621ce8);
      }
      else
      {
         FUN_005172b0(png_ptr, s_CRC_error_00621ce8);
      }
      return (1);
   }

   return (0);
}

static __forceinline int png_crc_finish_i(png_structp png_ptr, png_uint_32 skip)
{
   png_size_t i;
   png_size_t istop = *(png_size_t *)(png_ptr + 0xb0);

   for (i = (png_size_t)skip; i > istop; i -= istop)
   {
      png_crc_read(png_ptr, *(png_bytep *)(png_ptr + 0xac), *(png_size_t *)(png_ptr + 0xb0));
   }
   if (i)
   {
      png_crc_read(png_ptr, *(png_bytep *)(png_ptr + 0xac), i);
   }

   if (png_crc_error_g(png_ptr))
   {
      if (((*(png_byte *)(png_ptr + 0x11c) & 0x20) &&
           !(*(png_uint_32 *)(png_ptr + 0x6c) & 0x200)) ||
          (!(*(png_byte *)(png_ptr + 0x11c) & 0x20) &&
          (*(png_uint_32 *)(png_ptr + 0x6c) & 0x400)))
      {
         FUN_00517380(png_ptr, s_CRC_error_00621ce8);
      }
      else
      {
         FUN_005172b0(png_ptr, s_CRC_error_00621ce8);
      }
      return (1);
   }

   return (0);
}
MGLOBALA(char, s_Missing_IHDR_before_cHRM_006220ec, 25, 0x006220ec);
MGLOBALA(char, s_Invalid_cHRM_after_IDAT_006220b8, 24, 0x006220b8);
MGLOBALA(char, s_Missing_PLTE_before_cHRM_0062209c, 25, 0x0062209c);
MGLOBALA(char, s_Duplicate_cHRM_chunk_00622084, 21, 0x00622084);
MGLOBALA(char, s_Incorrect_cHRM_chunk_length_006220d0, 28, 0x006220d0);
MGLOBALA(char, s_Invalid_cHRM_white_point_00621fe4, 25, 0x00621fe4);
MGLOBALA(char, s_Invalid_cHRM_red_point_00622000, 23, 0x00622000);
MGLOBALA(char, s_Invalid_cHRM_green_point_00622018, 25, 0x00622018);
MGLOBALA(char, s_Invalid_cHRM_blue_point_00622034, 24, 0x00622034);
MGLOBALA(char, s_Ignoring_incorrect_cHRM_value_0062204c, 56, 0x0062204c);
typedef int png_fixed_point;
typedef struct png_info_head_s { png_uint_32 width; png_uint_32 height; png_uint_32 valid; } png_info;
typedef png_info *png_infop;
int abs(int);
#pragma intrinsic(abs)
void FUN_00517250(png_structp png_ptr, const char *message); /* png_error */
void FUN_00517270(png_structp png_ptr, const char *message); /* png_warning */
int FUN_00523170(png_structp png_ptr, png_uint_32 skip); /* png_crc_finish */
void FUN_00515ae0(png_structp png_ptr, png_infop info_ptr, double white_x, double white_y, double red_x,
   double red_y, double green_x, double green_y, double blue_x, double blue_y); /* png_set_cHRM */
void FUN_00515d50(png_structp png_ptr, png_infop info_ptr, png_fixed_point int_white_x,
   png_fixed_point int_white_y, png_fixed_point int_red_x, png_fixed_point int_red_y, png_fixed_point int_green_x,
   png_fixed_point int_green_y, png_fixed_point int_blue_x, png_fixed_point int_blue_y); /* png_set_cHRM_fixed */

static __forceinline png_uint_32 png_get_uint_32(png_bytep buf)
{
   png_uint_32 i = ((png_uint_32)(*buf) << 24) +
      ((png_uint_32)(*(buf + 1)) << 16) +
      ((png_uint_32)(*(buf + 2)) << 8) +
      (png_uint_32)(*(buf + 3));

   return (i);
}

// MATCH: 0x005245b0 FUN_005245b0
void FUN_005245b0(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_byte buf[4];
   float white_x, white_y, red_x, red_y, green_x, green_y, blue_x, blue_y;
   png_fixed_point int_x_white, int_y_white, int_x_red, int_y_red, int_x_green,
      int_y_green, int_x_blue, int_y_blue;


   if (!(*(png_uint_32 *)(png_ptr + 0x68) & 1))
      FUN_00517250(png_ptr, s_Missing_IHDR_before_cHRM_006220ec);
   else if (*(png_uint_32 *)(png_ptr + 0x68) & 4)
   {
      FUN_00517270(png_ptr, s_Invalid_cHRM_after_IDAT_006220b8);
      png_crc_finish_e(png_ptr, length);
      return;
   }
   else if (*(png_uint_32 *)(png_ptr + 0x68) & 2)
      
      FUN_00517270(png_ptr, s_Missing_PLTE_before_cHRM_0062209c);

   else if (info_ptr != 0 && (info_ptr->valid & 4)
      && !(info_ptr->valid & 0x800)
      )
   {
      FUN_00517270(png_ptr, s_Duplicate_cHRM_chunk_00622084);
      png_crc_finish_e(png_ptr, length);
      return;
   }

   if (length != 32)
   {
      FUN_00517270(png_ptr, s_Incorrect_cHRM_chunk_length_006220d0);
      png_crc_finish_e(png_ptr, length);
      return;
   }

   png_crc_read(png_ptr, buf, 4);
   int_x_white = (png_fixed_point)png_get_uint_32(buf);

   png_crc_read(png_ptr, buf, 4);
   int_y_white = (png_fixed_point)png_get_uint_32(buf);

   if (int_x_white > 80000L || int_y_white > 80000L ||
      int_x_white + int_y_white > 100000L)
   {
      FUN_00517270(png_ptr, s_Invalid_cHRM_white_point_00621fe4);
      png_crc_finish_e(png_ptr, 24);
      return;
   }

   png_crc_read(png_ptr, buf, 4);
   int_x_red = (png_fixed_point)png_get_uint_32(buf);

   png_crc_read(png_ptr, buf, 4);
   int_y_red = (png_fixed_point)png_get_uint_32(buf);

   if (int_x_red > 80000L || int_y_red > 80000L ||
      int_x_red + int_y_red > 100000L)
   {
      FUN_00517270(png_ptr, s_Invalid_cHRM_red_point_00622000);
      png_crc_finish_e(png_ptr, 16);
      return;
   }

   png_crc_read(png_ptr, buf, 4);
   int_x_green = (png_fixed_point)png_get_uint_32(buf);

   png_crc_read(png_ptr, buf, 4);
   int_y_green = (png_fixed_point)png_get_uint_32(buf);

   if (int_x_green > 80000L || int_y_green > 80000L ||
      int_x_green + int_y_green > 100000L)
   {
      FUN_00517270(png_ptr, s_Invalid_cHRM_green_point_00622018);
      png_crc_finish_e(png_ptr, 8);
      return;
   }

   png_crc_read(png_ptr, buf, 4);
   int_x_blue = (png_fixed_point)png_get_uint_32(buf);

   png_crc_read(png_ptr, buf, 4);
   int_y_blue = (png_fixed_point)png_get_uint_32(buf);

   if (int_x_blue > 80000L || int_y_blue > 80000L ||
      int_x_blue + int_y_blue > 100000L)
   {
      FUN_00517270(png_ptr, s_Invalid_cHRM_blue_point_00622034);
      FUN_00523170(png_ptr, 0);
      return;
   }
   white_y = (float)int_y_white / (float)100000.0;
   red_x   = (float)int_x_red   / (float)100000.0;
   red_y   = (float)int_y_red   / (float)100000.0;
   white_x = (float)int_x_white / (float)100000.0;
   green_x = (float)int_x_green / (float)100000.0;
   green_y = (float)int_y_green / (float)100000.0;
   blue_x  = (float)int_x_blue  / (float)100000.0;
   blue_y  = (float)int_y_blue  / (float)100000.0;

   if (info_ptr->valid & 0x800)
      {
      if (abs(int_x_white - 31270L) > 1000 ||
          abs(int_y_white - 32900L) > 1000 ||
          abs(  int_x_red - 64000L) > 1000 ||
          abs(  int_y_red - 33000L) > 1000 ||
          abs(int_x_green - 30000L) > 1000 ||
          abs(int_y_green - 60000L) > 1000 ||
          abs( int_x_blue - 15000L) > 1000 ||
          abs( int_y_blue -  6000L) > 1000)
         {

            FUN_00517270(png_ptr,
              s_Ignoring_incorrect_cHRM_value_0062204c);
         }
         FUN_00523170(png_ptr, 0);
         return;
      }

   FUN_00515ae0(png_ptr, info_ptr,
      white_x, white_y, red_x, red_y, green_x, green_y, blue_x, blue_y);
   FUN_00515d50(png_ptr, info_ptr,
      int_x_white, int_y_white, int_x_red, int_y_red, int_x_green,
      int_y_green, int_x_blue, int_y_blue);
   if (FUN_00523170(png_ptr, 0))
      return;
}
