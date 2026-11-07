// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x005172f0: libpng 1.0.12 pngerror.c png_format_buffer (static; png_digit at 0x005e336c,
   chunk_name at +0x11c). Official source. */
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
MGLOBALA(char, png_digit, 16, 0x005e336c);
#ifdef __cplusplus
extern "C"
#endif
void *__cdecl memcpy(void *d, const void *s, unsigned int n);
#pragma intrinsic(memcpy)
#define isnonalpha(c) ((c) < 41 || (c) > 122 || ((c) > 90 && (c) < 97))
// MATCH: 0x005172f0 FUN_005172f0
void FUN_005172f0(int png_ptr, char *buffer, const char *message)
{
   int iout = 0, iin = 0;

   while (iin < 4)
   {
      int c = ((unsigned char *)(png_ptr + 0x11c))[iin++];
      if (isnonalpha(c))
      {
         buffer[iout++] = '[';
         buffer[iout++] = png_digit[(c & 0xf0) >> 4];
         buffer[iout++] = png_digit[c & 0x0f];
         buffer[iout++] = ']';
      }
      else
      {
         buffer[iout++] = (unsigned char)c;
      }
   }

   if (message == 0)
      buffer[iout] = 0;
   else
   {
      buffer[iout++] = ':';
      buffer[iout++] = ' ';
      memcpy(buffer+iout, message, 64);
      buffer[iout+63] = 0;
   }
}
