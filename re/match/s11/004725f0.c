// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x004725f0 (h7): 100%. for loop over the 10-record table at 0x691500. Residual: none. */
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
typedef unsigned int undefined4;
typedef struct {
    undefined4 a;    /* 0x0 */
    undefined4 b;    /* 0x4 */
    undefined4 c;    /* 0x8 */
    undefined4 d;    /* 0xc */
} Rec00691500;
MGLOBALA(Rec00691500, DAT_00691500, 10, 0x00691500);
undefined4 FUN_0046be10(undefined4 *param_1, unsigned int param_2, int param_3, undefined4 param_4);
// MATCH: 0x004725f0 FUN_004725f0
void FUN_004725f0(void)
{
  int i;
  for (i = 0; i < 10; i++) {
    DAT_00691500[i].c = 0;
    DAT_00691500[i].d = 0;
    FUN_0046be10(&DAT_00691500[i].a, i, i, 0);
    FUN_0046be10(&DAT_00691500[i].b, i, i + 10, 0);
  }
}
