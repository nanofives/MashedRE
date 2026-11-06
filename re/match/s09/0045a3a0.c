// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x0045a3a0 (h9, by hand): best 81.0% (regs 90.5%). Raw converter 71.4%. Array of 4x5x5 records of
   0x34 bytes at 0x0068bd00 (pointer bias +4 is the compiler's: the loop starts at 0x0068d76c and ends at
   0x0068c31c). Moved it: do/while counters counting down at all three levels with a 3-D array index (the loop
   body is now instruction-identical, store order f2,f1,f0,f5,f4,f3,f8,f6,f11,f12 is source order); for-loops
   descending merged two levels (35.7%), ascending unrolled (10.6-25%), three explicit pointers 64-76%.
   Residual (b): the original keeps param_1 in EBP from entry (push ebp; mov ebp,[esp+0xc]), ours loads it into
   EAX after the loop. Tried a local copy, a float-struct param type, permuter (no improvement). */
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
typedef struct { int f[13]; } S34;
typedef S34 S34x5x5[5][5];
MGLOBALA(S34x5x5, DAT_0068bd00, 4, 0x0068bd00);
// MATCH: 0x0045a3a0 FUN_0045a3a0
void FUN_0045a3a0(int *param_1)
{
  int i, j, k;
  S34 *e;
  i = 4;
  do {
    i--;
    j = 5;
    do {
      j--;
      k = 5;
      do {
        k--;
        e = &DAT_0068bd00[i][j][k];
        e->f[2] = 0; e->f[1] = 0; e->f[0] = 0; e->f[5] = 0; e->f[4] = 0;
        e->f[3] = 0; e->f[8] = 0; e->f[6] = 0; e->f[11] = 0; e->f[12] = 0;
      } while (k != 0);
    } while (j != 0);
  } while (i != 0);
  param_1[0] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  param_1[3] = 0x3f800000;
}
