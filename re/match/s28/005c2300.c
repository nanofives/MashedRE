// FLAGS: /O1
/* MATCH-S28 h1 0x005c2300 : Plain C strncat, not byte-comparable. The original is the CRT's hand-written assembly strncat (dword-at-a-time zero-byte scan with the 0x7efefeff / 0x81010100 trick, alignment prologues, 4 tail cases), which the C compiler cannot emit. Score is whatever match.py reports for this C. Residual class (c), CRT asm routine; the port should call the CRT or use this C. */

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

// MATCH: 0x005c2300 FUN_005c2300
char *FUN_005c2300(char *dst, char *src, unsigned int n)
{
  char *p;

  p = dst;
  if (n == 0) {
    return dst;
  }
  while (*p != 0) {
    p++;
  }
  while (n != 0) {
    n--;
    if ((*p++ = *src++) == 0) {
      return dst;
    }
  }
  *p = 0;
  return dst;
}
