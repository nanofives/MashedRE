// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051d900: libpng 1.0.12 pngwutil.c png_write_sPLT, png_write_chunk_start / _data / _end and
   png_save_uint_16 written inline.
   100.0% (regs 100, ops 100). Keys: png_ptr and spalette copied from *_arg params (frees both arg slots for new_name
   and the length/crc buffers), length before data in the inlined new_name chunk_data. Residual: none. */
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
   unsigned short red;
   unsigned short green;
   unsigned short blue;
   unsigned short alpha;
   unsigned short frequency;
} png_sPLT_entry;

typedef struct
{
   char *name;
   unsigned char depth;
   png_sPLT_entry *entries;
   int nentries;
} png_sPLT_t;

MGLOBALA(char, s_Empty_keyword_in_sPLT_chunk_0062191c, 28, 0x0062191c);
MGLOBALA(unsigned char, png_sPLT, 5, 0x005e3294);
void FUN_00516bb0(int png_ptr, unsigned char *data, unsigned int length);
void FUN_00516d30(int png_ptr);
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length);
void FUN_00517270(int png_ptr, const char *msg);
unsigned int FUN_0051e7f0(int png_ptr, char *key, char **new_key);
void FUN_00520930(int png_ptr, void *ptr);

// MATCH: 0x0051d900 FUN_0051d900
void FUN_0051d900(int png_ptr_arg, png_sPLT_t *spalette_arg)
{
   int png_ptr = png_ptr_arg;
   png_sPLT_t *spalette = spalette_arg;
   unsigned int name_len;
   char *new_name;
   unsigned char entrybuf[10];
   int entry_size = (spalette->depth == 8 ? 6 : 10);
   int palette_size = entry_size * spalette->nentries;
   png_sPLT_entry *ep;

   if (spalette->name == 0 || (name_len = FUN_0051e7f0(png_ptr,
      spalette->name, &new_name)) == 0)
   {
      FUN_00517270(png_ptr, s_Empty_keyword_in_sPLT_chunk_0062191c);
      return;
   }

   {
      unsigned char buf[4];
      unsigned int length = (unsigned int)(name_len + 2 + palette_size);
      buf[0] = (unsigned char)((length >> 24) & 0xff);
      buf[1] = (unsigned char)((length >> 16) & 0xff);
      buf[2] = (unsigned char)((length >> 8) & 0xff);
      buf[3] = (unsigned char)(length & 0xff);
      FUN_00516bb0(png_ptr, buf, 4);
      FUN_00516bb0(png_ptr, png_sPLT, 4);
      FUN_00516d30(png_ptr);
      FUN_00516d50(png_ptr, png_sPLT, 4);
   }
   {
      unsigned int length = name_len + 1;
      unsigned char *data = (unsigned char *)new_name;
      if (data != 0 && length > 0)
      {
         FUN_00516d50(png_ptr, data, length);
         FUN_00516bb0(png_ptr, data, length);
      }
   }
   {
      unsigned char *data = (unsigned char *)&spalette->depth;
      if (data != 0)
      {
         FUN_00516d50(png_ptr, data, 1);
         FUN_00516bb0(png_ptr, data, 1);
      }
   }

   for (ep = spalette->entries; ep < spalette->entries + spalette->nentries; ep++)
   {
       if (spalette->depth == 8)
       {
           entrybuf[0] = (unsigned char)ep->red;
           entrybuf[1] = (unsigned char)ep->green;
           entrybuf[2] = (unsigned char)ep->blue;
           entrybuf[3] = (unsigned char)ep->alpha;
           { unsigned int i = ep->frequency; entrybuf[4] = (unsigned char)((i >> 8) & 0xff); entrybuf[4 + 1] = (unsigned char)(i & 0xff); }
       }
       else
       {
           { unsigned int i = ep->red; entrybuf[0] = (unsigned char)((i >> 8) & 0xff); entrybuf[0 + 1] = (unsigned char)(i & 0xff); }
           { unsigned int i = ep->green; entrybuf[2] = (unsigned char)((i >> 8) & 0xff); entrybuf[2 + 1] = (unsigned char)(i & 0xff); }
           { unsigned int i = ep->blue; entrybuf[4] = (unsigned char)((i >> 8) & 0xff); entrybuf[4 + 1] = (unsigned char)(i & 0xff); }
           { unsigned int i = ep->alpha; entrybuf[6] = (unsigned char)((i >> 8) & 0xff); entrybuf[6 + 1] = (unsigned char)(i & 0xff); }
           { unsigned int i = ep->frequency; entrybuf[8] = (unsigned char)((i >> 8) & 0xff); entrybuf[8 + 1] = (unsigned char)(i & 0xff); }
       }
       {
          unsigned int length = (unsigned int)entry_size;
          if (length > 0)
          {
             FUN_00516d50(png_ptr, entrybuf, length);
             FUN_00516bb0(png_ptr, entrybuf, length);
          }
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
   FUN_00520930(png_ptr, new_name);
}
