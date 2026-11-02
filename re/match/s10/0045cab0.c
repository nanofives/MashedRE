// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045cab0 (h2): 100% (regs 100%). Raw 96.3% (bool return gave xor al,al): int return, callees
   declared cdecl int with their real arity (0x0046cbb0 3 args, 0x0046c7b0 1 arg). */
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
int FUN_0046c7b0(int param_1);
int FUN_0046cbb0(int param_1, int *param_2, int *param_3);
// MATCH: 0x0045cab0 FUN_0045cab0
int FUN_0045cab0(int param_1)
{
  int local_8;
  int local_4;

  FUN_0046cbb0(param_1, &local_8, &local_4);
  if (local_8 != 0) {
    return 0;
  }
  return FUN_0046c7b0(param_1) == 1;
}
