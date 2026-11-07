// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051e990: libpng 1.0.12 pngwutil.c png_write_tEXt, png_write_chunk_start / _data / _end written
   inline, png_strlen = intrinsic strlen.
   68.3% (regs 74.0, ops 90.2). Residual: png_strlen. The original's intrinsic strlen is repne scasb (text in edi), 9176
   emits a byte loop, which also moves the edi/ebp allocation of text_len/text. Everything else lines up (png_ptr copied
   from png_ptr_arg so new_key packs into its slot). Tried /Os, /O1, /GS. Class (a) (same as S23 0x005278a0, 0x004b71c0). */
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
extern "C" unsigned int __cdecl strlen(const char *s);
#else
unsigned int __cdecl strlen(const char *s);
#endif
#pragma intrinsic(strlen)
MGLOBALA(char, s_Empty_keyword_in_tEXt_chunk_00621b80, 28, 0x00621b80);
MGLOBALA(unsigned char, png_tEXt, 5, 0x005e32a4);
void FUN_00516bb0(int png_ptr, unsigned char *data, unsigned int length);
void FUN_00516d30(int png_ptr);
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length);
void FUN_00517270(int png_ptr, const char *msg);
unsigned int FUN_0051e7f0(int png_ptr, char *key, char **new_key);
void FUN_00520930(int png_ptr, void *ptr);

// MATCH: 0x0051e990 FUN_0051e990
void FUN_0051e990(int png_ptr_arg, char *key, char *text, unsigned int text_len)
{
   unsigned int key_len;
   char *new_key;
   int png_ptr = png_ptr_arg;

   if (key == 0 || (key_len = FUN_0051e7f0(png_ptr, key, &new_key)) == 0)
   {
      FUN_00517270(png_ptr, s_Empty_keyword_in_tEXt_chunk_00621b80);
      return;
   }

   if (text == 0 || *text == '\0')
      text_len = 0;
   else
      text_len = strlen(text);

   {
      unsigned char buf[4];
      unsigned int length = (unsigned int)key_len + text_len + 1;
      buf[0] = (unsigned char)((length >> 24) & 0xff);
      buf[1] = (unsigned char)((length >> 16) & 0xff);
      buf[2] = (unsigned char)((length >> 8) & 0xff);
      buf[3] = (unsigned char)(length & 0xff);
      FUN_00516bb0(png_ptr, buf, 4);
      FUN_00516bb0(png_ptr, png_tEXt, 4);
      FUN_00516d30(png_ptr);
      FUN_00516d50(png_ptr, png_tEXt, 4);
   }
   {
      unsigned char *data = (unsigned char *)new_key;
      unsigned int length = key_len + 1;
      if (data != 0 && length > 0)
      {
         FUN_00516d50(png_ptr, data, length);
         FUN_00516bb0(png_ptr, data, length);
      }
   }
   if (text_len)
   {
      unsigned char *data = (unsigned char *)text;
      unsigned int length = text_len;
      if (data != 0 && length > 0)
      {
         FUN_00516d50(png_ptr, data, length);
         FUN_00516bb0(png_ptr, data, length);
      }
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
   FUN_00520930(png_ptr, new_key);
}
