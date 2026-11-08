// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00516da0: libpng 1.0.12 png.c png_create_info_struct (png_create_struct_2 + png_info_init
   inlined as memset 0x120). */
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
extern "C"
#endif
void *__cdecl memset(void *d, int c, unsigned int n);
#pragma intrinsic(memset)
void *FUN_005207c0(int type, void *malloc_fn, void *mem_ptr);
// MATCH: 0x00516da0 FUN_00516da0
void *FUN_00516da0(int png_ptr)
{
   void *info_ptr;

   if (png_ptr == 0)
      return (0);
   info_ptr = FUN_005207c0(2, *(void **)(png_ptr + 0x1ec), *(void **)(png_ptr + 0x1e8));
   if (info_ptr != 0)
      memset(info_ptr, 0, 0x120);

   return (info_ptr);
}
