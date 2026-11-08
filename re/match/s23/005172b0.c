// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x005172b0: libpng 1.0.12 pngerror.c png_chunk_error, with png_error (0x00517250) inlined
   (written inline here: error_fn at +0x40, then longjmp(png_ptr->jmpbuf, 1)). Ghidra size 53 stops at the noreturn call: the whole obj symbol (54 B)
   is BYTE-IDENTICAL (match.py --bytes). */
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
void FUN_005172f0(int *png_ptr, char *buffer, const char *message);
// MATCH: 0x005172b0 FUN_005172b0
void FUN_005172b0(int *png_ptr, const char *message)
{
   char msg[18+64];
   FUN_005172f0(png_ptr, msg, message);
   if (*(png_error_ptr *)((char *)png_ptr + 0x40) != 0)
      (*(*(png_error_ptr *)((char *)png_ptr + 0x40)))(png_ptr, msg);

   longjmp(png_ptr, 1);
}
