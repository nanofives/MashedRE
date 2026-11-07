// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00555830 (h2): 100% (regs 100%), byte-identical. The raw converter's 98.5% came from its
   `if (count != 0) do..while` (je); an unsigned for loop over the 0x20-byte entries gives the original's jbe.
   FUN_005c4d30 / FUN_005c4d50 take one argument each. */
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
typedef void (*rwfn1_t)(int);
typedef void (*rwfn2_t)(int, int);
typedef struct { int pad[6]; int obj; int pad2; } Entry20;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00912a08, 0x00912a08);
int FUN_004c5a60(int p0);
int FUN_00553e80(int p0);
Entry20 *FUN_005c4d30(int p0);
int FUN_005c4d50(int p0);
// MATCH: 0x00555830 FUN_00555830
int FUN_00555830(int param_1)
{
  unsigned int i;
  int *pp;
  int n;

  if (param_1 != 0) {
    if (*(int *)(param_1 + 0x134) != 0) {
      Entry20 *e = FUN_005c4d30(*(int *)(param_1 + 0x134));
      for (i = 0; i < *(unsigned int *)(param_1 + 0x130); i++) {
        if (e[i].obj != 0) {
          FUN_00553e80(e[i].obj);
          e[i].obj = 0;
        }
      }
      FUN_005c4d50(*(int *)(param_1 + 0x134));
    }
    *(int *)(param_1 + 0x134) = 0;
    if (*(int *)(param_1 + 300) != 0) {
      (*(rwfn1_t *)(DAT_007d3ff8 + 0x10c))(*(int *)(param_1 + 300));
    }
    *(int *)(param_1 + 300) = 0;
    for (n = 0; n < 4; n++) {
      pp = (int *)(param_1 + 0x14) + n;
      if (*pp != 0) {
        FUN_004c5a60(*pp);
      }
      *pp = 0;
    }
    (*(rwfn2_t *)(DAT_007d3ff8 + 0x11c))(DAT_00912a08, param_1);
    return 1;
  }
  return 0;
}
