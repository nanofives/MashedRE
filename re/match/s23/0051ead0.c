// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051ead0: libpng 1.0.12 pngwutil.c png_write_zTXt (libpair: not paired). The original has
   png_write_tEXt (the uncompressed path) inline, and inside it png_save_uint_32 as calls (0x0051ca60) and the text
   png_write_chunk_data as a call (0x0051cb40); the compressed path has png_write_chunk_start / _data / _end and
   png_save_uint_32 inline. png_strlen = intrinsic strlen. The file follows that.
   63.8% (regs ~72, ops ~93). png_ptr and text copied from *_arg params (best of the 1- and 2-param copy sweep).
   Residual: the two png_strlen sites (original repne scasb with the string in edi, 9176 a byte loop), which move the
   ebp/edi allocation of key_len/text_len, and the frame (ours 0x20, original 0x18: buf[1] and the inlined tEXt
   new_key sit in arg slots in the original). Class (a) strlen codegen, plus (b) stack homes. */
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
typedef struct
{
   char *input;
   int input_len;
   int num_output_ptr;
   int max_output_ptr;
   unsigned char **output_ptr;
} compression_state;

MGLOBALA(char, s_Empty_keyword_in_zTXt_chunk_00621b9c, 28, 0x00621b9c);
MGLOBALA(char, s_Empty_keyword_in_tEXt_chunk_00621b80, 28, 0x00621b80);
MGLOBALA(unsigned char, png_zTXt, 5, 0x005e32bc);
MGLOBALA(unsigned char, png_tEXt, 5, 0x005e32a4);
void FUN_00516bb0(int png_ptr, unsigned char *data, unsigned int length);
void FUN_00516d30(int png_ptr);
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length);
void FUN_00517270(int png_ptr, const char *msg);
void FUN_0051ca60(unsigned char *buf, unsigned int i);
void FUN_0051cb40(int png_ptr, unsigned char *data, unsigned int length);
int FUN_0051d560(int png_ptr, char *text, unsigned int text_len, int compression, compression_state *comp);
void FUN_0051d800(int png_ptr, compression_state *comp);
unsigned int FUN_0051e7f0(int png_ptr, char *key, char **new_key);
void FUN_00520930(int png_ptr, void *ptr);

// MATCH: 0x0051ead0 FUN_0051ead0
void FUN_0051ead0(int png_ptr_arg, char *key, char *text_arg, unsigned int text_len, int compression)
{
   int png_ptr = png_ptr_arg;
   char *text = text_arg;
   unsigned int key_len;
   char buf[1];
   char *new_key;
   compression_state comp;

   if (key == 0 || (key_len = FUN_0051e7f0(png_ptr, key, &new_key)) == 0)
   {
      FUN_00517270(png_ptr, s_Empty_keyword_in_zTXt_chunk_00621b9c);
      return;
   }

   if (text == 0 || *text == '\0' || compression == -1)
   {
      /* png_write_tEXt(png_ptr, new_key, text, 0) */
      char *t_key = new_key;
      char *t_text = text;
      unsigned int t_text_len;
      unsigned int t_key_len;
      char *t_new_key;

      if (t_key == 0 || (t_key_len = FUN_0051e7f0(png_ptr, t_key, &t_new_key)) == 0)
      {
         FUN_00517270(png_ptr, s_Empty_keyword_in_tEXt_chunk_00621b80);
      }
      else
      {
         if (t_text == 0 || *t_text == '\0')
            t_text_len = 0;
         else
            t_text_len = strlen(t_text);

         {
            unsigned char buf[4];
            FUN_0051ca60(buf, (unsigned int)t_key_len + t_text_len + 1);
            FUN_00516bb0(png_ptr, buf, 4);
            FUN_00516bb0(png_ptr, png_tEXt, 4);
            FUN_00516d30(png_ptr);
            FUN_00516d50(png_ptr, png_tEXt, 4);
         }
         {
            unsigned char *data = (unsigned char *)t_new_key;
            unsigned int length = t_key_len + 1;
            if (data != 0 && length > 0)
            {
               FUN_00516d50(png_ptr, data, length);
               FUN_00516bb0(png_ptr, data, length);
            }
         }
         if (t_text_len)
            FUN_0051cb40(png_ptr, (unsigned char *)t_text, t_text_len);
         {
            unsigned char buf[4];
            FUN_0051ca60(buf, *(unsigned int *)(png_ptr + 0x110));
            FUN_00516bb0(png_ptr, buf, 4);
         }
         FUN_00520930(png_ptr, t_new_key);
      }
      FUN_00520930(png_ptr, new_key);
      return;
   }

   text_len = strlen(text);

   FUN_00520930(png_ptr, new_key);

   text_len = FUN_0051d560(png_ptr, text, text_len, compression, &comp);

   {
      unsigned char buf[4];
      unsigned int length = (unsigned int)(key_len + text_len + 2);
      buf[0] = (unsigned char)((length >> 24) & 0xff);
      buf[1] = (unsigned char)((length >> 16) & 0xff);
      buf[2] = (unsigned char)((length >> 8) & 0xff);
      buf[3] = (unsigned char)(length & 0xff);
      FUN_00516bb0(png_ptr, buf, 4);
      FUN_00516bb0(png_ptr, png_zTXt, 4);
      FUN_00516d30(png_ptr);
      FUN_00516d50(png_ptr, png_zTXt, 4);
   }
   {
      unsigned char *data = (unsigned char *)key;
      unsigned int length = key_len + 1;
      if (data != 0 && length > 0)
      {
         FUN_00516d50(png_ptr, data, length);
         FUN_00516bb0(png_ptr, data, length);
      }
   }
   buf[0] = (unsigned char)compression;
   FUN_00516d50(png_ptr, (unsigned char *)buf, 1);
   FUN_00516bb0(png_ptr, (unsigned char *)buf, 1);
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
}
