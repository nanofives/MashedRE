// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051dca0: libpng 1.0.12 pngwutil.c png_write_cHRM (floating point, PNG_NO_CONSOLE_IO; libpair: not
   paired), png_save_uint_32 and png_write_chunk with its start / data / end written inline. Each value goes through a
   float temp as in the original (fstp dword / fld dword) before the float-to-int conversion.
   40.0% (regs 40.0, ops 76.5). Structure, constants and the inlined chunk tail are the official source. Residual: the 8
   float-to-int conversions: the original has the inline fnstcw / or ah,0xc / fldcw / fistp dword / fldcw sequence
   after a float temp, 9176 calls __ftol and drops the float store (as 0x0051d240 png_write_gAMA). Class (a). */
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
MGLOBALA(unsigned char, png_cHRM, 5, 0x005e3244);
MGLOBALA(char, s_Invalid_cHRM_white_point_speci_00621958, 35, 0x00621958);
MGLOBALA(char, s_Invalid_cHRM_red_point_specifi_0062197c, 33, 0x0062197c);
MGLOBALA(char, s_Invalid_cHRM_green_point_speci_006219a0, 35, 0x006219a0);
MGLOBALA(char, s_Invalid_cHRM_blue_point_specif_006219c4, 34, 0x006219c4);
MGLOBAL(double, DAT_005d1008, 0x005d1008);
MGLOBAL(double, DAT_005ce988, 0x005ce988);
MGLOBAL(double, DAT_005cd0c8, 0x005cd0c8);
MGLOBAL(double, DAT_005e31f0, 0x005e31f0);
MGLOBAL(double, DAT_005ce1d8, 0x005ce1d8);
void FUN_00516bb0(int png_ptr, unsigned char *data, unsigned int length);
void FUN_00516d30(int png_ptr);
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length);
void FUN_00517270(int png_ptr, const char *msg);

// MATCH: 0x0051dca0 FUN_0051dca0
void FUN_0051dca0(int png_ptr, double white_x, double white_y,
   double red_x, double red_y, double green_x, double green_y,
   double blue_x, double blue_y)
{
   unsigned char buf[32];
   unsigned int itemp;
   float f;

   if (white_x < DAT_005d1008 || white_x > DAT_005ce988 || white_y < DAT_005d1008 || white_y > DAT_005ce988 ||
       white_x + white_y > DAT_005cd0c8)
   {
      FUN_00517270(png_ptr, s_Invalid_cHRM_white_point_speci_00621958);
      return;
   }
   f = (float)(white_x * DAT_005e31f0 + DAT_005ce1d8);
   itemp = (unsigned int)(int)f;
   buf[0] = (unsigned char)((itemp >> 24) & 0xff);
   buf[0+1] = (unsigned char)((itemp >> 16) & 0xff);
   buf[0+2] = (unsigned char)((itemp >> 8) & 0xff);
   buf[0+3] = (unsigned char)(itemp & 0xff);
   f = (float)(white_y * DAT_005e31f0 + DAT_005ce1d8);
   itemp = (unsigned int)(int)f;
   buf[0+4] = (unsigned char)((itemp >> 24) & 0xff);
   buf[0+5] = (unsigned char)((itemp >> 16) & 0xff);
   buf[0+6] = (unsigned char)((itemp >> 8) & 0xff);
   buf[0+7] = (unsigned char)(itemp & 0xff);

   if (red_x < DAT_005d1008 || red_x > DAT_005ce988 || red_y < DAT_005d1008 || red_y > DAT_005ce988 ||
       red_x + red_y > DAT_005cd0c8)
   {
      FUN_00517270(png_ptr, s_Invalid_cHRM_red_point_specifi_0062197c);
      return;
   }
   f = (float)(red_x * DAT_005e31f0 + DAT_005ce1d8);
   itemp = (unsigned int)(int)f;
   buf[8] = (unsigned char)((itemp >> 24) & 0xff);
   buf[8+1] = (unsigned char)((itemp >> 16) & 0xff);
   buf[8+2] = (unsigned char)((itemp >> 8) & 0xff);
   buf[8+3] = (unsigned char)(itemp & 0xff);
   f = (float)(red_y * DAT_005e31f0 + DAT_005ce1d8);
   itemp = (unsigned int)(int)f;
   buf[8+4] = (unsigned char)((itemp >> 24) & 0xff);
   buf[8+5] = (unsigned char)((itemp >> 16) & 0xff);
   buf[8+6] = (unsigned char)((itemp >> 8) & 0xff);
   buf[8+7] = (unsigned char)(itemp & 0xff);

   if (green_x < DAT_005d1008 || green_x > DAT_005ce988 || green_y < DAT_005d1008 || green_y > DAT_005ce988 ||
       green_x + green_y > DAT_005cd0c8)
   {
      FUN_00517270(png_ptr, s_Invalid_cHRM_green_point_speci_006219a0);
      return;
   }
   f = (float)(green_x * DAT_005e31f0 + DAT_005ce1d8);
   itemp = (unsigned int)(int)f;
   buf[16] = (unsigned char)((itemp >> 24) & 0xff);
   buf[16+1] = (unsigned char)((itemp >> 16) & 0xff);
   buf[16+2] = (unsigned char)((itemp >> 8) & 0xff);
   buf[16+3] = (unsigned char)(itemp & 0xff);
   f = (float)(green_y * DAT_005e31f0 + DAT_005ce1d8);
   itemp = (unsigned int)(int)f;
   buf[16+4] = (unsigned char)((itemp >> 24) & 0xff);
   buf[16+5] = (unsigned char)((itemp >> 16) & 0xff);
   buf[16+6] = (unsigned char)((itemp >> 8) & 0xff);
   buf[16+7] = (unsigned char)(itemp & 0xff);

   if (blue_x < DAT_005d1008 || blue_x > DAT_005ce988 || blue_y < DAT_005d1008 || blue_y > DAT_005ce988 ||
       blue_x + blue_y > DAT_005cd0c8)
   {
      FUN_00517270(png_ptr, s_Invalid_cHRM_blue_point_specif_006219c4);
      return;
   }
   f = (float)(blue_x * DAT_005e31f0 + DAT_005ce1d8);
   itemp = (unsigned int)(int)f;
   buf[24] = (unsigned char)((itemp >> 24) & 0xff);
   buf[24+1] = (unsigned char)((itemp >> 16) & 0xff);
   buf[24+2] = (unsigned char)((itemp >> 8) & 0xff);
   buf[24+3] = (unsigned char)(itemp & 0xff);
   f = (float)(blue_y * DAT_005e31f0 + DAT_005ce1d8);
   itemp = (unsigned int)(int)f;
   buf[24+4] = (unsigned char)((itemp >> 24) & 0xff);
   buf[24+5] = (unsigned char)((itemp >> 16) & 0xff);
   buf[24+6] = (unsigned char)((itemp >> 8) & 0xff);
   buf[24+7] = (unsigned char)(itemp & 0xff);

   {
      unsigned char buf[4];
      unsigned int length = 32;
      buf[0] = (unsigned char)((length >> 24) & 0xff);
      buf[1] = (unsigned char)((length >> 16) & 0xff);
      buf[2] = (unsigned char)((length >> 8) & 0xff);
      buf[3] = (unsigned char)(length & 0xff);
      FUN_00516bb0(png_ptr, buf, 4);
      FUN_00516bb0(png_ptr, png_cHRM, 4);
      FUN_00516d30(png_ptr);
      FUN_00516d50(png_ptr, png_cHRM, 4);
   }
   FUN_00516d50(png_ptr, buf, 32);
   FUN_00516bb0(png_ptr, buf, 32);
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
