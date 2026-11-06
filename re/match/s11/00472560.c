// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00472560 (h7): 100%. 16-byte record table at 0x691500; i=0 assigned before the row pointer and a do-while
   placed xor esi first (for-loop form gave 97.3%, indexing the global in the loop 68.4%). Residual: none. */
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
MGLOBALA(Rec00691500, DAT_00691500, 1, 0x00691500);
undefined4 FUN_0046be10(undefined4 *param_1, unsigned int param_2, int param_3, undefined4 param_4);
// MATCH: 0x00472560 FUN_00472560
void FUN_00472560(int param_1,int param_2)
{
  int i;
  Rec00691500 *r;
  i = 0;
  r = &DAT_00691500[param_1];
  do {
    FUN_0046be10(&r->a, i, param_1, param_2 == 0);
    FUN_0046be10(&r->b, i, param_1 + 10, param_2);
    i++;
  } while (i < 4);
  r->c = param_2;
}
