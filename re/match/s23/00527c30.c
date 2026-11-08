// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00527c30 (h4): libpng 1.0.12 pngrutil.c png_handle_unknown. 100.0% (regs 100.0%) after the round-2 info_ptr arg copy (h6; was 99.2).
   Written from the official source with the original's inlining mirrored by hand: png_crc_read, png_crc_finish,
   png_crc_error, png_get_uint_32/16 (and png_check_chunk_name where present) are inline in the original, per call
   site either fully inline or calling png_crc_error 0x005232c0 / png_get_uint_32 0x00523110 / png_crc_finish
   0x00523170; each copy follows the original's form. png_chunk_error = 0x005172b0, png_chunk_warning = 0x00517380.
   Residual class (b): png_check_chunk_name and png_crc_finish(skip) inlined by hand; png_unknown_chunk needs its trailing `location` byte (frame 0x1c). Round 1 residual (istop spill in png_ptr's dead slot vs info_ptr's) fixed by `int info_ptr = info_ptr_arg;` (h6 round 2), which frees info_ptr's slot. */
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
extern "C" char *__cdecl strcpy(char *d, const char *s);
extern "C" int __cdecl memcmp(const void *a, const void *b, unsigned int n);
#else
char *strcpy(char *d, const char *s);
int memcmp(const void *a, const void *b, unsigned int n);
#endif
#pragma intrinsic(strcpy, memcmp)
typedef struct png_unknown_chunk_s { uchar name[5]; uchar *data; uint size; uchar location; } png_unknown_chunk;
typedef int (*png_user_chunk_ptr)(int png_ptr, png_unknown_chunk *chunk);
MGLOBALA(uchar, png_IDAT, 5, 0x005e3224);
MGLOBALA(char, s_invalid_chunk_type_006227dc, 19, 0x006227dc);
MGLOBALA(char, s_unknown_critical_chunk_006227c4, 23, 0x006227c4);
MGLOBALA(char, s_CRC_error_00621ce8, 10, 0x00621ce8);
int FUN_00517200(int png_ptr, uchar *chunk_name);
void FUN_005172b0(int png_ptr, char *msg);
void FUN_00517380(int png_ptr, char *msg);
void *FUN_005208c0(int png_ptr, uint size);
void FUN_00520930(int png_ptr, void *p);
void FUN_0051c9f0(int png_ptr, void *buf, uint length);
void FUN_00516d50(int png_ptr, void *buf, uint length);
void FUN_00516a70(int png_ptr, int info_ptr, png_unknown_chunk *unknowns, int num);

// MATCH: 0x00527c30 FUN_00527c30
void FUN_00527c30(int png_ptr, int info_ptr_arg, uint length)
{
   int info_ptr = info_ptr_arg;
   uint skip = 0;
   uint i, istop;
   uchar crc_bytes[4];
   int need_crc;
   int crc_err;
   uchar *chunk_name;

   if (*(uint *)(png_ptr + 0x68) & 4)
   {
      if (memcmp((uchar *)(png_ptr + 0x11c), png_IDAT, 4))
         *(uint *)(png_ptr + 0x68) |= 8;
   }

   chunk_name = (uchar *)(png_ptr + 0x11c);
   if (((chunk_name[0]) < 41 || (chunk_name[0]) > 122 || ((chunk_name[0]) > 90 && (chunk_name[0]) < 97)) ||
       ((chunk_name[1]) < 41 || (chunk_name[1]) > 122 || ((chunk_name[1]) > 90 && (chunk_name[1]) < 97)) ||
       ((chunk_name[2]) < 41 || (chunk_name[2]) > 122 || ((chunk_name[2]) > 90 && (chunk_name[2]) < 97)) ||
       ((chunk_name[3]) < 41 || (chunk_name[3]) > 122 || ((chunk_name[3]) > 90 && (chunk_name[3]) < 97)))
   {
      FUN_005172b0(png_ptr, s_invalid_chunk_type_006227dc);
   }

   if (!(*(uchar *)(png_ptr + 0x11c) & 0x20))
   {
      if (FUN_00517200(png_ptr, (uchar *)(png_ptr + 0x11c)) != 3
           && *(png_user_chunk_ptr *)(png_ptr + 0x1cc) == 0)
          FUN_005172b0(png_ptr, s_unknown_critical_chunk_006227c4);
   }

   if (*(uint *)(png_ptr + 0x6c) & 0x8000)
   {
       png_unknown_chunk chunk;

       strcpy((char *)chunk.name, (char *)(png_ptr + 0x11c));
       chunk.data = (uchar *)FUN_005208c0(png_ptr, length);
       chunk.size = length;
       {
          uchar *buf = chunk.data;
          FUN_0051c9f0(png_ptr, buf, length);
          FUN_00516d50(png_ptr, buf, length);
       }
       if (*(png_user_chunk_ptr *)(png_ptr + 0x1cc) != 0)
       {
          if ((*(*(png_user_chunk_ptr *)(png_ptr + 0x1cc)))(png_ptr, &chunk) <= 0)
          {
             if (!(*(uchar *)(png_ptr + 0x11c) & 0x20))
                if (FUN_00517200(png_ptr, (uchar *)(png_ptr + 0x11c)) != 3)
                   FUN_005172b0(png_ptr, s_unknown_critical_chunk_006227c4);
             FUN_00516a70(png_ptr, info_ptr, &chunk, 1);
          }
       }
       else
          FUN_00516a70(png_ptr, info_ptr, &chunk, 1);
       FUN_00520930(png_ptr, chunk.data);
   }
   else
      skip = length;

   istop = *(uint *)(png_ptr + 0xb0);
   for (i = skip; i > istop; i -= istop)
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
      
      return;
   }

}
