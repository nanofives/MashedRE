// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00523cd0: libpng 1.0.12 pngrutil.c png_handle_IEND, with png_crc_finish (and its png_crc_read,
   png_crc_error) inlined, written as static __forceinline copies above the function.
   91.3% (regs 91.3, ops 98.4). Residual: same flags-load hoist as 0x005232c0 png_crc_error (inlined here).
   Tried for the family: flags in a local (72.8% here), ternary / combined-condition forms of need_crc. Class (a), see 0x005232c0. */
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
MGLOBALA(char, s_No_image_in_file_00621e74, 17, 0x00621e74);
MGLOBALA(char, s_Incorrect_IEND_chunk_length_00621e58, 28, 0x00621e58);
void FUN_00517250(png_structp png_ptr, const char *message); /* png_error */
void FUN_00517270(png_structp png_ptr, const char *message); /* png_warning */

// MATCH: 0x00523cd0 FUN_00523cd0
void FUN_00523cd0(png_structp png_ptr, void *info_ptr, png_uint_32 length)
{
   if (!(*(png_uint_32 *)(png_ptr + 0x68) & 1) || !(*(png_uint_32 *)(png_ptr + 0x68) & 4))
   {
      FUN_00517250(png_ptr, s_No_image_in_file_00621e74);

      info_ptr = info_ptr;
   }

   *(png_uint_32 *)(png_ptr + 0x68) |= (0x08 | 0x10);

   if (length != 0)
   {
      FUN_00517270(png_ptr, s_Incorrect_IEND_chunk_length_00621e58);
   }
   png_crc_finish(png_ptr, length);
}
