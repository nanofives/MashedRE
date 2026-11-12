// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00523e40: libpng 1.0.12 pngrutil.c png_handle_gAMA (floating + fixed point, sRGB check, no
   console io). The original's compiler inlined png_crc_finish unevenly: the "Incorrect length" path and the
   main path call png_crc_error (0x005232c0), the IDAT and Duplicate paths inline png_crc_error but call
   png_get_uint_32 (0x00523110). Written as static __forceinline variants png_crc_finish_e / _i
   (pngcrc2 block).
   90.0% (regs 90.0, h6 round 2, was 87.5; the length-slot residual fixed by the length arg copy). Residual: (1) stack slot: the original reuses the dead length arg slot
   ([esp+0x24]) for istop / crc_bytes / igamma, ours the png_ptr slot ([esp+0x1c]); (2) the IDAT path's inlined
   png_crc_error hoists the flags load (as 0x005232c0), the Duplicate path does not, in both builds.
   Tried: prior prototype, K&R definition, permuter 106 variants. Class (b) stack packing.
   Round 2 (h6): length arg copy fixes the param-slot packing; only the class (a) flags-load hoist is left. */
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
MGLOBALA(char, s_Missing_IHDR_before_gAMA_00621f48, 25, 0x00621f48);
MGLOBALA(char, s_Invalid_gAMA_after_IDAT_00621f14, 24, 0x00621f14);
MGLOBALA(char, s_Out_of_place_gAMA_chunk_00621efc, 24, 0x00621efc);
MGLOBALA(char, s_Duplicate_gAMA_chunk_00621ee4, 21, 0x00621ee4);
MGLOBALA(char, s_Incorrect_gAMA_chunk_length_00621f2c, 28, 0x00621f2c);
MGLOBALA(char, s_Ignoring_gAMA_chunk_with_gamma_0_00621ec0, 33, 0x00621ec0);
MGLOBALA(char, s_Ignoring_incorrect_gAMA_value_00621e88, 56, 0x00621e88);
typedef int png_fixed_point;
typedef struct png_info_head_s { png_uint_32 width; png_uint_32 height; png_uint_32 valid; } png_info;
typedef png_info *png_infop;
void FUN_00517250(png_structp png_ptr, const char *message); /* png_error */
void FUN_00517270(png_structp png_ptr, const char *message); /* png_warning */
void FUN_00515e50(png_structp png_ptr, png_infop info_ptr, double file_gamma); /* png_set_gAMA */
void FUN_00515ee0(png_structp png_ptr, png_infop info_ptr, png_fixed_point int_gamma); /* png_set_gAMA_fixed */

// MATCH: 0x00523e40 FUN_00523e40
void FUN_00523e40(png_structp png_ptr, png_infop info_ptr, png_uint_32 length_arg)
{
   png_uint_32 length = length_arg;
   png_fixed_point igamma;
   float file_gamma;
   png_byte buf[4];

   if (!(*(png_uint_32 *)(png_ptr + 0x68) & 1))
      FUN_00517250(png_ptr, s_Missing_IHDR_before_gAMA_00621f48);
   else if (*(png_uint_32 *)(png_ptr + 0x68) & 4)
   {
      FUN_00517270(png_ptr, s_Invalid_gAMA_after_IDAT_00621f14);
      png_crc_finish_i(png_ptr, length);
      return;
   }
   else if (*(png_uint_32 *)(png_ptr + 0x68) & 2)
      FUN_00517270(png_ptr, s_Out_of_place_gAMA_chunk_00621efc);

   else if (info_ptr != 0 && (info_ptr->valid & 1)
      && !(info_ptr->valid & 0x800)
      )
   {
      FUN_00517270(png_ptr, s_Duplicate_gAMA_chunk_00621ee4);
      png_crc_finish_i(png_ptr, length);
      return;
   }

   if (length != 4)
   {
      FUN_00517270(png_ptr, s_Incorrect_gAMA_chunk_length_00621f2c);
      png_crc_finish_e(png_ptr, length);
      return;
   }

   png_crc_read(png_ptr, buf, 4);
   if (png_crc_finish_e(png_ptr, 0))
      return;

   igamma = (png_fixed_point)(((png_uint_32)(*buf) << 24) +
      ((png_uint_32)(*(buf + 1)) << 16) +
      ((png_uint_32)(*(buf + 2)) << 8) +
      (png_uint_32)(*(buf + 3)));
   if (igamma == 0)
      {
         FUN_00517270(png_ptr,
           s_Ignoring_gAMA_chunk_with_gamma_0_00621ec0);
         return;
      }

   if (info_ptr->valid & 0x800)
      if(igamma < 45000L || igamma > 46000L)
      {
         FUN_00517270(png_ptr,
           s_Ignoring_incorrect_gAMA_value_00621e88);
         return;
      }

   file_gamma = (float)igamma / (float)100000.0;
     *(float *)(png_ptr + 0x15c) = file_gamma;
     FUN_00515e50(png_ptr, info_ptr, file_gamma);
   FUN_00515ee0(png_ptr, info_ptr, igamma);
}
