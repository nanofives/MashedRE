// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b51d0 (h3): hand, 94.6% by instructions = 100%: Ghidra sizes the function 2 instructions short
   (stops before add esp,0x400 / ret); match.py --bytes reports the whole obj symbol (98 B) BYTE-IDENTICAL.
   The loop needs the list entry in a temp (index-in-subscript form 50%). */
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
int FUN_004b3fc0(void *p0, void **out);
int FUN_004b5190(void *p0, unsigned int p1, unsigned int p2);
// MATCH: 0x004b51d0 FUN_004b51d0
int FUN_004b51d0(void *param_1, void **param_2, unsigned int param_3, unsigned int param_4, unsigned int param_5)
{
  void *list[256];
  int n;
  int i;

  n = FUN_004b3fc0(param_1, list);
  for (i = 0; i < n; i++) {
    void *a = list[i];
    param_2[FUN_004b5190(a, param_4, param_5)] = a;
  }
  return n;
}
