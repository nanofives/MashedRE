// FLAGS: /O2 /GS /Ob1
/* 0x0040b090 FUN_0040b090 (236 B): 100.0% exact, regs 100.0%.
   Raw g2s was 59.6%. Hand form: switch with cases 0x11/0x12 each calling FUN_004292d0 + FUN_004292c0 (the
   compiler tail-merges them into the push/jmp form; one pointer variable gave mov eax,0 + push eax, 82.9%),
   nested i/j loops with break on the colour codes, -0.01f = 0xbc23d70a, 0.5f = 0x005cc32c.
   Note for the port: this caller passes 2 arguments to FUN_0040ae30, which reads 1 (re/match/s02/0040ae30.c
   declares 1); the prototype here is (int, float). Residual: none. */
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
MGLOBAL(float, DAT_0063b8e8, 0x0063b8e8);
MGLOBAL(float, DAT_005f2568, 0x005f2568);
MGLOBAL(undefined4, DAT_0066d728, 0x0066d728);

int FUN_0055dec0(undefined4 *p0);
void FUN_004292d0(undefined4 *p0, float *p1);
void FUN_004292c0(undefined4 *p0, int p1);
void FUN_0040aef0(int p0);
undefined4 FUN_0046d320(int *p0, int p1, int p2);
undefined4 FUN_0046d360(int *p0, int p1, int p2);
void FUN_0040ad40(int p0, float p1);
void FUN_0040ad90(int p0, float p1, float p2);
/* 0x0040ae30 reads one argument; this caller pushes two */
void FUN_0040ae30(int p0, float p1);

// MATCH: 0x0040b090 FUN_0040b090
void FUN_0040b090(void)
{
  int i;
  int j;
  int on;
  int col[4];

  switch (FUN_0055dec0(&DAT_0066d728)) {
  case 0x11:
    FUN_004292d0(&DAT_0066d728, &DAT_005f2568);
    FUN_004292c0(&DAT_0066d728, 0);
    break;
  case 0x12:
    FUN_004292d0(&DAT_0066d728, &DAT_0063b8e8);
    FUN_004292c0(&DAT_0066d728, 0);
    break;
  }
  for (i = 0; i < 4; i++) {
    FUN_0040aef0(i);
    for (j = 0; j < 4; j++) {
      FUN_0046d320(&col[j], i, j);
      FUN_0046d360(&on, i, j);
      if (on != 0) {
        if (col[j] == (int)0xff00ffff) break;
        if (col[j] == (int)0xff030303) {
          FUN_0040ad40(i, -0.01f);
        }
        if (col[j] == (int)0xffff00ff) {
          FUN_0040ad90(i, 0.0f, DAT_0063b8e8 * 0.5f);
          break;
        }
        if (col[j] == (int)0xff1a3380) {
          FUN_0040ae30(i, -0.01f);
          break;
        }
        if (col[j] == (int)0xffffbd31) break;
      }
    }
  }
}
