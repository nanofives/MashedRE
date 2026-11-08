// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 h1 0x00575560: 31.2% (regs 63.0%), class b. cmp ax 16-bit and loop reg alloc, 216 B-class int function; regs-renamed 59% (structure differs in locals/spill). Tried: ushort 0xffff compare, corrected callee prototypes, permuter 36.1 not adopted (moved a statement). cxx:ok. */
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
int FUN_005752b0(int p0, int p1, int p2);
uint FUN_00578ff0(int p0, uint p1, int p2, int p3, int p4, int p5);
// MATCH: 0x00575560 FUN_00575560
int FUN_00575560(int param_1,uint param_2,uint param_3,int param_4,int param_5)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  int local_8;
  uint local_4;
  
  iVar4 = param_2;
  iVar3 = param_1;
  uVar6 = 0;
  if (param_3 != 0) {
    if (*(ushort *)(param_2 + 0x5c) == 0xffff) {
      local_4 = 0;
    }
    else {
      local_4 = *(uint *)(*(int *)(param_2 + 0x58) + 8);
    }
    if (*(ushort *)(param_2 + 0xdc) == 0xffff) {
      param_2 = 0;
    }
    else {
      param_2 = *(uint *)(*(int *)(param_2 + 0xd8) + 8);
    }
    if (((local_4 & 0x20) == 0) && ((param_2 & 0x20) == 0)) {
      local_8 = param_4 + 0xe0;
      uVar7 = 0;
      if (param_3 != 0) {
        param_1 = iVar4;
        do {
          if (param_5 - 1U <= uVar7) break;
          iVar5 = FUN_005752b0(param_1,*(int *)(iVar3 + 0x70),local_8);
          if (iVar5 != 0) {
            uVar7 = uVar7 + 1;
            local_8 = local_8 + 0xe0;
          }
          uVar6 = uVar6 + 1;
          param_1 = param_1 + 0x120;
        } while (uVar6 < param_3);
      }
      param_3 = 0;
      if (uVar7 != 0) {
        param_3 = FUN_00578ff0(param_4 + 0xe0,uVar7,*(int *)(iVar3 + 0x7c),param_4,param_5,
                               *(int *)(iVar3 + 0x74));
      }
      if ((((param_2 | local_4) & 0x40) != 0) && (param_3 != 0)) {
        iVar5 = param_4;
        param_4 = param_3;
        do {
          if ((local_4 & 0x40) != 0) {
            uVar6 = *(uint *)(iVar3 + 0x68);
            iVar2 = *(int *)(iVar4 + 0x58);
            if (uVar6 < *(uint *)(iVar3 + 0x6c)) {
              piVar1 = (int *)(*(int *)(iVar3 + 100) + uVar6 * 0x14);
              *(uint *)(iVar3 + 0x68) = uVar6 + 1;
              *piVar1 = iVar2;
              piVar1[1] = *(int *)(iVar4 + 0xd8);
              piVar1[2] = *(int *)(iVar4 + 0xdc);
              piVar1[3] = iVar5;
              piVar1[4] = *(int *)(iVar2 + 0x2c);
              *(int **)(iVar2 + 0x2c) = piVar1;
            }
          }
          if ((param_2 & 0x40) != 0) {
            uVar6 = *(uint *)(iVar3 + 0x68);
            iVar2 = *(int *)(iVar4 + 0xd8);
            if (uVar6 < *(uint *)(iVar3 + 0x6c)) {
              piVar1 = (int *)(*(int *)(iVar3 + 100) + uVar6 * 0x14);
              *(uint *)(iVar3 + 0x68) = uVar6 + 1;
              *piVar1 = iVar2;
              piVar1[1] = *(int *)(iVar4 + 0x58);
              piVar1[2] = *(int *)(iVar4 + 0x5c);
              piVar1[3] = iVar5;
              piVar1[4] = *(int *)(iVar2 + 0x2c);
              *(int **)(iVar2 + 0x2c) = piVar1;
            }
          }
          iVar5 = iVar5 + 0xe0;
          param_4 = param_4 + -1;
        } while (param_4 != 0);
      }
      return param_3;
    }
    if ((local_4 & 0x40) != 0) {
      uVar6 = *(uint *)(param_1 + 0x68);
      iVar3 = *(int *)(iVar4 + 0x58);
      if (uVar6 < *(uint *)(param_1 + 0x6c)) {
        piVar1 = (int *)(*(int *)(param_1 + 100) + uVar6 * 0x14);
        *(uint *)(param_1 + 0x68) = uVar6 + 1;
        *piVar1 = iVar3;
        piVar1[1] = *(int *)(iVar4 + 0xd8);
        piVar1[2] = *(int *)(iVar4 + 0xdc);
        piVar1[3] = 0;
        piVar1[4] = *(int *)(iVar3 + 0x2c);
        *(int **)(iVar3 + 0x2c) = piVar1;
      }
    }
    if ((param_2 & 0x40) != 0) {
      uVar6 = *(uint *)(param_1 + 0x68);
      iVar3 = *(int *)(iVar4 + 0xd8);
      if (uVar6 < *(uint *)(param_1 + 0x6c)) {
        piVar1 = (int *)(*(int *)(param_1 + 100) + uVar6 * 0x14);
        *(uint *)(param_1 + 0x68) = uVar6 + 1;
        *piVar1 = iVar3;
        piVar1[1] = *(int *)(iVar4 + 0x58);
        piVar1[2] = *(int *)(iVar4 + 0x5c);
        piVar1[3] = 0;
        piVar1[4] = *(int *)(iVar3 + 0x2c);
        *(int **)(iVar3 + 0x2c) = piVar1;
      }
    }
  }
  return 0;
}
