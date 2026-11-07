// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00525bd0 (h4): libpng 1.0.12 pngrutil.c png_handle_tRNS. Best 72.6% (regs 77.7%).
   Written from the official source with the original's inlining mirrored by hand: png_crc_read, png_crc_finish,
   png_crc_error, png_get_uint_32/16 (and png_check_chunk_name where present) are inline in the original, per call
   site either fully inline or calling png_crc_error 0x005232c0 / png_get_uint_32 0x00523110 / png_crc_finish
   0x00523170; each copy follows the original's form. png_chunk_error = 0x005172b0, png_chunk_warning = 0x00517380.
   Residual class (b): Copies mirrored (zero-length copy = warning + 0x005232c0, gray/alpha copies and the final crc_finish(0) call 0x00523170). Residual: block layout (original places the length==0 check after the num_palette copy, ours right after the PLTE warning; tried if/else orders and flag sets), istop frame-slot order, u16 zero-ext idiom. */
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

MGLOBALA(char, s_Missing_IHDR_before_tRNS_006223ac, 25, 0x006223ac);
MGLOBALA(char, s_Invalid_tRNS_after_IDAT_00622378, 24, 0x00622378);
MGLOBALA(char, s_Duplicate_tRNS_chunk_00622360, 21, 0x00622360);
MGLOBALA(char, s_Missing_PLTE_before_tRNS_00622390, 25, 0x00622390);
MGLOBALA(char, s_Incorrect_tRNS_chunk_length_00622344, 28, 0x00622344);
MGLOBALA(char, s_Zero_length_tRNS_chunk_0062232c, 23, 0x0062232c);
MGLOBALA(char, s_tRNS_chunk_not_allowed_with_alph_00622300, 42, 0x00622300);
MGLOBALA(char, s_CRC_error_00621ce8, 10, 0x00621ce8);
void FUN_00517250(int png_ptr, char *msg);
void FUN_00517270(int png_ptr, char *msg);
void FUN_005172b0(int png_ptr, char *msg);
void FUN_00517380(int png_ptr, char *msg);
void FUN_0051c9f0(int png_ptr, void *buf, uint length);
void FUN_00516d50(int png_ptr, void *buf, uint length);
int FUN_005232c0(int png_ptr);
int FUN_00523170(int png_ptr, uint skip);
void FUN_00516870(int png_ptr, int info_ptr, uchar *trans, int num_trans, void *trans_values);

// MATCH: 0x00525bd0 FUN_00525bd0
void FUN_00525bd0(int png_ptr, int info_ptr, uint length)
{
   uchar readbuf[256];
   int crc_err;

   if (!(*(uint *)(png_ptr + 0x68) & 1))
      FUN_00517250(png_ptr, s_Missing_IHDR_before_tRNS_006223ac);
   else if (*(uint *)(png_ptr + 0x68) & 4)
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Invalid_tRNS_after_IDAT_00622378);
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
      
      return;
   }

      return;
   }
   else if (info_ptr != 0 && (*(uint *)(info_ptr + 8) & 0x10))
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Duplicate_tRNS_chunk_00622360);
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
      
      return;
   }

      return;
   }

   if (*(uchar *)(png_ptr + 0x126) == 3)
   {
      if (!(*(uint *)(png_ptr + 0x68) & 2))
      {
         FUN_00517270(png_ptr, s_Missing_PLTE_before_tRNS_00622390);
      }
      else if (length > (uint)*(ushort *)(png_ptr + 0x118))
      {
         uint i, istop;
         FUN_00517270(png_ptr, s_Incorrect_tRNS_chunk_length_00622344);
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
      
      return;
   }

         return;
      }
      if (length == 0)
      {
         FUN_00517270(png_ptr, s_Zero_length_tRNS_chunk_0062232c);
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
      
      return;
   }

         return;
      }

      FUN_0051c9f0(png_ptr, readbuf, length);
      FUN_00516d50(png_ptr, readbuf, length);
      *(ushort *)(png_ptr + 0x11a) = (ushort)length;
   }
   else if (*(uchar *)(png_ptr + 0x126) == 2)
   {
      uchar buf[6];

      if (length != 6)
      {
         uint i, istop;
         FUN_00517270(png_ptr, s_Incorrect_tRNS_chunk_length_00622344);
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
      
      return;
   }

         return;
      }

      FUN_0051c9f0(png_ptr, buf, length);
      FUN_00516d50(png_ptr, buf, length);
      *(ushort *)(png_ptr + 0x11a) = 1;
      *(ushort *)(png_ptr + 0x18e) = (ushort)(((ushort)(*buf) << 8) + (ushort)(*(buf + 1)));
      *(ushort *)(png_ptr + 0x190) = (ushort)(((ushort)(*(buf + 2)) << 8) + (ushort)(*(buf + 3)));
      *(ushort *)(png_ptr + 0x192) = (ushort)(((ushort)(*(buf + 4)) << 8) + (ushort)(*(buf + 5)));
   }
   else if (*(uchar *)(png_ptr + 0x126) == 0)
   {
      uchar buf[6];

      if (length != 2)
      {
         FUN_00517270(png_ptr, s_Incorrect_tRNS_chunk_length_00622344);
         FUN_00523170(png_ptr, length);
         return;
      }

      FUN_0051c9f0(png_ptr, buf, 2);
      FUN_00516d50(png_ptr, buf, 2);
      *(ushort *)(png_ptr + 0x11a) = 1;
      *(ushort *)(png_ptr + 0x194) = (ushort)(((ushort)(*buf) << 8) + (ushort)(*(buf + 1)));
   }
   else
   {
      FUN_00517270(png_ptr, s_tRNS_chunk_not_allowed_with_alph_00622300);
      FUN_00523170(png_ptr, length);
      return;
   }

   if (FUN_00523170(png_ptr, 0))
      return;

   FUN_00516870(png_ptr, info_ptr, readbuf, *(ushort *)(png_ptr + 0x11a),
      (void *)(png_ptr + 0x18c));
}
