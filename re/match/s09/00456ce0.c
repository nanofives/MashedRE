// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00456ce0: 100% by hand (h5), relocheck OK. Raw 70% (in_EAX). static with one pointer parameter so cl
   passes it in EAX (pilot finding 1). The real caller is 0x00456d20 (call at 0x00456d42, mov eax, edi first), which
   is not in this file, so a stand-in caller FUN_00456ce0_caller sits below it (not the real TU). Records of 0x68
   bytes at 0x00688810. */
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
typedef struct Rec00456ce0 {
  int m_0;
  int m_4;
  int pad[20];
  int m_58;
  float m_5c;
  int m_60;
  int m_64;
} Rec00456ce0;

// MATCH: 0x00456ce0 FUN_00456ce0
static void FUN_00456ce0(Rec00456ce0 *r)
{
  r->m_0 = 0;
  r->m_60 = 0;
  r->m_5c = -1.0f;
  r->m_58 = 0;
  r->m_4 = 0;
}

/* stand-in caller (not the real TU): makes cl pass r in EAX, as 0x00456d40 does (mov eax, edi) */
void FUN_00456ce0_caller(Rec00456ce0 *r)
{
  FUN_00456ce0(r);
}
