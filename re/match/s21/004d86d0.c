// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d86d0 (h1, by hand): identical (nosize.py: acc 100.0, regs 100.0, bytes_bad=[], symbol ends
   0x004d86fa, then 90 padding to 0x004d8700). No Ghidra size. Raw form returned void; the original returns the
   argument in eax (in-place a..z -> A..Z). relocheck OK, C++ ok. */
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
// MATCH: 0x004d86d0 FUN_004d86d0
char *FUN_004d86d0(char *param_1)

{
  char *p;

  if (param_1 != 0) {
    for (p = param_1; *p != '\0'; p++) {
      if (*p >= 'a' && *p <= 'z') {
        *p -= 0x20;
      }
    }
  }
  return param_1;
}
