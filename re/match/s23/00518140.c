// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00518140: libpng 1.0.12 pngread.c png_read_destroy (PNG_FREE_ME_SUPPORTED, no time_buffer /
   save_buffer frees). png_struct is 0x1f4 bytes in the game; offsets from the decomp. gamma_shift (+0x158)
   is an int in the game struct (a byte gives 83.4). 100% official statement order. */
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
void *__cdecl memcpy(void *d, const void *s, unsigned int n);
void *__cdecl memset(void *d, int c, unsigned int n);
#ifdef __cplusplus
}
#endif
#pragma intrinsic(memcpy, memset)
typedef void (*png_error_ptr)(void *, const char *);
typedef void (*png_free_ptr)(void *, void *);
#define PS(T, off) (*(T *)((char *)png_ptr + (off)))
void FUN_005171a0(void *png_ptr, void *info_ptr);
void FUN_00520930(void *png_ptr, void *ptr);
void FUN_00516d20(void *png_ptr, void *ptr);
int FUN_00522b00(void *strm);
// MATCH: 0x00518140 FUN_00518140
void FUN_00518140(void *png_ptr, void *info_ptr, void *end_info_ptr)
{
   int tmp_jmp[16];
   png_error_ptr error_fn;
   png_error_ptr warning_fn;
   void *error_ptr;
   png_free_ptr free_fn;

   if (info_ptr != 0)
      FUN_005171a0(png_ptr, info_ptr);

   if (end_info_ptr != 0)
      FUN_005171a0(png_ptr, end_info_ptr);

   FUN_00520930(png_ptr, PS(void *, 0xac));
   FUN_00520930(png_ptr, PS(void *, 0xec));
   FUN_00520930(png_ptr, PS(void *, 0xe8));
   FUN_00520930(png_ptr, PS(void *, 0x1a0));
   FUN_00520930(png_ptr, PS(void *, 0x1a4));
   FUN_00520930(png_ptr, PS(void *, 0x164));
   FUN_00520930(png_ptr, PS(void *, 0x168));
   FUN_00520930(png_ptr, PS(void *, 0x16c));
   if (PS(unsigned int, 0x1c4) & 0x1000)
      FUN_00516d20(png_ptr, PS(void *, 0x114));
   PS(unsigned int, 0x1c4) &= ~0x1000;
   if (PS(unsigned int, 0x1c4) & 0x2000)
      FUN_00520930(png_ptr, PS(void *, 0x188));
   PS(unsigned int, 0x1c4) &= ~0x2000;
   if (PS(unsigned int, 0x1c4) & 0x0008)
      FUN_00520930(png_ptr, PS(void *, 0x1a8));
   PS(unsigned int, 0x1c4) &= ~0x0008;
   if (PS(void **, 0x170) != 0)
   {
      int i;
      int istop = (1 << (8 - PS(int, 0x158)));
      for (i = 0; i < istop; i++)
      {
         FUN_00520930(png_ptr, PS(void **, 0x170)[i]);
      }
   FUN_00520930(png_ptr, PS(void **, 0x170));
   }
   if (PS(void **, 0x174) != 0)
   {
      int i;
      int istop = (1 << (8 - PS(int, 0x158)));
      for (i = 0; i < istop; i++)
      {
         FUN_00520930(png_ptr, PS(void **, 0x174)[i]);
      }
   FUN_00520930(png_ptr, PS(void **, 0x174));
   }
   if (PS(void **, 0x178) != 0)
   {
      int i;
      int istop = (1 << (8 - PS(int, 0x158)));
      for (i = 0; i < istop; i++)
      {
         FUN_00520930(png_ptr, PS(void **, 0x178)[i]);
      }
   FUN_00520930(png_ptr, PS(void **, 0x178));
   }

   FUN_00522b00((char *)png_ptr + 0x74);

   memcpy(tmp_jmp, png_ptr, sizeof (tmp_jmp));

   error_fn = PS(png_error_ptr, 0x40);
   warning_fn = PS(png_error_ptr, 0x44);
   error_ptr = PS(void *, 0x48);
   free_fn = PS(png_free_ptr, 0x1f0);

   memset(png_ptr, 0, 0x1f4);

   PS(png_error_ptr, 0x40) = error_fn;
   PS(png_error_ptr, 0x44) = warning_fn;
   PS(void *, 0x48) = error_ptr;
   PS(png_free_ptr, 0x1f0) = free_fn;

   memcpy(png_ptr, tmp_jmp, sizeof (tmp_jmp));

}
