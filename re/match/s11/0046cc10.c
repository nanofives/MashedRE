// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046cc10 (41 B): 100.0% (regs 100, ops 100), by hand (h3). Raw 58.3%.
   Unsigned index, early return 0; *out = (int)DAT_008815a0[i].+0x9e0 (float); the call is the CRT
   _ftol at 0x004a2c48 the compiler emits for the cast. */
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
typedef struct {
    char p0[0x9e0];
    float f9e0;                 /* 0x9e0 */
    char p1[0xd04 - 0x9e4];
} Car;                              /* 0xd04 */
MGLOBALA(Car, DAT_008815a0, 16, 0x008815a0);
// MATCH: 0x0046cc10 FUN_0046cc10
undefined4 FUN_0046cc10(int *param_1, uint param_2)
{
  if (param_2 >= 0x10) {
    return 0;
  }
  *param_1 = (int)DAT_008815a0[param_2].f9e0;
  return 1;
}
