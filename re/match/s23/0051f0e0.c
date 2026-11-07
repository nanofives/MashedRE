// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051f0e0: libpng 1.0.12 pngwutil.c png_write_sCAL_s (libpair: not paired), png_write_chunk_start /
   _data / _end written inline, png_strcpy / png_strlen = intrinsic strcpy / strlen.
   73.9% (regs 76.9, ops 79.1). Residual: the four png_strlen calls. The original's intrinsic strlen is repne scasb
   (string in edi), 9176 emits a byte loop. strcpy loops, the inlined chunk helpers and the stack layout line up.
   Class (a) (same as 0x0051e990, 0x005278a0). */
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
extern "C" {
#endif
unsigned int __cdecl strlen(const char *s);
char *__cdecl strcpy(char *d, const char *s);
#ifdef __cplusplus
}
#endif
#pragma intrinsic(strlen, strcpy)
MGLOBALA(unsigned char, png_sCAL, 5, 0x005e327c);
void FUN_00516bb0(int png_ptr, unsigned char *data, unsigned int length);
void FUN_00516d30(int png_ptr);
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length);

// MATCH: 0x0051f0e0 FUN_0051f0e0
void FUN_0051f0e0(int png_ptr, int unit, char *width, char *height)
{
   unsigned int total_len;
   char wbuf[32], hbuf[32];

   strcpy(wbuf, (const char *)width);
   strcpy(hbuf, (const char *)height);
   total_len = 1 + strlen(wbuf) + 1 + strlen(hbuf);

   {
      unsigned char buf[4];
      unsigned int length = (unsigned int)total_len;
      buf[0] = (unsigned char)((length >> 24) & 0xff);
      buf[1] = (unsigned char)((length >> 16) & 0xff);
      buf[2] = (unsigned char)((length >> 8) & 0xff);
      buf[3] = (unsigned char)(length & 0xff);
      FUN_00516bb0(png_ptr, buf, 4);
      FUN_00516bb0(png_ptr, png_sCAL, 4);
      FUN_00516d30(png_ptr);
      FUN_00516d50(png_ptr, png_sCAL, 4);
   }
   FUN_00516d50(png_ptr, (unsigned char *)&unit, 1);
   FUN_00516bb0(png_ptr, (unsigned char *)&unit, 1);
   {
      unsigned int length = strlen(wbuf) + 1;
      if (length > 0)
      {
         FUN_00516d50(png_ptr, (unsigned char *)wbuf, length);
         FUN_00516bb0(png_ptr, (unsigned char *)wbuf, length);
      }
   }
   {
      unsigned int length = strlen(hbuf);
      if (length > 0)
      {
         FUN_00516d50(png_ptr, (unsigned char *)hbuf, length);
         FUN_00516bb0(png_ptr, (unsigned char *)hbuf, length);
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
}
