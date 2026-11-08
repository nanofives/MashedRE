// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005af180: 84.6% (regs 98.1%, ops 98.1%), all 52 instructions the same opcodes. Hand rewrite: while-loop over the circular list (not an if-guarded do), result in a stack local, hoisted load of *node. Residual: register choice for param_1/the loaded *node (edx/eax vs ecx/edx) and one load order. Tried statement orders, operand orders, int* param, permuter (116 compiles). Class (b). Callback typed cb_t, global via MGLOBALA(char array). cxx pending. */
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
typedef unsigned char undefined1;
typedef unsigned char byte;
typedef void (*cb_t)(int *p0, int p1);
MGLOBALA(undefined1, DAT_005e6ce0, 1, 0x005e6ce0);
int FUN_005adf30(byte * p0, byte * p1);
// MATCH: 0x005af180 FUN_005af180
int * FUN_005af180(int param_1,cb_t param_2,int param_3)
{
  int *piVar1;
  int *piVar5;
  int *piVar4;
  int *result;
  byte **pp;

  result = 0;
  piVar5 = *(int **)(param_1 + 0x20);
  while (piVar5 != (int *)(param_1 + 0x20)) {
    piVar4 = piVar5 - 0xb;
    piVar5 = (int *)*piVar5;
    do {
      piVar1 = (int *)piVar4[0xe];
      pp = *(byte ***)piVar4;
      if ((*(int **)(param_1 + 0x14) == piVar4) ||
          (FUN_005adf30(*pp,DAT_005e6ce0) == 0)) {
        result = piVar4;
        break;
      }
      if (param_2 != 0) {
        param_2(piVar4,param_3);
      }
      piVar4 = piVar1;
    } while (piVar1 != 0);
    if (param_2 == 0) break;
  }
  return result;
}
