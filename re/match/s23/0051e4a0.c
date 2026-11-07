// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051e4a0: libpng 1.0.12 pngwutil.c png_write_bKGD (libpair: not paired; PNG_MNG_FEATURES_SUPPORTED).
   png_write_chunk and its start / data / end written inline in all three branches, png_save_uint_16 inline. In the
   palette branch the original calls png_save_uint_32 (0x0051ca60) for the length and crc words, in the other two it
   has it inline: the file follows that.
   100.0% (regs 100, ops 100). Key: back copied from back_arg (frees its slot for the length/crc buffers, 87.7 without).
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
   unsigned char index;
   unsigned short red;
   unsigned short green;
   unsigned short blue;
   unsigned short gray;
} png_color_16;

MGLOBALA(char, s_Invalid_background_palette_index_00621a40, 33, 0x00621a40);
MGLOBALA(unsigned char, png_bKGD, 5, 0x005e323c);
void FUN_00516bb0(int png_ptr, unsigned char *data, unsigned int length);
void FUN_00516d30(int png_ptr);
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length);
void FUN_00517270(int png_ptr, const char *msg);
void FUN_0051ca60(unsigned char *buf, unsigned int i);

// MATCH: 0x0051e4a0 FUN_0051e4a0
void FUN_0051e4a0(int png_ptr, png_color_16 *back_arg, int color_type)
{
   png_color_16 *back = back_arg;
   unsigned char buf[6];

   if (color_type == 3)
   {
      if ((*(unsigned short *)(png_ptr + 0x118) ||
          (!(*(unsigned int *)(png_ptr + 0x1e0) & 1))) &&
         back->index > *(unsigned short *)(png_ptr + 0x118))
      {
         FUN_00517270(png_ptr, s_Invalid_background_palette_index_00621a40);
         return;
      }
      buf[0] = back->index;
      {
         unsigned char buf[4];
         FUN_0051ca60(buf, 1);
         FUN_00516bb0(png_ptr, buf, 4);
         FUN_00516bb0(png_ptr, png_bKGD, 4);
         FUN_00516d30(png_ptr);
         FUN_00516d50(png_ptr, png_bKGD, 4);
      }
      FUN_00516d50(png_ptr, buf, 1);
      FUN_00516bb0(png_ptr, buf, 1);
      {
         unsigned char buf[4];
         FUN_0051ca60(buf, *(unsigned int *)(png_ptr + 0x110));
         FUN_00516bb0(png_ptr, buf, 4);
      }
   }
   else if (color_type & 2)
   {
      { unsigned int i = back->red; buf[0] = (unsigned char)((i >> 8) & 0xff); buf[0 + 1] = (unsigned char)(i & 0xff); }
      { unsigned int i = back->green; buf[2] = (unsigned char)((i >> 8) & 0xff); buf[2 + 1] = (unsigned char)(i & 0xff); }
      { unsigned int i = back->blue; buf[4] = (unsigned char)((i >> 8) & 0xff); buf[4 + 1] = (unsigned char)(i & 0xff); }
      {
         unsigned char buf[4];
         unsigned int length = 6;
         buf[0] = (unsigned char)((length >> 24) & 0xff);
         buf[1] = (unsigned char)((length >> 16) & 0xff);
         buf[2] = (unsigned char)((length >> 8) & 0xff);
         buf[3] = (unsigned char)(length & 0xff);
         FUN_00516bb0(png_ptr, buf, 4);
         FUN_00516bb0(png_ptr, png_bKGD, 4);
         FUN_00516d30(png_ptr);
         FUN_00516d50(png_ptr, png_bKGD, 4);
      }
      FUN_00516d50(png_ptr, buf, 6);
      FUN_00516bb0(png_ptr, buf, 6);
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
   else
   {
      { unsigned int i = back->gray; buf[0] = (unsigned char)((i >> 8) & 0xff); buf[0 + 1] = (unsigned char)(i & 0xff); }
      {
         unsigned char buf[4];
         unsigned int length = 2;
         buf[0] = (unsigned char)((length >> 24) & 0xff);
         buf[1] = (unsigned char)((length >> 16) & 0xff);
         buf[2] = (unsigned char)((length >> 8) & 0xff);
         buf[3] = (unsigned char)(length & 0xff);
         FUN_00516bb0(png_ptr, buf, 4);
         FUN_00516bb0(png_ptr, png_bKGD, 4);
         FUN_00516d30(png_ptr);
         FUN_00516d50(png_ptr, png_bKGD, 4);
      }
      FUN_00516d50(png_ptr, buf, 2);
      FUN_00516bb0(png_ptr, buf, 2);
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
}
