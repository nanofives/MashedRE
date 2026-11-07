// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051d800: libpng 1.0.12 pngwutil.c png_write_compressed_data_out, png_write_chunk_data written
   inline at each call.
   100.0% (regs 100, ops 100). Keys: in the loop's inlined chunk_data, length (zbuf_size) read before data;
   png_ptr copied from png_ptr_arg so the length spill lands in the png_ptr arg slot (98.0 without). Residual: none. */
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

void FUN_00516bb0(int png_ptr, unsigned char *data, unsigned int length);
void FUN_00516d50(int png_ptr, unsigned char *ptr, unsigned int length);
void FUN_00520930(int png_ptr, void *ptr);
int FUN_00521410(void *strm);

// MATCH: 0x0051d800 FUN_0051d800
void FUN_0051d800(int png_ptr_arg, compression_state *comp)
{
   int i;
   int png_ptr = png_ptr_arg;

   if (comp->input)
   {
      unsigned char *data = (unsigned char *)comp->input;
      unsigned int length = (unsigned int)comp->input_len;
      if (data != 0 && length > 0)
      {
         FUN_00516d50(png_ptr, data, length);
         FUN_00516bb0(png_ptr, data, length);
      }
      return;
   }

   for (i = 0; i < comp->num_output_ptr; i++)
   {
      {
         unsigned int length = *(unsigned int *)(png_ptr + 0xb0);
         unsigned char *data = comp->output_ptr[i];
         if (data != 0 && length > 0)
         {
            FUN_00516d50(png_ptr, data, length);
            FUN_00516bb0(png_ptr, data, length);
         }
      }
      FUN_00520930(png_ptr, comp->output_ptr[i]);
      comp->output_ptr[i] = 0;
   }
   if (comp->max_output_ptr != 0)
      FUN_00520930(png_ptr, comp->output_ptr);
   comp->output_ptr = 0;
   if (*(unsigned int *)(png_ptr + 0x84) < *(unsigned int *)(png_ptr + 0xb0))
   {
      unsigned char *data = *(unsigned char **)(png_ptr + 0xac);
      unsigned int length = *(unsigned int *)(png_ptr + 0xb0) - *(unsigned int *)(png_ptr + 0x84);
      if (data != 0 && length > 0)
      {
         FUN_00516d50(png_ptr, data, length);
         FUN_00516bb0(png_ptr, data, length);
      }
   }

   FUN_00521410((void *)(png_ptr + 0x74));
}
