// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051e6f0: libpng 1.0.12 pngwutil.c png_write_hIST, png_write_chunk_start / _data / _end and
   png_save_uint_16 written inline.
   100.0% (regs 100, ops 100). Keys: num_hist copied from num_hist_arg (frees that slot for the length/crc buffers),
   block-scoped buffer per inlined helper. Residual: none. */
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
MGLOBALA(char, s_Invalid_number_of_histogram_entr_00621a64, 46, 0x00621a64);
MGLOBALA(unsigned char, png_hIST, 5, 0x005e3254);
void FUN_00516bb0(int png_ptr, unsigned char *data, unsigned int length);
void FUN_00516d30(int png_ptr);
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length);
void FUN_00517270(int png_ptr, const char *msg);

// MATCH: 0x0051e6f0 FUN_0051e6f0
void FUN_0051e6f0(int png_ptr, unsigned short *hist, int num_hist_arg)
{
   int i;
   unsigned char buf[3];
   int num_hist = num_hist_arg;

   if (num_hist > (int)*(unsigned short *)(png_ptr + 0x118))
   {
      FUN_00517270(png_ptr, s_Invalid_number_of_histogram_entr_00621a64);
      return;
   }

   {
      unsigned char buf[4];
      unsigned int length = (unsigned int)(num_hist * 2);
      buf[0] = (unsigned char)((length >> 24) & 0xff);
      buf[1] = (unsigned char)((length >> 16) & 0xff);
      buf[2] = (unsigned char)((length >> 8) & 0xff);
      buf[3] = (unsigned char)(length & 0xff);
      FUN_00516bb0(png_ptr, buf, 4);
      FUN_00516bb0(png_ptr, png_hIST, 4);
      FUN_00516d30(png_ptr);
      FUN_00516d50(png_ptr, png_hIST, 4);
   }
   for (i = 0; i < num_hist; i++)
   {
      {
         unsigned int v = hist[i];
         buf[0] = (unsigned char)((v >> 8) & 0xff);
         buf[1] = (unsigned char)(v & 0xff);
      }
      FUN_00516d50(png_ptr, buf, 2);
      FUN_00516bb0(png_ptr, buf, 2);
   }
   {
      unsigned char buf[4];
      unsigned int crc = *(unsigned int *)(png_ptr + 0x110);
      buf[0] = (unsigned char)((crc >> 24) & 0xff);
      buf[1] = (unsigned char)((crc >> 16) & 0xff);
      buf[2] = (unsigned char)((crc >> 8) & 0xff);
      buf[3] = (unsigned char)(crc & 0xff);
      FUN_00516bb0(png_ptr, buf, 4);
   }
}
