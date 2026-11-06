// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x0045a130 (h9, by hand): 100% (regs 100%). Raw converter 73.3%. Moved it: a descending
   do/while pointer walk over the 4 records of 0x58 bytes (0x0068bbac down to 0x0068ba4c, flag read at
   p[-0x11]); a descending for-index loop gave 73.1% (index form, not a pointer). Real function-pointer type
   for the RwGlobals (+0x20) slot. Residual: none. */
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
typedef void (*rwfn_t)(int, int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(char, DAT_0068b9b8, 4, 0x0068b9b8);
MGLOBALA(int, DAT_0068ba4c, 0x58, 0x0068ba4c);
void FUN_004770a0(void *p0);
int FUN_004e6680(int p0);
// MATCH: 0x0045a130 FUN_0045a130
void FUN_0045a130(void)
{
  int *p;
  (*(rwfn_t *)(DAT_007d3ff8 + 0x20))(0x14, 1);
  p = &DAT_0068ba4c[4 * 0x16];
  do {
    p -= 0x16;
    if (p[-0x11] != 0) {
      FUN_004e6680(*p);
    }
  } while (p != DAT_0068ba4c);
  FUN_004770a0(DAT_0068b9b8);
  (*(rwfn_t *)(DAT_007d3ff8 + 0x20))(0x14, 2);
}
