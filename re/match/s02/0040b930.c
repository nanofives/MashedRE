// FLAGS: /O2 /GS /Ob1
/* 0x0040b930 FUN_0040b930 (51 B): out[i] = DAT_008a94e0[i] >= FUN_0040b8e0() for i 0..3. Best: 100% exact
   (regs 100, ops 100). Raw Ghidra 89.5%; written as a plain indexed for loop (the compiler makes the
   0x008a94e0 - out pointer difference itself). FUN_0040b8e0 takes no argument. Residual: none. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
MGLOBALA(int, DAT_008a94e0, 4, 0x008a94e0);

int FUN_0040b8e0(void);

// MATCH: 0x0040b930 FUN_0040b930
void FUN_0040b930(int *param_1)
{
  int i;
  for (i = 0; i < 4; i++) {
    param_1[i] = 0;
    if (DAT_008a94e0[i] >= FUN_0040b8e0()) {
      param_1[i] = 1;
    }
  }
}
