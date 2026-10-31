// FLAGS: /O2 /GS /Ob1
/* 0x0040cf80 (78 B): 100.0% exact, regs 100.0%.
   Raw g2s was 72.7%: Ghidra's bool flag compiled as a byte. An int flag and an int-returning
   FUN_0041f320 give the original. Residual: none. */
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
void FUN_0040cf40(void);
void FUN_0041a980(void);
void FUN_0041ec00(int p0);
int FUN_0041f320(int p0);
void FUN_00422140(void);
void FUN_0045b350(void);

// MATCH: 0x0040cf80 FUN_0040cf80
void FUN_0040cf80(void)
{
  int any;
  int i;

  any = 0;
  i = 0;
  do {
    if (FUN_0041f320(i) != 0) {
      any = 1;
    }
    i = i + 1;
  } while (i < 4);
  if (any) {
    FUN_0041a980();
    FUN_00422140();
    i = 0;
    do {
      FUN_0041ec00(i);
      i = i + 1;
    } while (i < 4);
    FUN_0045b350();
    FUN_0040cf40();
  }
}
