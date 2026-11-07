// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00550c30 (h1): 100% (regs 100%) by tools/nosize.py: symbol 28 B, ends 0x00550c4c, `90` padding to 0x00550c50 (next function, no pc_funcs row), bytes_bad=[]. Index into an array of 0x60-byte slots at [fs+0x58], bound [fs+4]. */
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
typedef struct FsSlot { int w[0x18]; } FsSlot;

// MATCH: 0x00550c30 FUN_00550c30
FsSlot *FUN_00550c30(int param_1, unsigned int param_2)
{
  if (param_2 < *(unsigned int *)(param_1 + 4)) {
    return &(*(FsSlot **)(param_1 + 0x58))[param_2];
  }
  return 0;
}
