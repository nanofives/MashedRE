// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045df70 (h3): 100% (regs 100%), byte-identical. Raw converter did not compile. Two
   independent compares against param_1 with the 0.025f step (@0x005cc9a4). Residual: none. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
MGLOBAL(float, DAT_006036c0, 0x006036c0);
// MATCH: 0x0045df70 FUN_0045df70
void FUN_0045df70(float param_1)
{
  if (DAT_006036c0 < param_1) {
    DAT_006036c0 = DAT_006036c0 + 0.025f;
  }
  if (DAT_006036c0 > param_1) {
    DAT_006036c0 = DAT_006036c0 - 0.025f;
  }
}
