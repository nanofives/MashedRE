// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00550950 (h4): 100.0% (regs 100.0%). fread-style wrapper: (param_4+0x38)->interface at +0x28,
   slot +8 called with (param_4, param_1, param_2*param_3), result / param_2. The interface pointer must be a
   separate local (&...->fsif); written inline the compiler folds +0x28 into the slot offset (70.6%). */
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
typedef unsigned int (*rd_t)(int, int, unsigned int);
typedef struct { int a; int b; rd_t read; } FsIf;
typedef struct { char pad[0x28]; FsIf fsif; } Fs;
// MATCH: 0x00550950 FUN_00550950
unsigned int FUN_00550950(int param_1, unsigned int param_2, int param_3, int param_4)
{
  FsIf *i = &(*(Fs **)(param_4 + 0x38))->fsif;
  return i->read(param_4, param_1, param_2 * param_3) / param_2;
}
