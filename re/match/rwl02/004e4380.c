// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e4380: best 72.2% (regs 72.2%), residual class b. compiler tail-duplicates store+pop+ret into the skip path (orig joins and reloads param_1); tried 4 spellings and permuter, flat at 72.2. */
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

MGLOBAL(int, DAT_007d7170, 0x007d7170);
void FUN_004c0e50(int p0);
// MATCH: 0x004e4380 FUN_004e4380
void FUN_004e4380(undefined4 param_1,int param_2)
{
  undefined4 *puVar1;
  puVar1 = (undefined4 *)(DAT_007d7170 + param_2);
  if (*(int *)(param_2 + 4) != 0) {
    FUN_004c0e50(*(int *)(param_2 + 4));
  }
  *puVar1 = param_1;
}
