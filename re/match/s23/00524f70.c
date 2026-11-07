// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00524f70 (h4): libpng 1.0.12 pngrutil.c png_handle_sRGB. Best 88.7% (regs 88.7%, h6 round 2, was 86.5).
   Written from the official source with the original's inlining mirrored by hand: png_crc_read, png_crc_finish,
   png_crc_error, png_get_uint_32/16 (and png_check_chunk_name where present) are inline in the original, per call
   site either fully inline or calling png_crc_error 0x005232c0 / png_get_uint_32 0x00523110 / png_crc_finish
   0x00523170; each copy follows the original's form. png_chunk_error = 0x005172b0, png_chunk_warning = 0x00517380.
   Residual class (b): Residual: param-slot choice (original even packs buf[1] into png_ptr's slot, istop into info_ptr/length) + flags-load hoist in the IDAT copy.
   Round 2 (h6): info_ptr + length arg copies fix the istop/crc_bytes/buf param-slot packing; only the class (a) flags-load hoist is left. */
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
extern "C" int __cdecl abs(int v);
#else
int abs(int v);
#endif
#pragma intrinsic(abs)
MGLOBALA(char, s_Missing_IHDR_before_sRGB_00622180, 25, 0x00622180);
MGLOBALA(char, s_Invalid_sRGB_after_IDAT_0062214c, 24, 0x0062214c);
MGLOBALA(char, s_Out_of_place_sRGB_chunk_00622134, 24, 0x00622134);
MGLOBALA(char, s_Duplicate_sRGB_chunk_0062211c, 21, 0x0062211c);
MGLOBALA(char, s_Incorrect_sRGB_chunk_length_00622164, 28, 0x00622164);
MGLOBALA(char, s_Unknown_sRGB_intent_00622108, 20, 0x00622108);
MGLOBALA(char, s_Ignoring_incorrect_gAMA_value_wh_00621e88, 57, 0x00621e88);
MGLOBALA(char, s_Ignoring_incorrect_cHRM_value_wh_0062204c, 57, 0x0062204c);
MGLOBALA(char, s_CRC_error_00621ce8, 10, 0x00621ce8);
void FUN_00517250(int png_ptr, char *msg);
void FUN_00517270(int png_ptr, char *msg);
void FUN_005172b0(int png_ptr, char *msg);
void FUN_00517380(int png_ptr, char *msg);
void FUN_0051c9f0(int png_ptr, void *buf, uint length);
void FUN_00516d50(int png_ptr, void *buf, uint length);
uint FUN_00523110(uchar *buf);
int FUN_005232c0(int png_ptr);
void FUN_00516460(int png_ptr, int info_ptr, int intent);

// MATCH: 0x00524f70 FUN_00524f70
void FUN_00524f70(int png_ptr, int info_ptr_arg, uint length_arg)
{
   int info_ptr = info_ptr_arg;
   uint length = length_arg;
   int intent;
   uchar buf[1];
   int crc_err;

   if (!(*(uint *)(png_ptr + 0x68) & 1))
      FUN_00517250(png_ptr, s_Missing_IHDR_before_sRGB_00622180);
   else if (*(uint *)(png_ptr + 0x68) & 4)
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Invalid_sRGB_after_IDAT_0062214c);
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
      crc_err = (int)((((((((uint)crc_bytes[0] << 8) + crc_bytes[1]) << 8) + crc_bytes[2]) << 8) + crc_bytes[3])
         != *(uint *)(png_ptr + 0x110));
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
   else if (*(uint *)(png_ptr + 0x68) & 2)
      FUN_00517270(png_ptr, s_Out_of_place_sRGB_chunk_00622134);

   else if (info_ptr != 0 && (*(uint *)(info_ptr + 8) & 0x800))
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Duplicate_sRGB_chunk_0062211c);
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

   if (length != 1)
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Incorrect_sRGB_chunk_length_00622164);
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

   FUN_0051c9f0(png_ptr, buf, 1);
   FUN_00516d50(png_ptr, buf, 1);
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


   intent = buf[0];
   if (intent >= 4)
   {
      FUN_00517270(png_ptr, s_Unknown_sRGB_intent_00622108);
      return;
   }

   if ((*(uint *)(info_ptr + 8) & 1))
   {
   int igamma;
      igamma = (int)*(int *)(info_ptr + 0xfc);
      if (igamma < 45000L || igamma > 46000L)
      {
         FUN_00517270(png_ptr, s_Ignoring_incorrect_gAMA_value_wh_00621e88);
      }
   }

   if (*(uint *)(info_ptr + 8) & 4)
      if (abs(*(int *)(info_ptr + 0x100) - 31270L) > 1000 ||
          abs(*(int *)(info_ptr + 0x104) - 32900L) > 1000 ||
          abs(*(int *)(info_ptr + 0x108) - 64000L) > 1000 ||
          abs(*(int *)(info_ptr + 0x10c) - 33000L) > 1000 ||
          abs(*(int *)(info_ptr + 0x110) - 30000L) > 1000 ||
          abs(*(int *)(info_ptr + 0x114) - 60000L) > 1000 ||
          abs(*(int *)(info_ptr + 0x118) - 15000L) > 1000 ||
          abs(*(int *)(info_ptr + 0x11c) -  6000L) > 1000)
         {
            FUN_00517270(png_ptr, s_Ignoring_incorrect_cHRM_value_wh_0062204c);
         }

   FUN_00516460(png_ptr, info_ptr, intent);
}
