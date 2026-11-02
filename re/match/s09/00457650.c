// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00457650 (h6): 100% (whole obj symbol byte-identical, 91 B; Ghidra size 88 stops before the
   pop ebp/pop ebx/ret epilogue, so the instruction score reads 92.5%). Raw converter was 23.7%: the source form is a
   counted for (i < 64) over a 64 x 0x68 B array (dword index i*0x1a) with an int-typed bit mask, which the
   compiler strength-reduces to the pointer compare and jl. Residual: none. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef unsigned char byte;
typedef unsigned int undefined4;
MGLOBALA(undefined4, DAT_00688858, 0x680, 0x00688858);
MGLOBALA(byte, DAT_0068a300, 8, 0x0068a300);
int FUN_0044d5e0(undefined4 *p0);
// MATCH: 0x00457650 FUN_00457650
void FUN_00457650(void)
{
  int i;
  for (i = 0; i < 64; i++) {
    if (DAT_0068a300[i >> 3] & (1 << (i & 7))) {
      if (FUN_0044d5e0(&DAT_00688858[i * 0x1a]) != 0) {
        DAT_0068a300[i >> 3] &= ~(1 << (i & 7));
      }
    }
  }
}
