// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00528810: 97.8% (regs 98.1%). libpng 1.0.12 pngrutil.c png_read_finish_row with png_crc_finish(0),
   png_crc_error, png_crc_read and png_get_uint_32 inline; png_struct offsets below (zstream +0x74, idat_size
   +0x10c, chunk_name +0x11c, interlaced +0x123, pass +0x124, pixel_depth +0x129); png_pass_* tables at
   0x005e32c4/32e0/32fc/3318, png_IDAT at 0x005e3224. What moved it (82.2 -> 97.8): png_crc_error's result as an
   int (setne + test), png_memcmp as the memcmp intrinsic, the zbuf png_crc_read with its arguments copied to
   locals. Residual: partial shrink-wrap only (the original pushes ebx/ebp/edi in the prologue, ours after the
   row_number < num_rows exit; the positive-if form gives the same code). Class (a), same as h1's shrink-wrap
   family. */
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
typedef unsigned int png_size_t;
typedef unsigned char Byte;
typedef unsigned int uInt;
typedef struct {
    Byte *next_in;              /* 0x74 */
    uInt avail_in;              /* 0x78 */
    unsigned long total_in;     /* 0x7c */
    Byte *next_out;             /* 0x80 */
    uInt avail_out;             /* 0x84 */
    unsigned long total_out;    /* 0x88 */
    char *msg;                  /* 0x8c */
    char rest[0x38 - 0x1c];
} z_stream;
typedef struct png_struct_def {
    char pad0[0x68];
    png_uint_32 mode;           /* 0x68 */
    png_uint_32 flags;          /* 0x6c */
    png_uint_32 transformations;/* 0x70 */
    z_stream zstream;           /* 0x74 */
    png_bytep zbuf;             /* 0xac */
    png_size_t zbuf_size;       /* 0xb0 */
    char padb4[0xc8 - 0xb4];
    png_uint_32 width;          /* 0xc8 */
    png_uint_32 height;         /* 0xcc */
    png_uint_32 num_rows;       /* 0xd0 */
    png_uint_32 usr_width;      /* 0xd4 */
    png_uint_32 rowbytes;       /* 0xd8 */
    png_uint_32 irowbytes;      /* 0xdc */
    png_uint_32 iwidth;         /* 0xe0 */
    png_uint_32 row_number;     /* 0xe4 */
    png_bytep prev_row;         /* 0xe8 */
    char padec[0x10c - 0xec];
    png_uint_32 idat_size;      /* 0x10c */
    png_uint_32 crc;            /* 0x110 */
    char pad114[0x11c - 0x114];
    png_byte chunk_name[5];     /* 0x11c */
    char pad121[2];
    png_byte interlaced;        /* 0x123 */
    png_byte pass;              /* 0x124 */
    char pad125[0x129 - 0x125];
    png_byte pixel_depth;       /* 0x129 */
} png_struct;
typedef png_struct *png_structp;

MGLOBALA(int, DAT_005e32c4, 7, 0x005e32c4);    /* png_pass_start */
MGLOBALA(int, DAT_005e32e0, 7, 0x005e32e0);    /* png_pass_inc */
MGLOBALA(int, DAT_005e32fc, 7, 0x005e32fc);    /* png_pass_ystart */
MGLOBALA(int, DAT_005e3318, 7, 0x005e3318);    /* png_pass_yinc */
MGLOBALA(png_byte, DAT_005e3224, 5, 0x005e3224); /* png_IDAT */
MGLOBALA(char, s_Extra_compressed_data_00621528, 22, 0x00621528);
MGLOBALA(char, s_Not_enough_image_data_00621554, 22, 0x00621554);
MGLOBALA(char, s_CRC_error_00621ce8, 10, 0x00621ce8);
MGLOBALA(char, s_Extra_compression_data_00622814, 23, 0x00622814);
MGLOBALA(char, s_Decompression_Error_0062282c, 20, 0x0062282c);

int memcmp(const void *buf1, const void *buf2, unsigned int count);
#pragma intrinsic(memcmp)
void FUN_00516d30(png_structp png_ptr);
void FUN_00516d50(png_structp png_ptr, png_bytep ptr, png_size_t length);
void FUN_00517250(png_structp png_ptr, const char *error_message);
void FUN_005172b0(png_structp png_ptr, const char *error_message);
void FUN_00517380(png_structp png_ptr, const char *warning_message);
void FUN_0051c9f0(png_structp png_ptr, png_bytep data, png_size_t length);
void *FUN_00520990(png_structp png_ptr, void *s1, int value, png_uint_32 size);
int FUN_00522ab0(z_stream *strm);
int FUN_00522ce0(z_stream *strm, int flush);

#define png_pass_start DAT_005e32c4
#define png_pass_inc DAT_005e32e0
#define png_pass_ystart DAT_005e32fc
#define png_pass_yinc DAT_005e3318
#define png_get_uint_32(buf) (((png_uint_32)(*(buf)) << 24) + ((png_uint_32)(*((buf) + 1)) << 16) + \
   ((png_uint_32)(*((buf) + 2)) << 8) + (png_uint_32)(*((buf) + 3)))
#define png_crc_read(p, buf, len) { FUN_0051c9f0(p, buf, len); FUN_00516d50(p, buf, len); }

// MATCH: 0x00528810 FUN_00528810
void FUN_00528810(png_structp png_ptr)
{
   png_ptr->row_number++;
   if (png_ptr->row_number < png_ptr->num_rows)
      return;

   if (png_ptr->interlaced)
   {
      png_ptr->row_number = 0;
      FUN_00520990(png_ptr, png_ptr->prev_row, 0, png_ptr->rowbytes + 1);
      do
      {
         png_ptr->pass++;
         if (png_ptr->pass >= 7)
            break;
         png_ptr->iwidth = (png_ptr->width +
            png_pass_inc[png_ptr->pass] - 1 -
            png_pass_start[png_ptr->pass]) /
            png_pass_inc[png_ptr->pass];
            png_ptr->irowbytes = ((png_ptr->iwidth *
               (png_uint_32)png_ptr->pixel_depth + 7) >> 3) +1;

         if (!(png_ptr->transformations & 0x0002))
         {
            png_ptr->num_rows = (png_ptr->height +
               png_pass_yinc[png_ptr->pass] - 1 -
               png_pass_ystart[png_ptr->pass]) /
               png_pass_yinc[png_ptr->pass];
            if (!(png_ptr->num_rows))
               continue;
         }
         else
            break;
      } while (png_ptr->iwidth == 0);

      if (png_ptr->pass < 7)
         return;
   }

   if (!(png_ptr->flags & 0x0020))
   {
      char extra;
      int ret;

      png_ptr->zstream.next_out = (Byte *)&extra;
      png_ptr->zstream.avail_out = (uInt)1;
      for(;;)
      {
         if (!(png_ptr->zstream.avail_in))
         {
            while (!png_ptr->idat_size)
            {
               png_byte chunk_length[4];

               /* png_crc_finish(png_ptr, 0) with png_crc_error inline */
               {
                  png_byte crc_bytes[4];
                  png_uint_32 crc;
                  int need_crc = 1;
                  int err;

                  if (png_ptr->chunk_name[0] & 0x20)
                  {
                     if ((png_ptr->flags & 0x0300) == (0x0100 | 0x0200))
                        need_crc = 0;
                  }
                  else
                  {
                     if (png_ptr->flags & 0x0800)
                        need_crc = 0;
                  }

                  FUN_0051c9f0(png_ptr, crc_bytes, 4);

                  if (need_crc)
                  {
                     crc = png_get_uint_32(crc_bytes);
                     err = (int)(crc != png_ptr->crc);
                  }
                  else
                     err = 0;
                  if (err)
                  {
                     {
                        if (((png_ptr->chunk_name[0] & 0x20) &&
                             !(png_ptr->flags & 0x0200)) ||
                            (!(png_ptr->chunk_name[0] & 0x20) &&
                             (png_ptr->flags & 0x0400)))
                        {
                           FUN_00517380(png_ptr, s_CRC_error_00621ce8);
                        }
                        else
                        {
                           FUN_005172b0(png_ptr, s_CRC_error_00621ce8);
                        }
                     }
                  }
               }

               FUN_0051c9f0(png_ptr, chunk_length, 4);
               png_ptr->idat_size = png_get_uint_32(chunk_length);

               FUN_00516d30(png_ptr);
               png_crc_read(png_ptr, png_ptr->chunk_name, 4);
               if (memcmp(png_ptr->chunk_name, DAT_005e3224, 4))
                  FUN_00517250(png_ptr, s_Not_enough_image_data_00621554);

            }
            png_ptr->zstream.avail_in = (uInt)png_ptr->zbuf_size;
            png_ptr->zstream.next_in = png_ptr->zbuf;
            if (png_ptr->zbuf_size > png_ptr->idat_size)
               png_ptr->zstream.avail_in = (uInt)png_ptr->idat_size;
            {
               png_bytep buf = png_ptr->zbuf;
               png_size_t length = png_ptr->zstream.avail_in;
               FUN_0051c9f0(png_ptr, buf, length);
               FUN_00516d50(png_ptr, buf, length);
            }
            png_ptr->idat_size -= png_ptr->zstream.avail_in;
         }
         ret = FUN_00522ce0(&png_ptr->zstream, 1);
         if (ret == 1)
         {
            if (!(png_ptr->zstream.avail_out) || png_ptr->zstream.avail_in ||
               png_ptr->idat_size)
               FUN_00517250(png_ptr, s_Extra_compressed_data_00621528);
            png_ptr->mode |= 0x08;
            png_ptr->flags |= 0x0020;
            break;
         }
         if (ret != 0)
            FUN_00517250(png_ptr, png_ptr->zstream.msg ? png_ptr->zstream.msg :
                      s_Decompression_Error_0062282c);

         if (!(png_ptr->zstream.avail_out))
            FUN_00517250(png_ptr, s_Extra_compressed_data_00621528);

      }
      png_ptr->zstream.avail_out = 0;
   }

   if (png_ptr->idat_size || png_ptr->zstream.avail_in)
      FUN_00517250(png_ptr, s_Extra_compression_data_00622814);

   FUN_00522ab0(&png_ptr->zstream);

   png_ptr->mode |= 0x08;
}
#undef png_pass_start
#undef png_pass_inc
#undef png_pass_ystart
#undef png_pass_yinc
#undef png_get_uint_32
#undef png_crc_read
