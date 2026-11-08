// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00527a40 (h4): libpng 1.0.12 pngrutil.c png_handle_zTXt. Best 97.8% (regs 97.8%).
   Written from the official source with the original's inlining mirrored by hand: png_crc_read, png_crc_finish,
   png_crc_error, png_get_uint_32/16 (and png_check_chunk_name where present) are inline in the original, per call
   site either fully inline or calling png_crc_error 0x005232c0 / png_get_uint_32 0x00523110 / png_crc_finish
   0x00523170; each copy follows the original's form. png_chunk_error = 0x005172b0, png_chunk_warning = 0x00517380.
   Residual class (b): data_len in an inner block (packs it into the dead length slot like the original: 89.1 -> 97.8). Remaining: the original loads data_len before the four pushes for png_set_text; tried all 24 store orders of the text_ptr fields, permuter 6 rounds + walk 40 (no gain). */
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

typedef struct png_text_s { int compression; char *key; char *text; uint text_length; } png_text;
MGLOBALA(char, s_Missing_IHDR_before_zTXt_006227a8, 25, 0x006227a8);
MGLOBALA(char, s_Zero_length_zTXt_chunk_00622790, 23, 0x00622790);
MGLOBALA(char, s_Unknown_compression_type_in_zTXt_00622768, 39, 0x00622768);
MGLOBALA(char, s_CRC_error_00621ce8, 10, 0x00621ce8);
void FUN_00517250(int png_ptr, char *msg);
void FUN_00517270(int png_ptr, char *msg);
void FUN_005172b0(int png_ptr, char *msg);
void FUN_00517380(int png_ptr, char *msg);
void *FUN_005208c0(int png_ptr, uint size);
void FUN_00520930(int png_ptr, void *p);
void FUN_0051c9f0(int png_ptr, void *buf, uint length);
void FUN_00516d50(int png_ptr, void *buf, uint length);
void FUN_00516650(int png_ptr, int info_ptr, png_text *text, int num);
char *FUN_00523360(int png_ptr, int comp_type, char *chunkdata, uint chunklength, uint prefix_size, uint *newlength);

// MATCH: 0x00527a40 FUN_00527a40
void FUN_00527a40(int png_ptr, int info_ptr, uint length)
{
   png_text *text_ptr;
   char *chunkdata;
   char *text;
   int comp_type;
   uint slength, prefix_len;
   uchar crc_bytes[4];
   int need_crc;
   int crc_err;

   if (!(*(uint *)(png_ptr + 0x68) & 1))
      FUN_00517250(png_ptr, s_Missing_IHDR_before_zTXt_006227a8);

   if (*(uint *)(png_ptr + 0x68) & 4)
      *(uint *)(png_ptr + 0x68) |= 8;

   chunkdata = (char *)FUN_005208c0(png_ptr, length + 1);
   slength = length;
   FUN_0051c9f0(png_ptr, chunkdata, slength);
   FUN_00516d50(png_ptr, chunkdata, slength);
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
      crc_err = (int)((((((((uint)crc_bytes[0] << 8) + crc_bytes[1]) << 8) + crc_bytes[2]) << 8) + crc_bytes[3])
         != *(uint *)(png_ptr + 0x110));
   else
      crc_err = 0;

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

   for (text = chunkdata; *text; text++)
      ;

   if (text == chunkdata + slength)
   {
      comp_type = -1;
      FUN_00517270(png_ptr, s_Zero_length_zTXt_chunk_00622790);
   }
   else
   {
       comp_type = *(++text);
       if (comp_type != 0)
       {
          FUN_00517270(png_ptr, s_Unknown_compression_type_in_zTXt_00622768);
          comp_type = 0;
       }
       text++;
   }
   prefix_len = text - chunkdata;

   {
   uint data_len;
   chunkdata = FUN_00523360(png_ptr, comp_type, chunkdata, length, prefix_len, &data_len);

   text_ptr = (png_text *)FUN_005208c0(png_ptr, sizeof(png_text));
   text_ptr->compression = comp_type;
   text_ptr->key = chunkdata;
   text_ptr->text = chunkdata + prefix_len;
   text_ptr->text_length = data_len;

   FUN_00516650(png_ptr, info_ptr, text_ptr, 1);

   FUN_00520930(png_ptr, text_ptr);
   FUN_00520930(png_ptr, chunkdata);
   }
}
