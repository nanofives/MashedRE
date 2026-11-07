// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0051d560: libpng 1.0.12 pngwutil.c png_text_compress (PNG_NO_STDIO warning form), png_memcpy =
   intrinsic memcpy, z_stream fields at png_struct+0x74.
   100.0% (regs 100, ops 100). Official body; keys: compression limit 3 (the image's PNG_TEXT_COMPRESSION_LAST), text_len
   copied from text_len_arg (its slot then holds old_max, 98.1 without). Residual: none. */
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
extern "C" void *__cdecl memcpy(void *d, const void *s, unsigned int n);
#else
void *__cdecl memcpy(void *d, const void *s, unsigned int n);
#endif
#pragma intrinsic(memcpy)
typedef struct
{
   char *input;
   int input_len;
   int num_output_ptr;
   int max_output_ptr;
   unsigned char **output_ptr;
} compression_state;

MGLOBALA(char, s_zlib_error_00620f0c, 11, 0x00620f0c);
MGLOBALA(char, s_Unknown_compression_type_00621900, 25, 0x00621900);
void FUN_00517250(int png_ptr, const char *msg);
void FUN_00517270(int png_ptr, const char *msg);
void *FUN_005208c0(int png_ptr, unsigned int size);
void FUN_00520930(int png_ptr, void *ptr);
int FUN_00521520(void *strm, int flush);

// MATCH: 0x0051d560 FUN_0051d560
int FUN_0051d560(int png_ptr, char *text, unsigned int text_len_arg, int compression, compression_state *comp)
{
   unsigned int text_len = text_len_arg;
   int ret;

   comp->num_output_ptr = comp->max_output_ptr = 0;
   comp->output_ptr = 0;
   comp->input = 0;

   if (compression == -1)
   {
       comp->input = text;
       comp->input_len = text_len;
       return((int)text_len);
   }

   if (compression >= 3)
   {
      FUN_00517270(png_ptr, s_Unknown_compression_type_00621900);
   }

   *(unsigned int *)(png_ptr + 0x78) = text_len;
   *(unsigned char **)(png_ptr + 0x74) = (unsigned char *)text;
   *(unsigned int *)(png_ptr + 0x84) = *(unsigned int *)(png_ptr + 0xb0);
   *(unsigned char **)(png_ptr + 0x80) = *(unsigned char **)(png_ptr + 0xac);

   do
   {
      
      ret = FUN_00521520((void *)(png_ptr + 0x74), 0);
      if (ret != 0)
      {
         
         if (*(char **)(png_ptr + 0x8c) != 0)
            FUN_00517250(png_ptr, *(char **)(png_ptr + 0x8c));
         else
            FUN_00517250(png_ptr, s_zlib_error_00620f0c);
      }
      
      if (!*(unsigned int *)(png_ptr + 0x84) && *(unsigned int *)(png_ptr + 0x78))
      {
         
         if (comp->num_output_ptr >= comp->max_output_ptr)
         {
            int old_max;

            old_max = comp->max_output_ptr;
            comp->max_output_ptr = comp->num_output_ptr + 4;
            if (comp->output_ptr != 0)
            {
               unsigned char ** old_ptr;

               old_ptr = comp->output_ptr;
               comp->output_ptr = (unsigned char **)FUN_005208c0(png_ptr,
                  (unsigned int)(comp->max_output_ptr * sizeof (unsigned char **)));
               memcpy(comp->output_ptr, old_ptr, old_max
                  * sizeof (unsigned char *));
               FUN_00520930(png_ptr, old_ptr);
            }
            else
               comp->output_ptr = (unsigned char **)FUN_005208c0(png_ptr,
                  (unsigned int)(comp->max_output_ptr * sizeof (unsigned char *)));
         }

         comp->output_ptr[comp->num_output_ptr] = (unsigned char *)FUN_005208c0(png_ptr,
            (unsigned int)*(unsigned int *)(png_ptr + 0xb0));
         memcpy(comp->output_ptr[comp->num_output_ptr], *(unsigned char **)(png_ptr + 0xac),
            *(unsigned int *)(png_ptr + 0xb0));
         comp->num_output_ptr++;

         *(unsigned int *)(png_ptr + 0x84) = *(unsigned int *)(png_ptr + 0xb0);
         *(unsigned char **)(png_ptr + 0x80) = *(unsigned char **)(png_ptr + 0xac);
      }
   
   } while (*(unsigned int *)(png_ptr + 0x78));

   do
   {
      
      ret = FUN_00521520((void *)(png_ptr + 0x74), 4);

      if (ret == 0)
      {
         
         if (!(*(unsigned int *)(png_ptr + 0x84)))
         {
            
            if (comp->num_output_ptr >= comp->max_output_ptr)
            {
               int old_max;

               old_max = comp->max_output_ptr;
               comp->max_output_ptr = comp->num_output_ptr + 4;
               if (comp->output_ptr != 0)
               {
                  unsigned char ** old_ptr;

                  old_ptr = comp->output_ptr;
                  
                  comp->output_ptr = (unsigned char **)FUN_005208c0(png_ptr,
                     (unsigned int)(comp->max_output_ptr * sizeof (unsigned char **)));
                  memcpy(comp->output_ptr, old_ptr,
                     old_max * sizeof (unsigned char *));
                  FUN_00520930(png_ptr, old_ptr);
               }
               else
                  comp->output_ptr = (unsigned char **)FUN_005208c0(png_ptr,
                     (unsigned int)(comp->max_output_ptr * sizeof (unsigned char *)));
            }

            comp->output_ptr[comp->num_output_ptr] =
               (unsigned char *)FUN_005208c0(png_ptr, (unsigned int)*(unsigned int *)(png_ptr + 0xb0));
            memcpy(comp->output_ptr[comp->num_output_ptr], *(unsigned char **)(png_ptr + 0xac),
               *(unsigned int *)(png_ptr + 0xb0));
            comp->num_output_ptr++;

            *(unsigned int *)(png_ptr + 0x84) = *(unsigned int *)(png_ptr + 0xb0);
            *(unsigned char **)(png_ptr + 0x80) = *(unsigned char **)(png_ptr + 0xac);
         }
      }
      else if (ret != 1)
      {
         
         if (*(char **)(png_ptr + 0x8c) != 0)
            FUN_00517250(png_ptr, *(char **)(png_ptr + 0x8c));
         else
            FUN_00517250(png_ptr, s_zlib_error_00620f0c);
      }
   } while (ret != 1);

   text_len = *(unsigned int *)(png_ptr + 0xb0) * comp->num_output_ptr;
   if (*(unsigned int *)(png_ptr + 0x84) < *(unsigned int *)(png_ptr + 0xb0))
      text_len += *(unsigned int *)(png_ptr + 0xb0) - (unsigned int)*(unsigned int *)(png_ptr + 0x84);

   return((int)text_len);
}
