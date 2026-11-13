// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051cfc0: libpng 1.0.12 pngwutil.c png_write_PLTE (PNG_MNG_FEATURES_SUPPORTED), with
   png_write_chunk_start / _data / _end and png_save_uint_32 written inline (the original has them inline).
   76.7% (regs 95.1, ops 97.1). Key: num_pal copied from num_pal_arg (frees that arg slot for the length/crc
   buffers, as in the original), block-scoped buffer per inlined helper, function-scope buf[3] (61.3 -> 76.7).
   Residual: palette load in the loop preheader (orig mov eax,[palette]; lea edi,[eax+2], ours mov edi; add edi,2)
   and the byte-register choice in the loop and in the crc tail. Tried png_color struct form, png_ptr/palette copies,
   index loop, permuter 300 s (88.3% only by moving a buf store past its call: semantics change, rejected). Class (b). */
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
MGLOBALA(char, s_Invalid_number_of_colors_in_pale_00621870, 36, 0x00621870);
MGLOBALA(char, s_Ignoring_request_to_write_a_PLTE_00621838, 56, 0x00621838);
MGLOBALA(unsigned char, png_PLTE, 5, 0x005e3234);
void FUN_00516bb0(int png_ptr, unsigned char *data, unsigned int length);
void FUN_00516d30(int png_ptr);
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length);
void FUN_00517250(int png_ptr, const char *msg);
void FUN_00517270(int png_ptr, const char *msg);

// MATCH: 0x0051cfc0 FUN_0051cfc0
void FUN_0051cfc0(int png_ptr, unsigned char *palette, unsigned int num_pal_arg)
{
   unsigned int i;
   unsigned char *pal_ptr;
   unsigned int num_pal = num_pal_arg;
   unsigned char buf[3];

   if ((!(*(unsigned int *)(png_ptr + 0x1e0) & 1) && num_pal == 0) || num_pal > 256)
   {
     if (*(unsigned char *)(png_ptr + 0x126) == 3)
     {
        FUN_00517250(png_ptr, s_Invalid_number_of_colors_in_pale_00621870);
     }
     else
     {
        FUN_00517270(png_ptr, s_Invalid_number_of_colors_in_pale_00621870);
        return;
     }
   }

   if (!(*(unsigned char *)(png_ptr + 0x126) & 2))
   {
      FUN_00517270(png_ptr, s_Ignoring_request_to_write_a_PLTE_00621838);
      return;
   }

   *(unsigned short *)(png_ptr + 0x118) = (unsigned short)num_pal;

   {
   unsigned char lbuf[4];
   unsigned int length = num_pal * 3;
   lbuf[0] = (unsigned char)((length >> 24) & 0xff);
   lbuf[1] = (unsigned char)((length >> 16) & 0xff);
   lbuf[2] = (unsigned char)((length >> 8) & 0xff);
   lbuf[3] = (unsigned char)(length & 0xff);
   FUN_00516bb0(png_ptr, lbuf, 4);
   FUN_00516bb0(png_ptr, png_PLTE, 4);
   FUN_00516d30(png_ptr);
   FUN_00516d50(png_ptr, png_PLTE, 4);
   }

   for (i = 0, pal_ptr = palette; i < num_pal; i++, pal_ptr += 3)
   {
      buf[0] = pal_ptr[0];
      buf[1] = pal_ptr[1];
      buf[2] = pal_ptr[2];
      FUN_00516d50(png_ptr, buf, 3);
      FUN_00516bb0(png_ptr, buf, 3);
   }

   {
   unsigned char lbuf[4];
   unsigned int crc = *(unsigned int *)(png_ptr + 0x110);
   lbuf[0] = (unsigned char)((crc >> 24) & 0xff);
   lbuf[1] = (unsigned char)((crc >> 16) & 0xff);
   lbuf[2] = (unsigned char)((crc >> 8) & 0xff);
   lbuf[3] = (unsigned char)(crc & 0xff);
   FUN_00516bb0(png_ptr, lbuf, 4);
   }
   *(unsigned int *)(png_ptr + 0x68) |= 2;
}
