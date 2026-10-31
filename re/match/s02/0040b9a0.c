// FLAGS: /O2 /GS /Ob1
/* 0x0040b9a0 (Ghidra 92 B / 95 B symbol): whole obj symbol BYTE-IDENTICAL (match.py --bytes). The
   instruction score reads 91.9% only because Ghidra's size stops 3 bytes short (pop esi; pop ebx; ret).
   Raw g2s 81.1%. Form: max over the int[4] at 0x008a94e0 of the slots where param_2 == 0 or
   FUN_0046c7b0(i) != 0, then the same mark loop as 0x0040ba00. Residual: none. */
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

int FUN_0046c7b0(int p0);

// MATCH: 0x0040b9a0 FUN_0040b9a0
void FUN_0040b9a0(int *param_1, int param_2)
{
  int best;
  int i;

  best = 0;
  for (i = 0; i < 4; i++) {
    if (param_2 == 0 || FUN_0046c7b0(i) != 0) {
      if (DAT_008a94e0[i] > best) {
        best = DAT_008a94e0[i];
      }
    }
  }
  for (i = 0; i < 4; i++) {
    param_1[i] = 0;
    if (DAT_008a94e0[i] == best) {
      param_1[i] = 1;
    }
  }
}
