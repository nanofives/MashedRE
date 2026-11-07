// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00523640: libpng 1.0.12 pngrutil.c png_handle_IHDR (no PNG_MNG_FEATURES filter_type store),
   with png_crc_read / png_crc_finish / png_crc_error / png_get_uint_32 inlined, written as static __forceinline
   copies above the function.
   100.0% (regs 100, ops 100) first pass. The inlined png_crc_error here has no flags-load hoist. */
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
MGLOBALA(char, s_Out_of_place_IHDR_00621d64, 18, 0x00621d64);
MGLOBALA(char, s_Invalid_IHDR_chunk_00621d50, 19, 0x00621d50);
void FUN_00517250(png_structp png_ptr, const char *message); /* png_error */
void FUN_00515fd0(png_structp png_ptr, void *info_ptr, png_uint_32 width, png_uint_32 height, int bit_depth,
   int color_type, int interlace_type, int compression_type, int filter_type); /* png_set_IHDR */

static __forceinline png_uint_32 png_get_uint_32(png_bytep buf)
{
   png_uint_32 i = ((png_uint_32)(*buf) << 24) +
      ((png_uint_32)(*(buf + 1)) << 16) +
      ((png_uint_32)(*(buf + 2)) << 8) +
      (png_uint_32)(*(buf + 3));

   return (i);
}

// MATCH: 0x00523640 FUN_00523640
void FUN_00523640(png_structp png_ptr, void *info_ptr, png_uint_32 length)
{
   png_byte buf[13];
   png_uint_32 width, height;
   int bit_depth, color_type, compression_type, filter_type;
   int interlace_type;

   if (*(png_uint_32 *)(png_ptr + 0x68) & 1)
      FUN_00517250(png_ptr, s_Out_of_place_IHDR_00621d64);

   if (length != 13)
      FUN_00517250(png_ptr, s_Invalid_IHDR_chunk_00621d50);

   *(png_uint_32 *)(png_ptr + 0x68) |= 1;

   png_crc_read(png_ptr, buf, 13);
   png_crc_finish(png_ptr, 0);

   width = png_get_uint_32(buf);
   height = png_get_uint_32(buf + 4);
   bit_depth = buf[8];
   color_type = buf[9];
   compression_type = buf[10];
   filter_type = buf[11];
   interlace_type = buf[12];

   *(png_uint_32 *)(png_ptr + 0xc8) = width;
   *(png_uint_32 *)(png_ptr + 0xcc) = height;
   *(png_byte *)(png_ptr + 0x127) = (png_byte)bit_depth;
   *(png_byte *)(png_ptr + 0x123) = (png_byte)interlace_type;
   *(png_byte *)(png_ptr + 0x126) = (png_byte)color_type;

   switch (*(png_byte *)(png_ptr + 0x126))
   {
      case 0:
      case 3:
         *(png_byte *)(png_ptr + 0x12a) = 1;
         break;
      case 2:
         *(png_byte *)(png_ptr + 0x12a) = 3;
         break;
      case 4:
         *(png_byte *)(png_ptr + 0x12a) = 2;
         break;
      case 6:
         *(png_byte *)(png_ptr + 0x12a) = 4;
         break;
   }

   *(png_byte *)(png_ptr + 0x129) = (png_byte)(*(png_byte *)(png_ptr + 0x127) *
   *(png_byte *)(png_ptr + 0x12a));
   *(png_uint_32 *)(png_ptr + 0xd8) = ((*(png_uint_32 *)(png_ptr + 0xc8) *
      (png_uint_32)*(png_byte *)(png_ptr + 0x129) + 7) >> 3);
   FUN_00515fd0(png_ptr, info_ptr, width, height, bit_depth,
      color_type, interlace_type, compression_type, filter_type);
}
