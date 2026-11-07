// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005aeaf0: 100% by hand: calloc-shaped (n=a*b; p=FUN_004a45fb(n) [CRT malloc]; if(p) memset(p,0,n)) with #pragma intrinsic(memset). cxx pending. */
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
typedef unsigned int size_t;
void * FUN_004a45fb(size_t p0);
void * memset(void *, int, size_t);
#pragma intrinsic(memset)
// MATCH: 0x005aeaf0 FUN_005aeaf0
void * FUN_005aeaf0(int param_1,int param_2)
{
  void *p;
  size_t n;

  n = param_1 * param_2;
  p = FUN_004a45fb(n);
  if (p != 0) {
    memset(p,0,n);
  }
  return p;
}
