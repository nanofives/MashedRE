// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051d240: libpng 1.0.12 pngwutil.c png_write_gAMA (floating point), png_write_chunk and its
   start / data / end written inline.
   58.4% (regs 58.4, ops 83.1). Residual: the original converts with an inline fnstcw / or ah,0xc / fldcw / fistp dword /
   fldcw sequence through a float temp (fstp dword, fld dword); cl 13.00.9176 calls __ftol and drops the float store.
   Tried (int)(float) casts, /Op, /QIfist, /Oi, /G6: no flag emits the inline sequence. The x87-free tail (inlined
   png_write_chunk) lines up but its stack homes shift with the different frame. Class (a): float-to-int codegen. */
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
MGLOBALA(unsigned char, png_gAMA, 5, 0x005e324c);
MGLOBAL(double, DAT_005e31f0, 0x005e31f0);
MGLOBAL(double, DAT_005ce1d8, 0x005ce1d8);
void FUN_00516bb0(int png_ptr, unsigned char *data, unsigned int length);
void FUN_00516d30(int png_ptr);
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length);

// MATCH: 0x0051d240 FUN_0051d240
void FUN_0051d240(int png_ptr, double file_gamma)
{
   unsigned int igamma;
   unsigned char buf[4];
   float f;

   f = (float)(file_gamma * DAT_005e31f0 + DAT_005ce1d8);
   igamma = (unsigned int)(int)f;
   buf[0] = (unsigned char)((igamma >> 24) & 0xff);
   buf[1] = (unsigned char)((igamma >> 16) & 0xff);
   buf[2] = (unsigned char)((igamma >> 8) & 0xff);
   buf[3] = (unsigned char)(igamma & 0xff);
   {
      unsigned char buf[4];
      unsigned int length = 4;
      buf[0] = (unsigned char)((length >> 24) & 0xff);
      buf[1] = (unsigned char)((length >> 16) & 0xff);
      buf[2] = (unsigned char)((length >> 8) & 0xff);
      buf[3] = (unsigned char)(length & 0xff);
      FUN_00516bb0(png_ptr, buf, 4);
      FUN_00516bb0(png_ptr, png_gAMA, 4);
      FUN_00516d30(png_ptr);
      FUN_00516d50(png_ptr, png_gAMA, 4);
   }
   FUN_00516d50(png_ptr, buf, 4);
   FUN_00516bb0(png_ptr, buf, 4);
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
