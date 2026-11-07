// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00517200: libpng 1.0.12 png.c png_handle_as_unknown. Official source; chunk_list at +0x1d4,
   num_chunk_list at +0x1d0. 100%: the early-return test written as the positive condition around the loop
   (official form gives 68.6, the pushes move above the test). */
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
int __cdecl memcmp(const void *a, const void *b, unsigned int n);
#pragma intrinsic(memcmp)
// MATCH: 0x00517200 FUN_00517200
int FUN_00517200(int png_ptr, unsigned char *chunk_name)
{
   int i;
   unsigned char *p;
   if((png_ptr != 0 || chunk_name != 0) && *(int *)(png_ptr + 0x1d0)>0) {
   p=*(unsigned char **)(png_ptr + 0x1d4)+*(int *)(png_ptr + 0x1d0)*5-5;
   for (i = *(int *)(png_ptr + 0x1d0); i; i--, p-=5)
      if (!memcmp(chunk_name, p, 4))
        return ((int)*(p+4));
   }
   return 0;
}
