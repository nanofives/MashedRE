// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x004963e0 (h3): by hand, 100% (regs 100%), relocheck OK (fputs = CRT 0x004a4254, unchecked
   by name). Stock bytes (disable_log patches 0x004963e7 in the patched exe, not matched here). */
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
// MATCH: 0x004963e0 FUN_004963e0
void FUN_004963e0(char *param_1)
{
  if (DAT_00772fbc != 0) {
    fputs(param_1, DAT_00772fbc);
  }
}
