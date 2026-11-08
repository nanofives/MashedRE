// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b8130: 100.0% (regs 100.0%). Hand: Q struct {Ent* base; ushort cap,w6,w8,wa; HANDLE h}, 0x14-byte Ent copied as ints. */
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
typedef unsigned short ushort;
typedef unsigned int uint;
typedef void *HANDLE;
typedef int BOOL;
typedef unsigned long DWORD;
typedef long LONG;
typedef struct Ent { int a, b, c, d, e; } Ent;
typedef struct Q { Ent *base; ushort cap; ushort w6; ushort w8; ushort wa; HANDLE h; } Q;
__declspec(dllimport) BOOL __stdcall ReleaseSemaphore(HANDLE p0, LONG p1, LONG *p2);
__declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE p0, DWORD p1);
// MATCH: 0x005b8130 FUN_005b8130
int FUN_005b8130(Q *q, int *out)
{
  int *src;
  uint n;

  WaitForSingleObject(q->h, 0xffffffff);
  if (q->w6 == q->w8 && q->wa == 0) {
    ReleaseSemaphore(q->h, 1, (LONG *)0);
    return 0;
  }
  src = (int *)(q->base + q->w8);
  out[1] = src[1];
  out[0] = src[0];
  out[4] = src[4];
  out[3] = src[3];
  out[2] = src[2];
  n = q->w8 + 1;
  q->wa = 0;
  if (n == q->cap) {
    n = 0;
  }
  q->w8 = (ushort)n;
  ReleaseSemaphore(q->h, 1, (LONG *)0);
  return 1;
}
