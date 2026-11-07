// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051e230: libpng 1.0.12 pngwutil.c png_write_tRNS (libpair: not paired).
   png_write_chunk and its start / data / end written inline in all three branches, png_save_uint_16 inline. In the
   palette and gray branches the original calls png_save_uint_32 (0x0051ca60) for the length and crc words, in the
   RGB branch it has it inline: the file follows that.
   100.0% (regs 100, ops 100). Key: png_ptr and tran copied from *_arg params (frees both slots, 94.7/96.7 with one).
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

MGLOBALA(char, s_Invalid_number_of_transparent_col_00621a10, 47, 0x00621a10);
MGLOBALA(char, s_Can_t_write_tRNS_with_an_alpha_c_006219e8, 39, 0x006219e8);
MGLOBALA(unsigned char, png_tRNS, 5, 0x005e32b4);
void FUN_00516bb0(int png_ptr, unsigned char *data, unsigned int length);
void FUN_00516d30(int png_ptr);
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length);
void FUN_00517270(int png_ptr, const char *msg);
void FUN_0051ca60(unsigned char *buf, unsigned int i);

// MATCH: 0x0051e230 FUN_0051e230
void FUN_0051e230(int png_ptr_arg, unsigned char *trans, png_color_16 *tran_arg, int num_trans, int color_type)
{
   int png_ptr = png_ptr_arg;
   png_color_16 *tran = tran_arg;
   unsigned char buf[6];

   if (color_type == 3)
   {
      if (num_trans <= 0 || num_trans > (int)*(unsigned short *)(png_ptr + 0x118))
      {
         FUN_00517270(png_ptr, s_Invalid_number_of_transparent_col_00621a10);
         return;
      }
      {
         unsigned char buf[4];
         FUN_0051ca60(buf, (unsigned int)num_trans);
         FUN_00516bb0(png_ptr, buf, 4);
         FUN_00516bb0(png_ptr, png_tRNS, 4);
         FUN_00516d30(png_ptr);
         FUN_00516d50(png_ptr, png_tRNS, 4);
      }
      {
         unsigned char *data = trans;
         unsigned int length = (unsigned int)num_trans;
         if (data != 0 && length > 0)
         {
            FUN_00516d50(png_ptr, data, length);
            FUN_00516bb0(png_ptr, data, length);
         }
      }
      {
         unsigned char buf[4];
         FUN_0051ca60(buf, *(unsigned int *)(png_ptr + 0x110));
         FUN_00516bb0(png_ptr, buf, 4);
      }
   }
   else if (color_type == 0)
   {
      { unsigned int i = tran->gray; buf[0] = (unsigned char)((i >> 8) & 0xff); buf[0 + 1] = (unsigned char)(i & 0xff); }
      {
         unsigned char buf[4];
         FUN_0051ca60(buf, 2);
         FUN_00516bb0(png_ptr, buf, 4);
         FUN_00516bb0(png_ptr, png_tRNS, 4);
         FUN_00516d30(png_ptr);
         FUN_00516d50(png_ptr, png_tRNS, 4);
      }
      FUN_00516d50(png_ptr, buf, 2);
      FUN_00516bb0(png_ptr, buf, 2);
      {
         unsigned char buf[4];
         FUN_0051ca60(buf, *(unsigned int *)(png_ptr + 0x110));
         FUN_00516bb0(png_ptr, buf, 4);
      }
   }
   else if (color_type == 2)
   {
      { unsigned int i = tran->red; buf[0] = (unsigned char)((i >> 8) & 0xff); buf[0 + 1] = (unsigned char)(i & 0xff); }
      { unsigned int i = tran->green; buf[2] = (unsigned char)((i >> 8) & 0xff); buf[2 + 1] = (unsigned char)(i & 0xff); }
      { unsigned int i = tran->blue; buf[4] = (unsigned char)((i >> 8) & 0xff); buf[4 + 1] = (unsigned char)(i & 0xff); }
      {
         unsigned char buf[4];
         unsigned int length = 6;
         buf[0] = (unsigned char)((length >> 24) & 0xff);
         buf[1] = (unsigned char)((length >> 16) & 0xff);
         buf[2] = (unsigned char)((length >> 8) & 0xff);
         buf[3] = (unsigned char)(length & 0xff);
         FUN_00516bb0(png_ptr, buf, 4);
         FUN_00516bb0(png_ptr, png_tRNS, 4);
         FUN_00516d30(png_ptr);
         FUN_00516d50(png_ptr, png_tRNS, 4);
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
      FUN_00517270(png_ptr, s_Can_t_write_tRNS_with_an_alpha_c_006219e8);
   }
}
