// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00526bf0 (h4): libpng 1.0.12 pngrutil.c png_handle_pCAL. Best 78.3% (regs 78.6%).
   Written from the official source with the original's inlining mirrored by hand: png_crc_read, png_crc_finish,
   png_crc_error, png_get_uint_32/16 (and png_check_chunk_name where present) are inline in the original, per call
   site either fully inline or calling png_crc_error 0x005232c0 / png_get_uint_32 0x00523110 / png_crc_finish
   0x00523170; each copy follows the original's form. png_chunk_error = 0x005172b0, png_chunk_warning = 0x00517380.
   Residual class (b): X0/X1 png_get_int_32 inline (matches the xor/mov idiom). Residual: frame 0x18 vs 0x14 - the original keeps nparams in png_ptr's dead slot, so every esp offset shifts by 4; plus istop/crc_bytes slots and the hoist. Tried declaration order and block scope. */
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

MGLOBALA(char, s_Missing_IHDR_before_pCAL_00622634, 25, 0x00622634);
MGLOBALA(char, s_Invalid_pCAL_after_IDAT_0062261c, 24, 0x0062261c);
MGLOBALA(char, s_Duplicate_pCAL_chunk_00622604, 21, 0x00622604);
MGLOBALA(char, s_Invalid_pCAL_data_006225f0, 18, 0x006225f0);
MGLOBALA(char, s_Invalid_pCAL_parameters_for_equa_006225c4, 42, 0x006225c4);
MGLOBALA(char, s_Unrecognized_equation_type_for_p_00621be0, 42, 0x00621be0);
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
void FUN_005161d0(int png_ptr, int info_ptr, char *purpose, int X0, int X1, int type, int nparams, char *units, char **params);

// MATCH: 0x00526bf0 FUN_00526bf0
void FUN_00526bf0(int png_ptr, int info_ptr, uint length)
{
   char *purpose;
   int X0, X1;
   uchar type, nparams;
   char *buf, *units, *endptr;
   char **params;
   uint slength;
   int j;
   int crc_err;

   if (!(*(uint *)(png_ptr + 0x68) & 1))
      FUN_00517250(png_ptr, s_Missing_IHDR_before_pCAL_00622634);
   else if (*(uint *)(png_ptr + 0x68) & 4)
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Invalid_pCAL_after_IDAT_0062261c);
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
   else if (info_ptr != 0 && (*(uint *)(info_ptr + 8) & 0x400))
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Duplicate_pCAL_chunk_00622604);
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

   purpose = (char *)FUN_005208c0(png_ptr, length + 1);
   slength = length;
   FUN_0051c9f0(png_ptr, purpose, slength);
   FUN_00516d50(png_ptr, purpose, slength);
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
      FUN_00520930(png_ptr, purpose);
      return;
   }


   purpose[slength] = 0x00;

   for (buf = purpose; *buf; buf++)
      ;

   endptr = purpose + slength;

   if (endptr <= buf + 12)
   {
      FUN_00517270(png_ptr, s_Invalid_pCAL_data_006225f0);
      FUN_00520930(png_ptr, purpose);
      return;
   }

   X0 = (((((((int)((uchar *)buf)[1] << 8) + ((uchar *)buf)[2]) << 8) + ((uchar *)buf)[3]) << 8) + ((uchar *)buf)[4]);
   X1 = (((((((int)((uchar *)buf)[5] << 8) + ((uchar *)buf)[6]) << 8) + ((uchar *)buf)[7]) << 8) + ((uchar *)buf)[8]);
   type = buf[9];
   nparams = buf[10];
   units = buf + 11;

   if ((type == 0 && nparams != 2) ||
       (type == 1 && nparams != 3) ||
       (type == 2 && nparams != 3) ||
       (type == 3 && nparams != 4))
   {
      FUN_00517270(png_ptr, s_Invalid_pCAL_parameters_for_equa_006225c4);
      FUN_00520930(png_ptr, purpose);
      return;
   }
   else if (type >= 4)
   {
      FUN_00517270(png_ptr, s_Unrecognized_equation_type_for_p_00621be0);
   }

   for (buf = units; *buf; buf++)
      ;

   params = (char **)FUN_005208c0(png_ptr, (uint)(nparams
      *sizeof(char *))) ;

   for (j = 0; j < (int)nparams; j++)
   {
      buf++;

      for (params[j] = buf; *buf != 0x00 && buf <= endptr; buf++)
         ;

      if (buf > endptr)
      {
         FUN_00517270(png_ptr, s_Invalid_pCAL_data_006225f0);
         FUN_00520930(png_ptr, purpose);
         FUN_00520930(png_ptr, params);
         return;
      }
   }

   FUN_005161d0(png_ptr, info_ptr, purpose, X0, X1, type, nparams,
      units, params);

   FUN_00520930(png_ptr, purpose);
   FUN_00520930(png_ptr, params);
}
