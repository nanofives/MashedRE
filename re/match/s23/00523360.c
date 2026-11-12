// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00523360: libpng 1.0.12 pngrutil.c png_decompress_chunk, PNG_NO_STDIO form (no sprintf;
   the two messages are the #else strings), inflateReset called, png_memcpy = memcpy intrinsic.
   76.5% (regs 77.4, ops 98.7). Residual: frame packing only. The original has 4 bytes of locals (push ecx) and
   puts text in the dead png_ptr arg slot, ret in the comp_type slot, tmp in the local; ours has 8 bytes, text in
   the comp_type slot, ret and tmp in locals, so every [esp+N] differs. Tried: ret / tmp / text_size declaration
   order and scope, png_struct typedef, inverted comp_type if, permuter 234 variants (86.3% only by moving
   png_free before the memcpy, which changes behaviour: not kept). Class (b) stack packing. */
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
typedef unsigned char png_byte;
typedef unsigned char *png_bytep;
typedef char *png_charp;
typedef unsigned int png_uint_32;
typedef unsigned int png_size_t;
typedef unsigned int uInt;
typedef char *png_structp;
MGLOBALA(char, s_Error_decoding_compressed_text_00621cc8, 31, 0x00621cc8); /* static msg[] */
MGLOBALA(char, s_Incomplete_compressed_datastream_00621d14, 57, 0x00621d14);
MGLOBALA(char, s_Unknown_zTXt_compression_type_00621cf4, 30, 0x00621cf4);
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
void *FUN_005208c0(png_structp png_ptr, png_uint_32 size); /* png_malloc */
void FUN_00520930(png_structp png_ptr, void *ptr); /* png_free */
void FUN_00517270(png_structp png_ptr, const char *message); /* png_warning */
int FUN_00522ab0(void *strm); /* inflateReset */
int FUN_00522ce0(void *strm, int flush); /* inflate */
#define msg s_Error_decoding_compressed_text_00621cc8
#define ZSTREAM (png_ptr + 0x74)
#define NEXT_IN (*(png_bytep *)(png_ptr + 0x74))
#define AVAIL_IN (*(uInt *)(png_ptr + 0x78))
#define NEXT_OUT (*(png_bytep *)(png_ptr + 0x80))
#define AVAIL_OUT (*(uInt *)(png_ptr + 0x84))
#define ZMSG (*(char **)(png_ptr + 0x8c))
#define ZBUF (*(png_bytep *)(png_ptr + 0xac))
#define ZBUF_SIZE (*(png_size_t *)(png_ptr + 0xb0))
// MATCH: 0x00523360 FUN_00523360
png_charp FUN_00523360(png_structp png_ptr, int comp_type,
                              png_charp chunkdata, png_size_t chunklength,
                              png_size_t prefix_size, png_size_t *newlength)
{
   png_charp text = 0;
   png_size_t text_size;

   if (comp_type == 0)
   {
      int ret = 0;
      NEXT_IN = (png_bytep)(chunkdata + prefix_size);
      AVAIL_IN = (uInt)(chunklength - prefix_size);
      NEXT_OUT = ZBUF;
      AVAIL_OUT = (uInt)ZBUF_SIZE;

      text_size = 0;
      text = 0;

      while (AVAIL_IN)
      {
         ret = FUN_00522ce0(ZSTREAM, 1);
         if (ret != 0 && ret != 1)
         {
            if (ZMSG != 0)
               FUN_00517270(png_ptr, ZMSG);
            else
               FUN_00517270(png_ptr, msg);
            FUN_00522ab0(ZSTREAM);
            AVAIL_IN = 0;

            if (text ==  0)
            {
               text_size = prefix_size + sizeof(msg) + 1;
               text = (png_charp)FUN_005208c0(png_ptr, text_size);
               memcpy(text, chunkdata, prefix_size);
            }

            text[text_size - 1] = 0x00;

            /* Copy what we can of the error message into the text chunk */
            text_size = (png_size_t)(chunklength - (text - chunkdata) - 1);
            text_size = sizeof(msg) > text_size ? text_size : sizeof(msg);
            memcpy(text + prefix_size, msg, text_size + 1);
            break;
         }
         if (!AVAIL_OUT || ret == 1)
         {
            if (text == 0)
            {
               text_size = prefix_size +
                   ZBUF_SIZE - AVAIL_OUT;
               text = (png_charp)FUN_005208c0(png_ptr, text_size + 1);
               memcpy(text + prefix_size, ZBUF,
                    text_size - prefix_size);
               memcpy(text, chunkdata, prefix_size);
               *(text + text_size) = 0x00;
            }
            else
            {
               png_charp tmp;

               tmp = text;
               text = (png_charp)FUN_005208c0(png_ptr, (png_uint_32)(text_size +
                  ZBUF_SIZE - AVAIL_OUT + 1));
               memcpy(text, tmp, text_size);
               FUN_00520930(png_ptr, tmp);
               memcpy(text + text_size, ZBUF,
                  (ZBUF_SIZE - AVAIL_OUT));
               text_size += ZBUF_SIZE - AVAIL_OUT;
               *(text + text_size) = 0x00;
            }
            if (ret == 1)
               break;
            else
            {
               NEXT_OUT = ZBUF;
               AVAIL_OUT = (uInt)ZBUF_SIZE;
            }
         }
      }
      if (ret != 1)
      {
         FUN_00517270(png_ptr,
            s_Incomplete_compressed_datastream_00621d14);
         text_size=prefix_size;
         if (text ==  0)
         {
            text = (png_charp)FUN_005208c0(png_ptr, text_size+1);
            memcpy(text, chunkdata, prefix_size);
         }
         *(text + text_size) = 0x00;
      }

      FUN_00522ab0(ZSTREAM);
      AVAIL_IN = 0;

      FUN_00520930(png_ptr, chunkdata);
      chunkdata = text;
      *newlength=text_size;
   }
   else /* if (comp_type != 0) */
   {
      FUN_00517270(png_ptr, s_Unknown_zTXt_compression_type_00621cf4);

      *(chunkdata + prefix_size) = 0x00;
      *newlength=prefix_size;
   }

   return chunkdata;
}
