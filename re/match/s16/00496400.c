// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00496400 (h3): by hand, 100% (regs 100%), relocheck OK. Variadic cdecl log printf: the
   va_list is (char *)(&fmt + 1) passed to the vsprintf at 0x004a42c5, 512-byte /GS buffer. Stock bytes
   (disable_log patches 0x00496400 in the patched exe, not matched here). */
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
typedef struct _iobuf FILE;
MGLOBAL(FILE *, DAT_00772fbc, 0x00772fbc);
int __cdecl fputs(const char *s, FILE *f);
int __cdecl FUN_004a42c5(char *buf, const char *fmt, char *args);
// MATCH: 0x00496400 FUN_00496400
void __cdecl FUN_00496400(const char *fmt, ...)
{
  char buf[512];
  if (DAT_00772fbc != 0) {
    FUN_004a42c5(buf, fmt, (char *)(&fmt + 1));
    fputs(buf, DAT_00772fbc);
  }
}
