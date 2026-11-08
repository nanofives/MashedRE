// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051d400: libpng 1.0.12 pngwutil.c png_write_iCCP, png_write_chunk_start / _data / _end written inline.
   100.0% (regs 100, ops 100). Key: png_ptr copied into a local (png_ptr_arg -> png_ptr) frees the png_ptr arg slot,
   so the compiler packs new_name into it as the original does (82.2% without). Residual: none. */
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
   char *input;
   int input_len;
   int num_output_ptr;
   int max_output_ptr;
   unsigned char **output_ptr;
} compression_state;

MGLOBALA(char, s_Unknown_compression_type_in_iCCP_006218d8, 39, 0x006218d8);
MGLOBALA(char, s_Empty_keyword_in_iCCP_chunk_006218bc, 28, 0x006218bc);
MGLOBALA(unsigned char, png_iCCP, 5, 0x005e325c);
void FUN_00516bb0(int png_ptr, unsigned char *data, unsigned int length);
void FUN_00516d30(int png_ptr);
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length);
void FUN_00517270(int png_ptr, const char *msg);
unsigned int FUN_0051e7f0(int png_ptr, char *key, char **new_key);
int FUN_0051d560(int png_ptr, char *text, unsigned int text_len, int compression, compression_state *comp);
void FUN_0051d800(int png_ptr, compression_state *comp);
void FUN_00520930(int png_ptr, void *ptr);

// MATCH: 0x0051d400 FUN_0051d400
void FUN_0051d400(int png_ptr_arg, char *name, int compression_type, char *profile, int profile_len)
{
   unsigned int name_len;
   char *new_name;
   compression_state comp;
   int png_ptr = png_ptr_arg;

   if (name == 0 || (name_len = FUN_0051e7f0(png_ptr, name, &new_name)) == 0)
   {
      FUN_00517270(png_ptr, s_Empty_keyword_in_iCCP_chunk_006218bc);
      return;
   }

   if (compression_type != 0)
      FUN_00517270(png_ptr, s_Unknown_compression_type_in_iCCP_006218d8);

   if (profile == 0)
      profile_len = 0;

   if (profile_len)
      profile_len = FUN_0051d560(png_ptr, profile, (unsigned int)profile_len, 0, &comp);

   {
      unsigned char buf[4];
      unsigned int length = (unsigned int)name_len + profile_len + 2;
      buf[0] = (unsigned char)((length >> 24) & 0xff);
      buf[1] = (unsigned char)((length >> 16) & 0xff);
      buf[2] = (unsigned char)((length >> 8) & 0xff);
      buf[3] = (unsigned char)(length & 0xff);
      FUN_00516bb0(png_ptr, buf, 4);
      FUN_00516bb0(png_ptr, png_iCCP, 4);
      FUN_00516d30(png_ptr);
      FUN_00516d50(png_ptr, png_iCCP, 4);
   }
   new_name[name_len + 1] = 0x00;
   {
      unsigned char *data = (unsigned char *)new_name;
      unsigned int length = name_len + 2;
      if (data != 0 && length > 0)
      {
         FUN_00516d50(png_ptr, data, length);
         FUN_00516bb0(png_ptr, data, length);
      }
   }

   if (profile_len)
      FUN_0051d800(png_ptr, &comp);

   {
      unsigned char buf[4];
      unsigned int crc = *(unsigned int *)(png_ptr + 0x110);
      buf[0] = (unsigned char)((crc >> 24) & 0xff);
      buf[1] = (unsigned char)((crc >> 16) & 0xff);
      buf[2] = (unsigned char)((crc >> 8) & 0xff);
      buf[3] = (unsigned char)(crc & 0xff);
      FUN_00516bb0(png_ptr, buf, 4);
   }
   FUN_00520930(png_ptr, new_name);
}
