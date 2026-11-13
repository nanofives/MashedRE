// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x005253c0 (h4): libpng 1.0.12 pngrutil.c png_handle_iCCP. Best 90.7% (regs 90.7%, h6 round 2, was 88.6).
   Written from the official source with the original's inlining mirrored by hand: png_crc_read, png_crc_finish,
   png_crc_error, png_get_uint_32/16 (and png_check_chunk_name where present) are inline in the original, per call
   site either fully inline or calling png_crc_error 0x005232c0 / png_get_uint_32 0x00523110 / png_crc_finish
   0x00523170; each copy follows the original's form. png_chunk_error = 0x005172b0, png_chunk_warning = 0x00517380.
   Residual class (b): Raw converter failed; written from libpng source. profile_size in the nested shl-8 form. data_length in an inner block. Residual: hoist + istop/data_length slots (original info_ptr/length slots, ours png_ptr slot).
   Round 2 (h6): info_ptr + length arg copies fix the istop/data_length slots, and the official <<24 profile_size form through a block-local p = chunkdata+prefix_length gives the original's lea/movsx order; only the class (a) flags-load hoist is left. */
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

MGLOBALA(char, s_Missing_IHDR_before_iCCP_0062227c, 25, 0x0062227c);
MGLOBALA(char, s_Invalid_iCCP_after_IDAT_00622264, 24, 0x00622264);
MGLOBALA(char, s_Out_of_place_iCCP_chunk_0062224c, 24, 0x0062224c);
MGLOBALA(char, s_Duplicate_iCCP_chunk_00622234, 21, 0x00622234);
MGLOBALA(char, s_Malformed_iCCP_chunk_0062221c, 21, 0x0062221c);
MGLOBALA(char, s_Ignoring_nonzero_compression_typ_006221ec, 48, 0x006221ec);
MGLOBALA(char, s_Profile_size_field_missing_from_006221c0, 43, 0x006221c0);
MGLOBALA(char, s_Ignoring_truncated_iCCP_profile_0062219c, 35, 0x0062219c);
MGLOBALA(char, s_CRC_error_00621ce8, 10, 0x00621ce8);
void FUN_00517250(int png_ptr, char *msg);
void FUN_00517270(int png_ptr, char *msg);
void FUN_005172b0(int png_ptr, char *msg);
void FUN_00517380(int png_ptr, char *msg);
void *FUN_005208c0(int png_ptr, uint size);
void FUN_00520930(int png_ptr, void *p);
void FUN_0051c9f0(int png_ptr, void *buf, uint length);
void FUN_00516d50(int png_ptr, void *buf, uint length);
char *FUN_00523360(int png_ptr, int comp_type, char *chunkdata, uint chunklength, uint prefix_size, uint *newlength);
void FUN_00516570(int png_ptr, int info_ptr, char *name, int compression_type, char *profile, uint proflen);

// MATCH: 0x005253c0 FUN_005253c0
void FUN_005253c0(int png_ptr, int info_ptr_arg, uint length_arg)
{
   int info_ptr = info_ptr_arg;
   uint length = length_arg;
   char *chunkdata;
   uchar compression_type;
   char *profile;
   uint profile_size = 0;
   uint profile_length = 0;
   uint slength, prefix_length;
   int crc_err;

   if (!(*(uint *)(png_ptr + 0x68) & 1))
      FUN_00517250(png_ptr, s_Missing_IHDR_before_iCCP_0062227c);
   else if (*(uint *)(png_ptr + 0x68) & 4)
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Invalid_iCCP_after_IDAT_00622264);
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
      FUN_00517270(png_ptr, s_Out_of_place_iCCP_chunk_0062224c);

   else if (info_ptr != 0 && (*(uint *)(info_ptr + 8) & 0x1000))
   {
      uint i, istop;
      FUN_00517270(png_ptr, s_Duplicate_iCCP_chunk_00622234);
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

   chunkdata = (char *)FUN_005208c0(png_ptr, length + 1);
   slength = length;
   FUN_0051c9f0(png_ptr, chunkdata, slength);
   FUN_00516d50(png_ptr, chunkdata, slength);
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
      FUN_00520930(png_ptr, chunkdata);
      return;
   }


   chunkdata[slength] = 0x00;

   for (profile = chunkdata; *profile; profile++)
      ;

   ++profile;

   if ( profile >= chunkdata + slength)
   {
      FUN_00520930(png_ptr, chunkdata);
      FUN_00517270(png_ptr, s_Malformed_iCCP_chunk_0062221c);
      return;
   }

   compression_type = *profile++;
   if (compression_type)
   {
      FUN_00517270(png_ptr, s_Ignoring_nonzero_compression_typ_006221ec);
      compression_type=0x00;
   }

   prefix_length = profile - chunkdata;
   {
   uint data_length;
   chunkdata = FUN_00523360(png_ptr, compression_type, chunkdata,
                                    slength, prefix_length, &data_length);

   profile_length = data_length - prefix_length;
   }

   if ( profile_length < 4)
   {
      FUN_00520930(png_ptr, chunkdata);
      FUN_00517270(png_ptr, s_Profile_size_field_missing_from_006221c0);
      return;
   }


   {
   char *p = chunkdata+prefix_length;
   profile_size = ((*p)<<24) | ((*(p+1))<<16) | ((*(p+2))<< 8) | ((*(p+3)));
   }

   if(profile_size < profile_length)
      profile_length = profile_size;

   if(profile_size > profile_length)
   {
      FUN_00520930(png_ptr, chunkdata);
      FUN_00517270(png_ptr, s_Ignoring_truncated_iCCP_profile_0062219c);
      return;
   }

   FUN_00516570(png_ptr, info_ptr, chunkdata, compression_type,
                chunkdata + prefix_length, profile_length);
   FUN_00520930(png_ptr, chunkdata);
}
