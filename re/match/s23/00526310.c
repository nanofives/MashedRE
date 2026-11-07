// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00526310 (h4): libpng 1.0.12 pngrutil.c png_handle_hIST. Best 92.0% (regs 92.0%).
   Written from the official source with the original's inlining mirrored by hand: png_crc_read, png_crc_finish,
   png_crc_error, png_get_uint_32/16 (and png_check_chunk_name where present) are inline in the original, per call
   site either fully inline or calling png_crc_error 0x005232c0 / png_get_uint_32 0x00523110 / png_crc_finish
   0x00523170; each copy follows the original's form. png_chunk_error = 0x005172b0, png_chunk_warning = 0x00517380.
   Residual class (b): Same family as pHYs. Residual: flags-load hoist in the IDAT copy; png_get_uint_16 zero-extension `xor dx,dx; mov dl,al` (original) vs `movzx dx,al` (ours), unchanged by 4 spellings and /G3../G7. */
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

MGLOBALA(char, s_Missing_IHDR_before_hIST_006224d8, 25, 0x006224d8);
MGLOBALA(char, s_Invalid_hIST_after_IDAT_006224a4, 24, 0x006224a4);
MGLOBALA(char, s_Missing_PLTE_before_hIST_00622488, 25, 0x00622488);
MGLOBALA(char, s_Duplicate_hIST_chunk_00622470, 21, 0x00622470);
MGLOBALA(char, s_Incorrect_hIST_chunk_length_006224bc, 28, 0x006224bc);
MGLOBALA(char, s_CRC_error_00621ce8, 10, 0x00621ce8);
void FUN_00517250(int png_ptr, char *msg);
void FUN_00517270(int png_ptr, char *msg);
void FUN_005172b0(int png_ptr, char *msg);
void FUN_00517380(int png_ptr, char *msg);
void FUN_0051c9f0(int png_ptr, void *buf, uint length);
void FUN_00516d50(int png_ptr, void *buf, uint length);
uint FUN_00523110(uchar *buf);
int FUN_005232c0(int png_ptr);
void FUN_00515f30(int png_ptr, int info_ptr, ushort *hist);

// MATCH: 0x00526310 FUN_00526310
void FUN_00526310(int png_ptr, int info_ptr, uint length)
{
   int num, j;
   ushort readbuf[256];
   int crc_err;

   if (!(*(uint *)(png_ptr + 0x68) & 1))
      FUN_00517250(png_ptr, s_Missing_IHDR_before_hIST_006224d8);
   else if (*(uint *)(png_ptr + 0x68) & 4)
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Invalid_hIST_after_IDAT_006224a4);
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
   else if (!(*(uint *)(png_ptr + 0x68) & 2))
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Missing_PLTE_before_hIST_00622488);
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
   else if (info_ptr != 0 && (*(uint *)(info_ptr + 8) & 0x40))
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Duplicate_hIST_chunk_00622470);
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

   num = (int)length / 2;
   if (num != *(ushort *)(png_ptr + 0x118))
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Incorrect_hIST_chunk_length_006224bc);
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

   for (j = 0; j < num; j++)
   {
      uchar buf[2];

      FUN_0051c9f0(png_ptr, buf, 2);
      FUN_00516d50(png_ptr, buf, 2);
      readbuf[j] = (ushort)(((ushort)(*buf) << 8) + (ushort)(*(buf + 1)));
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


   FUN_00515f30(png_ptr, info_ptr, readbuf);
}
