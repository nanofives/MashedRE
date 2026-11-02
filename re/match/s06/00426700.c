// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00426700 (h1, by hand): 100% / regs 100% over the whole 125 B obj symbol (BYTE-IDENTICAL
   under match.py --bytes; Ghidra size 118 B stops before the loop tail jl/pops/ret, so the plain score reads 86.0%).
   Moves: param_3 unsigned (fild + 2^32 fadd fixup), guarded do-while loop (if count>0 { do..while }) instead of a
   for loop (put the ebx/esi pushes inside the guard), 0x0c-stride char pointer at param_2+0x74. Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
MGLOBAL(float, DAT_005cc94c, 0x005cc94c);
MGLOBAL(float, DAT_005cc948, 0x005cc948);
void FUN_004756e0(int p0, int p1, float *p2, float p3, int p4);
// MATCH: 0x00426700 FUN_00426700
void FUN_00426700(int param_1, int param_2, unsigned int param_3)
{
  float f;
  int i;
  char *p;

  f = (float)param_3 * DAT_005cc948;
  i = 0;
  if (0 < *(int *)(param_2 + 0xd0)) {
    p = (char *)(param_2 + 0x74);
    do {
      FUN_004756e0(*(int *)(param_1 + 0x10110 + p[-4] * 4),
                   *(int *)(param_1 + 0x105b0 + p[-3] * 4), (float *)p, f, *(int *)(p + 4));
      i++;
      p += 0xc;
    } while (i < *(int *)(param_2 + 0xd0));
  }
}
