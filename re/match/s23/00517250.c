// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00517250: libpng 1.0.12 pngerror.c png_error (no PNG_ERROR_NUMBERS; png_default_error inlined
   as longjmp(png_ptr->jmpbuf, 1); jmpbuf at +0, error_fn at +0x40). Ghidra size 31 stops at the noreturn call; the whole obj symbol
   (32 B, trailing pop esi) is BYTE-IDENTICAL (match.py --bytes). */
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
typedef void (*png_error_ptr)(void *png_ptr, const char *msg);
#ifdef __cplusplus
extern "C"
#endif
__declspec(noreturn) void __cdecl longjmp(int *buf, int v);
// MATCH: 0x00517250 FUN_00517250
void FUN_00517250(int *png_ptr, const char *message)
{
   if (*(png_error_ptr *)((char *)png_ptr + 0x40) != 0)
      (*(*(png_error_ptr *)((char *)png_ptr + 0x40)))(png_ptr, message);

   longjmp(png_ptr, 1);
}
