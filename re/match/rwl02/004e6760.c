// FLAGS: /O2 /Ob1 /G5
// h2 hand-matched. 54.8% (regs 54.8%, ops 71.0%). Raw Ghidra do-while form scores best; struct/while/for forms (41-45%) and a 200 s permuter run were worse. Residual: original evaluates end = clump+0x10 after loading the first link, keeps cb/data in ebx/edi and end in ebp, tests the first link before loading args. Class (b).
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

#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d7230, 0x007d7230);
// MATCH: 0x004e6760 RpClumpForAllLights
int RpClumpForAllLights(int param_1, code *param_2, undefined4 param_3)
{
  undefined4 *puVar1;
  undefined4 *puVar2;
  int iVar3;

  puVar2 = *(undefined4 **)(param_1 + 0x10);
  do {
    if (puVar2 == (undefined4 *)(param_1 + 0x10)) {
      return param_1;
    }
    puVar1 = (undefined4 *)*puVar2;
    iVar3 = (*param_2)((int)puVar2 + (-4 - DAT_007d7230), param_3);
    puVar2 = puVar1;
  } while (iVar3 != 0);
  return param_1;
}
