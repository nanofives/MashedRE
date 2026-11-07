// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d8700 (h2): 89.5% (regs 89.5%), no Ghidra size: scored with nosize.py (sym_len 42, ends at
   0x004d872a, 90 padding to 0x004d8730). strlwr shape: returns s, lowercases 'A'..'Z' (+0x20) in place.
   Residual: the pre-loop test is `cmp byte ptr [eax],0` in ours, `mov cl,[eax]; test cl,cl` in the original
   (8 bytes). Tried 9 forms (while/for/do, index, char temp before the loop, `c=*p` rotated, `s && *s`, -'A'+'a')
   and the permuter (88 compiles, no gain). Class (b): source shape of the first-character test not found. */
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
// MATCH: 0x004d8700 FUN_004d8700
char *FUN_004d8700(char *s)
{
  char *p;
  if (s) {
    p = s;
    while (*p != '\0') {
      char c = *p;
      if (c >= 'A' && c <= 'Z') *p = c + 0x20;
      p++;
    }
  }
  return s;
}
