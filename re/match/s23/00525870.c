// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00525870 (h4): libpng 1.0.12 pngrutil.c png_handle_sPLT. Best 91.4% (regs 91.4%).
   Written from the official source with the original's inlining mirrored by hand: png_crc_read, png_crc_finish,
   png_crc_error, png_get_uint_32/16 (and png_check_chunk_name where present) are inline in the original, per call
   site either fully inline or calling png_crc_error 0x005232c0 / png_get_uint_32 0x00523110 / png_crc_finish
   0x00523170; each copy follows the original's form. png_chunk_error = 0x005172b0, png_chunk_warning = 0x00517380.
   Residual class (b): Residual: flags-load hoist in the IDAT copy (1 ins, shifts every later jump by 1 byte) + crc_bytes / pp-offset temp in png_ptr's slot vs the original's length slot (tried uchar* png_ptr). */
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

typedef struct png_sPLT_entry_s { ushort red; ushort green; ushort blue; ushort alpha; ushort frequency; } png_sPLT_entry;
typedef struct png_sPLT_s { char *name; uchar depth; png_sPLT_entry *entries; int nentries; } png_sPLT_t;
MGLOBALA(char, s_Missing_IHDR_before_sPLT_006222e4, 25, 0x006222e4);
MGLOBALA(char, s_Invalid_sPLT_after_IDAT_006222cc, 24, 0x006222cc);
MGLOBALA(char, s_malformed_sPLT_chunk_006222b4, 21, 0x006222b4);
MGLOBALA(char, s_sPLT_chunk_has_bad_length_00622298, 26, 0x00622298);
MGLOBALA(char, s_CRC_error_00621ce8, 10, 0x00621ce8);
void FUN_00517250(int png_ptr, char *msg);
void FUN_00517270(int png_ptr, char *msg);
void FUN_005172b0(int png_ptr, char *msg);
void FUN_00517380(int png_ptr, char *msg);
void *FUN_005208c0(int png_ptr, uint size);
void FUN_00520930(int png_ptr, void *p);
void FUN_0051c9f0(int png_ptr, void *buf, uint length);
void FUN_00516d50(int png_ptr, void *buf, uint length);
uint FUN_00523110(uchar *buf);
int FUN_005232c0(int png_ptr);
void FUN_00516920(int png_ptr, int info_ptr, png_sPLT_t *entries, int nentries);

// MATCH: 0x00525870 FUN_00525870
void FUN_00525870(int png_ptr, int info_ptr, uint length)
{
   uchar *chunkdata;
   uchar *entry_start;
   png_sPLT_t new_palette;
   int data_length, entry_size, j;
   uint slength;
   int crc_err;

   if (!(*(uint *)(png_ptr + 0x68) & 1))
      FUN_00517250(png_ptr, s_Missing_IHDR_before_sPLT_006222e4);
   else if (*(uint *)(png_ptr + 0x68) & 4)
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Invalid_sPLT_after_IDAT_006222cc);
   istop = *(uint *)(png_ptr + 0xb0);
   for (i = length; i > istop; i -= istop)
   {
      uint len = *(uint *)(png_ptr + 0xb0);
      uchar *buf = *(uchar **)(png_ptr + 0xac);
      FUN_0051c9f0(png_ptr, buf, len);
      FUN_00516d50(png_ptr, buf, len);
   }
   if (i)
   {
      uchar *buf = *(uchar **)(png_ptr + 0xac);
      FUN_0051c9f0(png_ptr, buf, i);
      FUN_00516d50(png_ptr, buf, i);
   }

      {
      uchar crc_bytes[4];
      int need_crc;
   need_crc = 1;
   if (*(uchar *)(png_ptr + 0x11c) & 0x20)
   {
      if ((*(uint *)(png_ptr + 0x6c) & 0x300) == 0x300)
         need_crc = 0;
   }
   else
   {
      if (*(uint *)(png_ptr + 0x6c) & 0x800)
         need_crc = 0;
   }
   FUN_0051c9f0(png_ptr, crc_bytes, 4);
   if (need_crc)
      crc_err = (int)(FUN_00523110(crc_bytes) != *(uint *)(png_ptr + 0x110));
   else
      crc_err = 0;

      }
   if (crc_err)
   {
      if (((*(uchar *)(png_ptr + 0x11c) & 0x20) &&
           !(*(uint *)(png_ptr + 0x6c) & 0x200)) ||
          (!(*(uchar *)(png_ptr + 0x11c) & 0x20) &&
          (*(uint *)(png_ptr + 0x6c) & 0x400)))
      {
         FUN_00517380(png_ptr, s_CRC_error_00621ce8);
      }
      else
      {
         FUN_005172b0(png_ptr, s_CRC_error_00621ce8);
      }
      
      return;
   }

      return;
   }

   chunkdata = (uchar *)FUN_005208c0(png_ptr, length + 1);
   slength = length;
   FUN_0051c9f0(png_ptr, chunkdata, slength);
   FUN_00516d50(png_ptr, chunkdata, slength);
   crc_err = FUN_005232c0(png_ptr);

   if (crc_err)
   {
      if (((*(uchar *)(png_ptr + 0x11c) & 0x20) &&
           !(*(uint *)(png_ptr + 0x6c) & 0x200)) ||
          (!(*(uchar *)(png_ptr + 0x11c) & 0x20) &&
          (*(uint *)(png_ptr + 0x6c) & 0x400)))
      {
         FUN_00517380(png_ptr, s_CRC_error_00621ce8);
      }
      else
      {
         FUN_005172b0(png_ptr, s_CRC_error_00621ce8);
      }
      FUN_00520930(png_ptr, chunkdata);
      return;
   }


   chunkdata[slength] = 0x00;

   for (entry_start = chunkdata; *entry_start; entry_start++)
      ;
   ++entry_start;

   if (entry_start > chunkdata + slength)
   {
      FUN_00520930(png_ptr, chunkdata);
      FUN_00517270(png_ptr, s_malformed_sPLT_chunk_006222b4);
      return;
   }

   new_palette.depth = *entry_start++;
   entry_size = (new_palette.depth == 8 ? 6 : 10);
   data_length = (slength - (entry_start - chunkdata));

   if (data_length % entry_size)
   {
      FUN_00520930(png_ptr, chunkdata);
      FUN_00517270(png_ptr, s_sPLT_chunk_has_bad_length_00622298);
      return;
   }

   new_palette.nentries = data_length / entry_size;
   new_palette.entries = (png_sPLT_entry *)FUN_005208c0(
       png_ptr, new_palette.nentries * sizeof(png_sPLT_entry));

   for (j = 0; j < new_palette.nentries; j++)
   {
      png_sPLT_entry *pp = new_palette.entries + j;

      if (new_palette.depth == 8)
      {
          pp->red = *entry_start++;
          pp->green = *entry_start++;
          pp->blue = *entry_start++;
          pp->alpha = *entry_start++;
      }
      else
      {
          pp->red   = (ushort)(((ushort)(*entry_start) << 8) + (ushort)(*(entry_start + 1))); entry_start += 2;
          pp->green = (ushort)(((ushort)(*entry_start) << 8) + (ushort)(*(entry_start + 1))); entry_start += 2;
          pp->blue  = (ushort)(((ushort)(*entry_start) << 8) + (ushort)(*(entry_start + 1))); entry_start += 2;
          pp->alpha = (ushort)(((ushort)(*entry_start) << 8) + (ushort)(*(entry_start + 1))); entry_start += 2;
      }
      pp->frequency = (ushort)(((ushort)(*entry_start) << 8) + (ushort)(*(entry_start + 1))); entry_start += 2;
   }

   new_palette.name = (char *)chunkdata;

   FUN_00516920(png_ptr, info_ptr, &new_palette, 1);

   FUN_00520930(png_ptr, chunkdata);
   FUN_00520930(png_ptr, new_palette.entries);
}
