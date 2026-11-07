// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x005509b0 (h4): 100.0% (regs 100.0%). fseek-style wrapper. Interface pointer (param_1+0x38)+0x28 as a local (same as
   0x00550950), switch on origin 0/1/2 to interface slot +0x10 with 1/2/3, default -1. */
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
typedef int (*FsSeekFn)(int file, int offset, int origin);
typedef struct { int f0; int f4; int f8; int fc; FsSeekFn seek; } FsIf;
typedef struct { char pad[0x28]; FsIf fsif; } Fs;
// MATCH: 0x005509b0 FUN_005509b0
int FUN_005509b0(int param_1, int param_2, int param_3)
{
  FsIf *i = &(*(Fs **)(param_1 + 0x38))->fsif;

  if (*(int *)(param_1 + 0x18) != 0) {
    *(int *)(param_1 + 0x28) = 4;
    *(int *)(param_1 + 0x20) = 3;
  }
  switch (param_3) {
  case 0:
    i->seek(param_1, param_2, 1);
    return 0;
  case 1:
    i->seek(param_1, param_2, 2);
    return 0;
  case 2:
    i->seek(param_1, param_2, 3);
    return 0;
  }
  return -1;
}
