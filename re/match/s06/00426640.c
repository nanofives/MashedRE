// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00426640: 100% by hand (h4), regs 100%. Ghidra gave FUN_0041e950 no argument (pilot finding 3):
   the original stores &DAT_00646e58 into the arg slot and tail-jumps, so the callee takes one pointer argument.
   relocheck OK. Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0066d704, 0x0066d704);
MGLOBAL(undefined1, DAT_00646e58, 0x00646e58);
void FUN_0041e950(void *p0);
int FUN_0041e9b0(int p0);
int FUN_0041ea70(void);
// MATCH: 0x00426640 FUN_00426640
void FUN_00426640(int param_1)
{
  if (DAT_0066d704 != 0) {
    FUN_0041e9b0(param_1);
    if (FUN_0041ea70() != 0) {
      FUN_0041e950(&DAT_00646e58);
    }
  }
}
