// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042f8d0: 100% by hand (raw 22.1%). FUN_0042f8d0 is a static function with a register
   convention (the alpha byte arrives in AL): it is declared `static` here and this file holds a stand-in caller
   (Caller0042f8d0, not a function of the binary; the real callers are in 0x00434xxx/0x0043axxx) so the compiler
   gives it that convention (pilot finding 1). The four stack floats are two 8-byte structs by value (pos, size),
   and FUN_00472c60 takes (V2d, V2d, RwRGBA) by value too: that is what makes the compiler store the constant 2.0f
   into the parameter slot and reload it before the push. The size copy is one struct (s0 = size, at E-8/E-4),
   and h - 2.0f is written twice (the compiler keeps the CSE temp in the dead colour slot). */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
// FLAGS: /O2 /GS /Ob1
/* v */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
#define MATCH_STATIC
#else
#define MATCH_STATIC static
#endif
typedef struct { unsigned char r; unsigned char g; unsigned char b; unsigned char a; } Rgba0042f8d0;
typedef struct { float x; float y; } V2d0042f8d0;
void FUN_00472c60(V2d0042f8d0 pos, V2d0042f8d0 size, Rgba0042f8d0 c);
// MATCH: 0x0042f8d0 FUN_0042f8d0
MATCH_STATIC void FUN_0042f8d0(unsigned char alpha, V2d0042f8d0 pos, V2d0042f8d0 size)
{
  Rgba0042f8d0 c;
  V2d0042f8d0 s0;
  s0 = size;
  c.r = 0xf0;
  c.g = 0x6e;
  c.b = 0x14;
  c.a = alpha >> 1;
  FUN_00472c60(pos, size, c);
  pos.x = pos.x - 2.0f;
  c.r = 0xb4;
  c.g = 0x50;
  c.b = 0x10;
  c.a = alpha;
  size.x = 2.0f;
  FUN_00472c60(pos, size, c);
  size.x = s0.x + 4.0f;
  size.y = 2.0f;
  FUN_00472c60(pos, size, c);
  pos.y += s0.y - 2.0f;
  FUN_00472c60(pos, size, c);
  pos.x = s0.x + pos.x + 2.0f;
  size.x = 2.0f;
  size.y = s0.y;
  pos.y -= s0.y - 2.0f;
  FUN_00472c60(pos, size, c);
}
#ifndef __cplusplus
/* stand-in caller (not a real function of the binary): makes the compiler give the static FUN_0042f8d0 its
   register convention (alpha in AL). */
void Caller0042f8d0(unsigned char a, V2d0042f8d0 p, V2d0042f8d0 s)
{
  FUN_0042f8d0(a, p, s);
  FUN_0042f8d0(a, s, p);
}
#endif
