// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00520640: libpng 1.0.12 pngwutil.c png_write_filtered_row, with png_write_IDAT ->
   png_write_chunk (chunk_start / chunk_data / chunk_end, png_save_uint_32) written inline.
   100.0% (regs 100, ops 100) first pass from the official source. relocheck OK, C++ OK. */
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
MGLOBALA(unsigned char, DAT_005e3224, 5, 0x005e3224); /* png_IDAT */
MGLOBALA(char, s_zlib_error_00620f0c, 11, 0x00620f0c);
void FUN_005150e0(char *png_ptr);
void FUN_00516bb0(char *png_ptr, unsigned char *data, uint length);
void FUN_00516d30(char *png_ptr);
void FUN_00516d50(char *png_ptr, unsigned char *ptr, uint length);
void FUN_00517250(char *png_ptr, const char *msg);
void FUN_0051f4f0(char *png_ptr);
int FUN_00521520(char *strm, int flush);
// MATCH: 0x00520640 FUN_00520640
void FUN_00520640(char *png_ptr, unsigned char *filtered_row)
{
   *(unsigned char **)(png_ptr + 0x74) = filtered_row;
   *(uint *)(png_ptr + 0x78) = *(uint *)(png_ptr + 0x104) + 1;
   do
   {
      int ret;

      ret = FUN_00521520(png_ptr + 0x74, 0);
      if (ret != 0)
      {
         if (*(char **)(png_ptr + 0x8c) != 0)
            FUN_00517250(png_ptr, *(char **)(png_ptr + 0x8c));
         else
            FUN_00517250(png_ptr, s_zlib_error_00620f0c);
      }

      if (!*(uint *)(png_ptr + 0x84))
      {
         unsigned char *data = *(unsigned char **)(png_ptr + 0xac);
         uint length = *(uint *)(png_ptr + 0xb0);
         {
            unsigned char buf[4];

            buf[0] = (unsigned char)((length >> 24) & 0xff);
            buf[1] = (unsigned char)((length >> 16) & 0xff);
            buf[2] = (unsigned char)((length >> 8) & 0xff);
            buf[3] = (unsigned char)(length & 0xff);
            FUN_00516bb0(png_ptr, buf, 4);
            FUN_00516bb0(png_ptr, DAT_005e3224, 4);
            FUN_00516d30(png_ptr);
            FUN_00516d50(png_ptr, DAT_005e3224, 4);
         }
         if (data != 0 && length > 0)
         {
            FUN_00516d50(png_ptr, data, length);
            FUN_00516bb0(png_ptr, data, length);
         }
         {
            unsigned char buf[4];
            uint i = *(uint *)(png_ptr + 0x110);

            buf[0] = (unsigned char)((i >> 24) & 0xff);
            buf[1] = (unsigned char)((i >> 16) & 0xff);
            buf[2] = (unsigned char)((i >> 8) & 0xff);
            buf[3] = (unsigned char)(i & 0xff);
            FUN_00516bb0(png_ptr, buf, 4);
         }
         *(uint *)(png_ptr + 0x68) |= 4;
         *(unsigned char **)(png_ptr + 0x80) = *(unsigned char **)(png_ptr + 0xac);
         *(uint *)(png_ptr + 0x84) = *(uint *)(png_ptr + 0xb0);
      }
   } while (*(uint *)(png_ptr + 0x78));

   if (*(unsigned char **)(png_ptr + 0xe8) != 0)
   {
      unsigned char *tptr;

      tptr = *(unsigned char **)(png_ptr + 0xe8);
      *(unsigned char **)(png_ptr + 0xe8) = *(unsigned char **)(png_ptr + 0xec);
      *(unsigned char **)(png_ptr + 0xec) = tptr;
   }

   FUN_0051f4f0(png_ptr);

   (*(uint *)(png_ptr + 0x154))++;

   if (*(uint *)(png_ptr + 0x150) > 0 &&
       *(uint *)(png_ptr + 0x154) >= *(uint *)(png_ptr + 0x150))
   {
      FUN_005150e0(png_ptr);
   }
}
