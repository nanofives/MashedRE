// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046be10 (55 B): 100.0% (regs 100, ops 100), by hand (h3).
   Raw 37.5%. What moved it: the per-car record as a struct array (DAT_008815a0[i], stride 0xd04) with two
   32-dword arrays at +0xbfc / +0xc7c instead of Ghidra's flat (&DAT)[i*0x341+j] (gives imul;add;shl;[eax+disp]),
   and `if (i >= 0x10) return 0;` first. */
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
    char p0[0xbfc];
    undefined4 a[32];               /* 0xbfc */
    undefined4 b[32];               /* 0xc7c */
    char p1[0xd04 - 0xcfc];
} Car;                              /* 0xd04 */
MGLOBALA(Car, DAT_008815a0, 16, 0x008815a0);
// MATCH: 0x0046be10 FUN_0046be10
undefined4 FUN_0046be10(undefined4 *param_1,uint param_2,int param_3,undefined4 param_4)
{
  if (param_2 >= 0x10) {
    return 0;
  }
  DAT_008815a0[param_2].a[param_3] = *param_1;
  DAT_008815a0[param_2].b[param_3] = param_4;
  return 1;
}
