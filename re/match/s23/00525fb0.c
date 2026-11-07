// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00525fb0 (h4): libpng 1.0.12 pngrutil.c png_handle_bKGD. Best 86.7% (regs 88.8%).
   Written from the official source with the original's inlining mirrored by hand: png_crc_read, png_crc_finish,
   png_crc_error, png_get_uint_32/16 (and png_check_chunk_name where present) are inline in the original, per call
   site either fully inline or calling png_crc_error 0x005232c0 / png_get_uint_32 0x00523110 / png_crc_finish
   0x00523170; each copy follows the original's form. png_chunk_error = 0x005172b0, png_chunk_warning = 0x00517380.
   Residual class (a): All copies call png_crc_error 0x005232c0. Residual: u16 zero-extension idiom in the background math (original xor r16/mov r8 and dword/word loads of buf, ours movzx), istop slot of the length-check copy. */
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

typedef struct png_color_s { uchar red; uchar green; uchar blue; } png_color;
MGLOBALA(char, s_Missing_IHDR_before_bKGD_00622454, 25, 0x00622454);
MGLOBALA(char, s_Invalid_bKGD_after_IDAT_0062243c, 24, 0x0062243c);
MGLOBALA(char, s_Missing_PLTE_before_bKGD_00622420, 25, 0x00622420);
MGLOBALA(char, s_Duplicate_bKGD_chunk_00622408, 21, 0x00622408);
MGLOBALA(char, s_Incorrect_bKGD_chunk_length_006223ec, 28, 0x006223ec);
MGLOBALA(char, s_Incorrect_bKGD_chunk_index_value_006223c8, 33, 0x006223c8);
MGLOBALA(char, s_CRC_error_00621ce8, 10, 0x00621ce8);
void FUN_00517250(int png_ptr, char *msg);
void FUN_00517270(int png_ptr, char *msg);
void FUN_005172b0(int png_ptr, char *msg);
void FUN_00517380(int png_ptr, char *msg);
void FUN_0051c9f0(int png_ptr, void *buf, uint length);
void FUN_00516d50(int png_ptr, void *buf, uint length);
int FUN_005232c0(int png_ptr);
void FUN_00515aa0(int png_ptr, int info_ptr, void *background);

// MATCH: 0x00525fb0 FUN_00525fb0
void FUN_00525fb0(int png_ptr, int info_ptr, uint length)
{
   uint truelen;
   uchar buf[6];
   int crc_err;

   if (!(*(uint *)(png_ptr + 0x68) & 1))
      FUN_00517250(png_ptr, s_Missing_IHDR_before_bKGD_00622454);
   else if (*(uint *)(png_ptr + 0x68) & 4)
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Invalid_bKGD_after_IDAT_0062243c);
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
   else if (*(uchar *)(png_ptr + 0x126) == 3 &&
            !(*(uint *)(png_ptr + 0x68) & 2))
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Missing_PLTE_before_bKGD_00622420);
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
   else if (info_ptr != 0 && (*(uint *)(info_ptr + 8) & 0x20))
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Duplicate_bKGD_chunk_00622408);
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
      truelen = 1;
   else if (*(uchar *)(png_ptr + 0x126) & 2)
      truelen = 6;
   else
      truelen = 2;

   if (length != truelen)
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Incorrect_bKGD_chunk_length_006223ec);
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

   FUN_0051c9f0(png_ptr, buf, truelen);
   FUN_00516d50(png_ptr, buf, truelen);
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


   if (*(uchar *)(png_ptr + 0x126) == 3)
   {
      *(uchar *)(png_ptr + 0x138) = buf[0];
      if (*(ushort *)(info_ptr + 0x14))
      {
          if (buf[0] > *(ushort *)(info_ptr + 0x14))
          {
             FUN_00517270(png_ptr, s_Incorrect_bKGD_chunk_index_value_006223c8);
             return;
          }
          *(ushort *)(png_ptr + 0x13a) = (ushort)(*(png_color **)(png_ptr + 0x114))[buf[0]].red;
          *(ushort *)(png_ptr + 0x13c) = (ushort)(*(png_color **)(png_ptr + 0x114))[buf[0]].green;
          *(ushort *)(png_ptr + 0x13e) = (ushort)(*(png_color **)(png_ptr + 0x114))[buf[0]].blue;
      }
   }
   else if (!(*(uchar *)(png_ptr + 0x126) & 2))
   {
      *(ushort *)(png_ptr + 0x13a) =
      *(ushort *)(png_ptr + 0x13c) =
      *(ushort *)(png_ptr + 0x13e) =
      *(ushort *)(png_ptr + 0x140) = (ushort)(((ushort)(*buf) << 8) + (ushort)(*(buf + 1)));
   }
   else
   {
      *(ushort *)(png_ptr + 0x13a) = (ushort)(((ushort)(*buf) << 8) + (ushort)(*(buf + 1)));
      *(ushort *)(png_ptr + 0x13c) = (ushort)(((ushort)(*(buf + 2)) << 8) + (ushort)(*(buf + 3)));
      *(ushort *)(png_ptr + 0x13e) = (ushort)(((ushort)(*(buf + 4)) << 8) + (ushort)(*(buf + 5)));
   }

   FUN_00515aa0(png_ptr, info_ptr, (void *)(png_ptr + 0x138));
}
