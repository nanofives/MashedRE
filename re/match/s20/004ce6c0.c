// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004ce6c0 (h3): 100% (regs 100%; same under the game flags). result = 0; FUN_004ce480(path, 0x14, FUN_004ce6f0, &result); returns result. FUN_004ce6f0 is a function (the converter had it as a byte global). Relocheck OK. */
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
typedef int (*FileCB)(void *a, void *b);
int FUN_004ce6f0(void *a, void *b);
int FUN_004ce480(const char *path, int mode, FileCB cb, void *data);

// MATCH: 0x004ce6c0 FUN_004ce6c0
void *FUN_004ce6c0(const char *param_1)
{
  void *result;
  result = 0;
  FUN_004ce480(param_1, 0x14, FUN_004ce6f0, &result);
  return result;
}
