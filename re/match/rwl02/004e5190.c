// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h3 0x004e5190: hand-written. */
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ae0, 0x00911ae0);
int FUN_004d40d0(int p0, unsigned int p1, int p2);
// MATCH: 0x004e5190 FUN_004e5190
unsigned int FUN_004e5190(unsigned int param_1)
{
  int iVar1;

  if (*(unsigned short *)(param_1 + 0x84) <= 0) {
    return param_1;
  }
  iVar1 = *(int *)(param_1 + 0x7c);
  if (iVar1 == 0) {
    iVar1 = *(int *)(*(int *)(DAT_007d3ff8 + 4) + 0x6c);
    if (iVar1 == 0) {
      iVar1 = *(int *)(DAT_00911ae0 + 0x40 + DAT_007d3ff8);
    }
  }
  iVar1 = FUN_004d40d0(iVar1, param_1, 1);
  return iVar1 != 0 ? param_1 : 0;
}

