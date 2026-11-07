// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005b09c0 (161 B). Best 79.7% (regs 81.2%), residual class (b). ops 96.9%: list walk + shift/div rounding, callee 0x005b1140 variadic (4 args); residual is when *param_2 byte is loaded (orig loads flags byte into dl after v, we fold into test byte ptr) and shift reg choice; permuter 240s no gain. */
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
void FUN_005b1140(int p0, int p1, int p2, ...);
// MATCH: 0x005b09c0 FUN_005b09c0
void FUN_005b09c0(int param_1,unsigned char *param_2)
{
  int *n;
  int off;
  n = *(int **)(param_1 + 0x20);
  if (n != (int *)(param_1 + 0x20)) {
    off = 0;
    do {
      int *e = n - 0xb;
      if (*(int *)(*(int *)(*e + 0x54) + 0xc) != 0) {
        unsigned int v = **(unsigned int **)(*(int *)(*(int *)(param_1 + 0xc) + 0x18) + 0x14 + off);
        unsigned char fl = *param_2;
        unsigned int sh = *(unsigned int *)(param_1 + 0x44);
        if ((fl & 2) != 0 && sh < 0x20) {
          unsigned int m = *(unsigned int *)(param_1 + 0x40);
          int k = 2;
          while (v == m && m != 0) { k--; m = m >> 1; }
          sh = sh - k;
          v = ((v << sh) / *(unsigned int *)(param_1 + 0x4c)) * *(unsigned int *)(param_1 + 0x4c) >> sh;
        }
        FUN_005b1140((int)e, e[0x16], 0, v);
      }
      n = (int *)*n;
      off += 0x28;
    } while (n != (int *)(param_1 + 0x20));
  }
}
