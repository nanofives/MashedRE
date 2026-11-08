// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004c3e20: 85.2% regs 100% (raw g2s form). Only diff: address operand order [eax+ecx+4] vs original [ecx+eax+4] (4 insns). Tried: ff8+ffc source order, global types unsigned, char pointer, int pointer, grouping of +4; type char* gives the order but reloads ff8 first (relocheck BAD). class (a)/(b). */
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
typedef unsigned int undefined4;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d3ffc, 0x007d3ffc);
MGLOBAL(int, DAT_007d4000, 0x007d4000);
// MATCH: 0x004c3e20 FUN_004c3e20

undefined4 FUN_004c3e20(undefined4 param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(DAT_007d3ffc + 4 + DAT_007d3ff8);
  if (iVar1 != 0) {
    (**(code **)(DAT_007d3ff8 + 0x10c))(iVar1);
    *(undefined4 *)(DAT_007d3ffc + 4 + DAT_007d3ff8) = 0;
  }
  if (*(int *)(DAT_007d3ffc + DAT_007d3ff8) != 0) {
    (**(code **)(DAT_007d3ff8 + 0x10c))(*(int *)(DAT_007d3ffc + DAT_007d3ff8));
    *(undefined4 *)(DAT_007d3ffc + DAT_007d3ff8) = 0;
  }
  DAT_007d4000 = DAT_007d4000 + -1;
  return param_1;
}
