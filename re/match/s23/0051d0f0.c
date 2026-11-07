// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051d0f0: libpng 1.0.12 pngwutil.c png_write_IDAT (libpair named png_write_gAMA: wrong), with
   png_write_chunk and its start / data / end and png_save_uint_32 written inline (the original has them inline).
   Same form as 0x0051ca90.
   100.0% (regs 100, ops 100). Residual: none. */
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
MGLOBALA(unsigned char, png_IDAT, 5, 0x005e3224);
void FUN_00516bb0(int png_ptr, unsigned char *data, unsigned int length);
void FUN_00516d30(int png_ptr);
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length);

// MATCH: 0x0051d0f0 FUN_0051d0f0
void FUN_0051d0f0(int png_ptr, unsigned char *data, unsigned int length)
{
   unsigned char buf[4];
   unsigned int crc;

   buf[0] = (unsigned char)((length >> 24) & 0xff);
   buf[1] = (unsigned char)((length >> 16) & 0xff);
   buf[2] = (unsigned char)((length >> 8) & 0xff);
   buf[3] = (unsigned char)(length & 0xff);
   FUN_00516bb0(png_ptr, buf, 4);
   FUN_00516bb0(png_ptr, png_IDAT, 4);
   FUN_00516d30(png_ptr);
   FUN_00516d50(png_ptr, png_IDAT, 4);

   if (data != 0 && length > 0)
   {
      FUN_00516d50(png_ptr, data, length);
      FUN_00516bb0(png_ptr, data, length);
   }

   crc = *(unsigned int *)(png_ptr + 0x110);
   buf[0] = (unsigned char)((crc >> 24) & 0xff);
   buf[1] = (unsigned char)((crc >> 16) & 0xff);
   buf[2] = (unsigned char)((crc >> 8) & 0xff);
   buf[3] = (unsigned char)(crc & 0xff);
   FUN_00516bb0(png_ptr, buf, 4);
   *(unsigned int *)(png_ptr + 0x68) |= 4;
}
