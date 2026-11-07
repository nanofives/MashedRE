// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051db30: libpng 1.0.12 pngwutil.c png_write_sBIT, png_write_chunk and its start / data / end
   written inline.
   100.0% (regs 100, ops 100). Key: sbit copied from sbit_arg so buf packs into the sbit arg slot (66.9 without).
   Residual: none. */
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
typedef struct
{
   unsigned char red;
   unsigned char green;
   unsigned char blue;
   unsigned char gray;
   unsigned char alpha;
} png_color_8;

MGLOBALA(char, s_Invalid_sBIT_depth_specified_00621938, 29, 0x00621938);
MGLOBALA(unsigned char, png_sBIT, 5, 0x005e328c);
void FUN_00516bb0(int png_ptr, unsigned char *data, unsigned int length);
void FUN_00516d30(int png_ptr);
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length);
void FUN_00517270(int png_ptr, const char *msg);

// MATCH: 0x0051db30 FUN_0051db30
void FUN_0051db30(int png_ptr, png_color_8 *sbit_arg, int color_type)
{
   unsigned char buf[4];
   unsigned int size;
   png_color_8 *sbit = sbit_arg;

   if (color_type & 2)
   {
      unsigned char maxbits;

      maxbits = (unsigned char)(color_type == 3 ? 8 : *(unsigned char *)(png_ptr + 0x128));
      if (sbit->red == 0 || sbit->red > maxbits ||
          sbit->green == 0 || sbit->green > maxbits ||
          sbit->blue == 0 || sbit->blue > maxbits)
      {
         FUN_00517270(png_ptr, s_Invalid_sBIT_depth_specified_00621938);
         return;
      }
      buf[0] = sbit->red;
      buf[1] = sbit->green;
      buf[2] = sbit->blue;
      size = 3;
   }
   else
   {
      if (sbit->gray == 0 || sbit->gray > *(unsigned char *)(png_ptr + 0x128))
      {
         FUN_00517270(png_ptr, s_Invalid_sBIT_depth_specified_00621938);
         return;
      }
      buf[0] = sbit->gray;
      size = 1;
   }

   if (color_type & 4)
   {
      if (sbit->alpha == 0 || sbit->alpha > *(unsigned char *)(png_ptr + 0x128))
      {
         FUN_00517270(png_ptr, s_Invalid_sBIT_depth_specified_00621938);
         return;
      }
      buf[size++] = sbit->alpha;
   }

   {
      unsigned char buf[4];
      unsigned int length = size;
      buf[0] = (unsigned char)((length >> 24) & 0xff);
      buf[1] = (unsigned char)((length >> 16) & 0xff);
      buf[2] = (unsigned char)((length >> 8) & 0xff);
      buf[3] = (unsigned char)(length & 0xff);
      FUN_00516bb0(png_ptr, buf, 4);
      FUN_00516bb0(png_ptr, png_sBIT, 4);
      FUN_00516d30(png_ptr);
      FUN_00516d50(png_ptr, png_sBIT, 4);
   }
   if (size > 0)
   {
      FUN_00516d50(png_ptr, buf, size);
      FUN_00516bb0(png_ptr, buf, size);
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
