// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00456d00: 100% by hand (h5), relocheck OK. Raw 75% (in_EAX). static with one pointer parameter so cl
   passes it in EAX (pilot finding 1). Real callers 0x00456d20 (call at 0x00456d29) and the one calling at 0x0045777a,
   neither in this file, so a stand-in caller FUN_00456d00_caller sits below it (not the real TU). 16-byte records
   at 0x0068a250. FUN_00474d60 is cdecl (ptr, int). */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct Rec00456d00 {
  float m_0;
  int m_4;
  int m_8;
  int m_c;
} Rec00456d00;
void FUN_00474d60(int p0, int p1);

// MATCH: 0x00456d00 FUN_00456d00
static void FUN_00456d00(Rec00456d00 *r)
{
  r->m_0 = 1.0f;
  r->m_8 = 0;
  r->m_4 = 0;
  FUN_00474d60(r->m_c, 0);
}

/* stand-in caller (not the real TU): makes cl pass r in EAX, as 0x00456d27 does (mov eax, esi) */
void FUN_00456d00_caller(Rec00456d00 *r)
{
  FUN_00456d00(r);
}
