// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h3 0x004e8940: hand-written. */
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
int FUN_004f3cb0(int p0, int p1);
int FUN_004f3be0(int p0, int p1);
// MATCH: 0x004e8940 FUN_004e8940
int FUN_004e8940(int param_1, int param_2, int param_3)
{
  int iVar1;

  if (param_3 != 0) {
    iVar1 = FUN_004f3cb0(param_1 + 0x20, param_3);
    if (iVar1 < 0) {
      iVar1 = FUN_004f3be0(param_1 + 0x20, param_3);
      if (iVar1 < 0) {
        return 0;
      }
    }
    *(short *)(param_2 + 6) = (short)iVar1;
    return param_1;
  }
  *(unsigned short *)(param_2 + 6) = 0xffff;
  return param_1;
}

