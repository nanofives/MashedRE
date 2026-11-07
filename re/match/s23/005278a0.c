// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x005278a0 (h4): libpng 1.0.12 pngrutil.c png_handle_tEXt. Best 75.8% (regs 90.1%).
   Written from the official source with the original's inlining mirrored by hand: png_crc_read, png_crc_finish,
   png_crc_error, png_get_uint_32/16 (and png_check_chunk_name where present) are inline in the original, per call
   site either fully inline or calling png_crc_error 0x005232c0 / png_get_uint_32 0x00523110 / png_crc_finish
   0x00523170; each copy follows the original's form. png_chunk_error = 0x005172b0, png_chunk_warning = 0x00517380.
   Residual class (a): strlen intrinsic: the original emits `repne scasb` (needs text in edi, so key/length/need_crc move to ebp/edi/ebx); cl/c2 13.00.9176/9178 emits a byte loop under every flag set tried (/O1 /O2 /Ox /Os /G3../G7, pragma intrinsic). Rest of the body is instruction-identical modulo that allocation. */
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

#ifdef __cplusplus
extern "C" unsigned int __cdecl strlen(const char *s);
#else
unsigned int strlen(const char *s);
#endif
#pragma intrinsic(strlen)
typedef struct png_text_s { int compression; char *key; char *text; uint text_length; } png_text;
MGLOBALA(char, s_Missing_IHDR_before_tEXt_0062274c, 25, 0x0062274c);
MGLOBALA(char, s_CRC_error_00621ce8, 10, 0x00621ce8);
void FUN_00517250(int png_ptr, char *msg);
void FUN_005172b0(int png_ptr, char *msg);
void FUN_00517380(int png_ptr, char *msg);
void *FUN_005208c0(int png_ptr, uint size);
void FUN_00520930(int png_ptr, void *p);
void FUN_0051c9f0(int png_ptr, void *buf, uint length);
void FUN_00516d50(int png_ptr, void *buf, uint length);
void FUN_00516650(int png_ptr, int info_ptr, png_text *text, int num);

// MATCH: 0x005278a0 FUN_005278a0
void FUN_005278a0(int png_ptr, int info_ptr, uint length)
{
   png_text *text_ptr;
   char *key;
   char *text;
   uint slength;
   uchar crc_bytes[4];
   int need_crc;
   int crc_err;

   if (!(*(uint *)(png_ptr + 0x68) & 1))
      FUN_00517250(png_ptr, s_Missing_IHDR_before_tEXt_0062274c);

   if (*(uint *)(png_ptr + 0x68) & 4)
      *(uint *)(png_ptr + 0x68) |= 8;

   key = (char *)FUN_005208c0(png_ptr, length + 1);
   slength = length;
   FUN_0051c9f0(png_ptr, key, slength);
   FUN_00516d50(png_ptr, key, slength);
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
      FUN_00520930(png_ptr, key);
      return;
   }

   key[slength] = 0x00;

   for (text = key; *text; text++)
      ;

   if (text != key + slength)
      text++;

   text_ptr = (png_text *)FUN_005208c0(png_ptr, sizeof(png_text));
   text_ptr->compression = -1;
   text_ptr->key = key;
   text_ptr->text = text;
   text_ptr->text_length = strlen(text);

   FUN_00516650(png_ptr, info_ptr, text_ptr, 1);

   FUN_00520930(png_ptr, key);
   FUN_00520930(png_ptr, text_ptr);
}
