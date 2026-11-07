// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d8730 (h2): 100% (regs 100%), no Ghidra size: scored with nosize.py, acc 100, bytes_bad=[],
   symbol ends at 0x004d874a followed by 90 padding to 0x004d8750. strchr shape.
   What moved it: the result variable `r = 0` set before the loop and assigned + `break` on a hit (the original
   keeps the 0 in esi). Early `return s` / `return 0` forms gave 60% (xor eax,eax instead of esi). */
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
// MATCH: 0x004d8730 FUN_004d8730
char *FUN_004d8730(char *s, char c)
{
  char *r = 0;
  char ch;
  do { ch = *s; if (ch == c) { r = s; break; } s++; } while (ch);
  return r;
}
