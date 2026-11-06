// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046cbe0 (44 B): 100.0% (regs 100, ops 100), by hand (h3). Raw 83.3%.
   Unsigned index, early return 0; two setters DAT_008815a0[i].+0x9f0 / +0x9f4. */
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
    char p0[0x9f0];
    int f9f0;                   /* 0x9f0 */
    int f9f4;                   /* 0x9f4 */
    char p1[0xd04 - 0x9f8];
} Car;                              /* 0xd04 */
MGLOBALA(Car, DAT_008815a0, 16, 0x008815a0);
// MATCH: 0x0046cbe0 FUN_0046cbe0
undefined4 FUN_0046cbe0(uint param_1, int param_2, int param_3)
{
  if (param_1 >= 0x10) {
    return 0;
  }
  DAT_008815a0[param_1].f9f0 = param_2;
  DAT_008815a0[param_1].f9f4 = param_3;
  return 1;
}
