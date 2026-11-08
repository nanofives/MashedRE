// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004ccba0: 95.9% regs 100%.  class (a/b) Only 3 byte-access address operand orders differ ([edx+esi+8] vs [esi+edx+8], [esi+eax+8] vs [eax+esi+8]). Tried index/base source orders, array index, int casts. Shift is 0x80>>(n-8*(n>>3)) without the 0x1f mask.. */
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
typedef unsigned int uint;
typedef unsigned char byte;
typedef int code();
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
typedef int code1(int);
typedef int code1(int);
typedef int code1(int);
typedef int code1(int);
// MATCH: 0x004ccba0 FUN_004ccba0
uint * FUN_004ccba0(uint *param_1,uint param_2)
{
  uint *puVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;

  puVar1 = (uint *)param_1[4];
  uVar2 = param_1[2];
  while (1) {
    if (puVar1 == param_1 + 4) {
      return (uint *)0;
    }
    uVar3 = (int)puVar1 + uVar2 + 8;
    if (param_2 >= uVar3 && param_2 <= param_1[1] * *param_1 + uVar3) break;
    puVar1 = (uint *)*puVar1;
  }
  uVar3 = (param_2 - uVar3) / *param_1;
  uVar5 = uVar3 >> 3;
  ((unsigned char *)puVar1)[uVar5 + 8] &= ~(0x80 >> (uVar3 - (uVar5 << 3)));
  if ((param_1[6] & 2) != 0) {
    iVar4 = 0;
    uVar3 = 0;
    if (0 < uVar2) {
      do {
        iVar4 = iVar4 + *(unsigned char *)(uVar3 + 8 + (int)puVar1);
        uVar3 = uVar3 + 1;
      } while (uVar3 < uVar2);
      if (iVar4 != 0) {
        return param_1;
      }
    }
    *(uint *)puVar1[1] = *puVar1;
    *(uint *)(*puVar1 + 4) = puVar1[1];
    (**(code1 **)(DAT_007d3ff8 + 0x10c))((int)puVar1);
  }
  return param_1;
}
