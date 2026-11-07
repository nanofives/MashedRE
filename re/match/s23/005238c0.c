// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x005238c0: libpng 1.0.12 pngrutil.c png_handle_PLTE (PNG_READ_OPT_PLTE_SUPPORTED, pointer
   indexing, tRNS read). png_crc_finish inlined per call site as the original's compiler did: IDAT path fully,
   grayscale path with png_get_uint_32 called, invalid-length and main paths with png_crc_error called
   (static __forceinline variants, pngcrc3 block).
   87.1% (regs 87.1, ops 99.4). Residual: one instruction. The IDAT path's inlined png_crc_error: the
   original hoists the identical 'mov eax,[esi+0x6c]' out of both arms, ours keeps it in each arm; every other
   '!' line is a branch displacement shifted by those 3 bytes. Class (a), see 0x005232c0. */
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
MGLOBALA(char, s_Missing_IHDR_before_PLTE_00621e3c, 25, 0x00621e3c);
MGLOBALA(char, s_Invalid_PLTE_after_IDAT_00621e24, 24, 0x00621e24);
MGLOBALA(char, s_Duplicate_PLTE_chunk_00621e0c, 21, 0x00621e0c);
MGLOBALA(char, s_Ignoring_PLTE_chunk_in_grayscale_PNG_00621de4, 37, 0x00621de4);
MGLOBALA(char, s_Invalid_palette_chunk_00621dcc, 22, 0x00621dcc);
MGLOBALA(char, s_Truncating_incorrect_tRNS_chunk_length_00621da4, 39, 0x00621da4);
MGLOBALA(char, s_Truncating_incorrect_info_tRNS_chunk_length_00621d78, 44, 0x00621d78);
typedef unsigned short png_uint_16;
typedef struct png_info_head_s { png_uint_32 width; png_uint_32 height; png_uint_32 valid; png_uint_32 rowbytes;
   void *palette; png_uint_16 num_palette; png_uint_16 num_trans; } png_info;
typedef png_info *png_infop;
typedef struct png_color_struct { png_byte red; png_byte green; png_byte blue; } png_color;
typedef png_color *png_colorp;
void FUN_00517250(png_structp png_ptr, const char *message); /* png_error */
void FUN_00517270(png_structp png_ptr, const char *message); /* png_warning */
void FUN_005163a0(png_structp png_ptr, png_infop info_ptr, png_colorp palette, int num_palette); /* png_set_PLTE */

// MATCH: 0x005238c0 FUN_005238c0
void FUN_005238c0(png_structp png_ptr, png_infop info_ptr, png_uint_32 length)
{
   png_color palette[256];
   int num, i;
   png_colorp pal_ptr;

   if (!(*(png_uint_32 *)(png_ptr + 0x68) & 1))
      FUN_00517250(png_ptr, s_Missing_IHDR_before_PLTE_00621e3c);
   else if (*(png_uint_32 *)(png_ptr + 0x68) & 4)
   {
      FUN_00517270(png_ptr, s_Invalid_PLTE_after_IDAT_00621e24);
      png_crc_finish(png_ptr, length);
      return;
   }
   else if (*(png_uint_32 *)(png_ptr + 0x68) & 2)
      FUN_00517250(png_ptr, s_Duplicate_PLTE_chunk_00621e0c);

   *(png_uint_32 *)(png_ptr + 0x68) |= 2;

   if (!(*(png_byte *)(png_ptr + 0x126) & 2))
   {
      FUN_00517270(png_ptr,
        s_Ignoring_PLTE_chunk_in_grayscale_PNG_00621de4);
      png_crc_finish_i(png_ptr, length);
      return;
   }

   if (length > 3*256 || length % 3)
   {
      if (*(png_byte *)(png_ptr + 0x126) != 3)
      {
         FUN_00517270(png_ptr, s_Invalid_palette_chunk_00621dcc);
         png_crc_finish_e(png_ptr, length);
         return;
      }
      else
      {
         FUN_00517250(png_ptr, s_Invalid_palette_chunk_00621dcc);
      }
   }

   num = (int)length / 3;

   for (i = 0, pal_ptr = palette; i < num; i++, pal_ptr++)
   {
      png_byte buf[3];

      png_crc_read(png_ptr, buf, 3);
      pal_ptr->red = buf[0];
      pal_ptr->green = buf[1];
      pal_ptr->blue = buf[2];
   }

   {
      png_crc_finish_e(png_ptr, 0);
   }

   FUN_005163a0(png_ptr, info_ptr, palette, num);

   if (*(png_byte *)(png_ptr + 0x126) == 3)
   {
      if (info_ptr != 0 && (info_ptr->valid & 0x10))
      {
         if (*(png_uint_16 *)(png_ptr + 0x11a) > (png_uint_16)num)
         {
            FUN_00517270(png_ptr, s_Truncating_incorrect_tRNS_chunk_length_00621da4);
            *(png_uint_16 *)(png_ptr + 0x11a) = (png_uint_16)num;
         }
         if (info_ptr->num_trans > (png_uint_16)num)
         {
            FUN_00517270(png_ptr, s_Truncating_incorrect_info_tRNS_chunk_length_00621d78);
            info_ptr->num_trans = (png_uint_16)num;
         }
      }
   }

}
