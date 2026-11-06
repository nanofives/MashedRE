// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047abd0 (h1): 100% (regs 100%). Lua C function, 6 args into the copter record (floats +0x200/+0x1f8/
   +0x1fc, bytes +0x1f4..+0x1f6, +0x1f7 = 0xff). Raw 94.3%. What moved it: DAT_006bf1cc file-static (pilot finding 2),
   so the last byte stores reuse the loaded pointer. Residual: none. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
#ifndef __cplusplus
static char *DAT_006bf1cc;
#else
MGLOBAL(char *, DAT_006bf1cc, 0x006bf1cc);
#endif
int FUN_004b6fc0(void *L);
double FUN_004b7090(void *L, int idx);

// MATCH: 0x0047abd0 FUN_0047abd0
int FUN_0047abd0(void *L)
{
  int top;

  top = FUN_004b6fc0(L);
  *(float *)(DAT_006bf1cc + 0x200) = (float)FUN_004b7090(L, top - 5);
  *(float *)(DAT_006bf1cc + 0x1f8) = (float)FUN_004b7090(L, top - 4);
  *(float *)(DAT_006bf1cc + 0x1fc) = (float)FUN_004b7090(L, top - 3);
  *(unsigned char *)(DAT_006bf1cc + 0x1f4) = (unsigned char)FUN_004b7090(L, top - 2);
  *(unsigned char *)(DAT_006bf1cc + 0x1f5) = (unsigned char)FUN_004b7090(L, top - 1);
  *(unsigned char *)(DAT_006bf1cc + 0x1f6) = (unsigned char)FUN_004b7090(L, top);
  *(unsigned char *)(DAT_006bf1cc + 0x1f7) = 0xff;
  return 0;
}
