// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004d7de0: 61.5% regs 69.2%.  class (b) Fixed to compile: FUN_004d7ff0 (returns its first arg) and FUN_004cca80 are functions not globals. Original puts the 0x80000017 error path (1 + FUN_004d7ff0 + FUN_004d8480) as an inline early return and uses sub esp,8; ours differs in layout. Not reached in budget.. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
int FUN_004cca80(int p0, int p1);
MGLOBAL(int, DAT_007d6c50, 0x007d6c50);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(uint, DAT_007d6c58, 0x007d6c58);
MGLOBAL(int, DAT_007d6c54, 0x007d6c54);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 FUN_004c2d70(void);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004d7de0 FUN_004d7de0
int FUN_004d7de0(int *param_1,int param_2,int param_3,code *param_4,code *param_5,code *param_6)

{
  int iVar1;
  uint uVar2;
  int *piVar3;
  undefined4 local_8;
  undefined4 local_4;
  
  if (DAT_007d6c50 != 0) {
    iVar1 = FUN_004c2d70();
    if (iVar1 == 0) {
      if (*(code **)(DAT_007d3ff8 + 0x118) != (code *)FUN_004cca80) {
        uVar2 = 0;
        if (DAT_007d6c58 != 0) {
          do {
            if (param_1 == *(int **)(DAT_007d6c54 + uVar2 * 4)) break;
            uVar2 = uVar2 + 1;
          } while (uVar2 < DAT_007d6c58);
        }
        if (DAT_007d6c58 == uVar2) {
          iVar1 = (**(code **)(DAT_007d3ff8 + 0x108))(DAT_007d6c58 * 4 + 4,0x40000);
          uVar2 = 0;
          if (DAT_007d6c54 != 0) {
            if (DAT_007d6c58 != 0) {
              do {
                uVar2 = uVar2 + 1;
                *(undefined4 *)(iVar1 + -4 + uVar2 * 4) =
                     *(undefined4 *)(DAT_007d6c54 + -4 + uVar2 * 4);
              } while (uVar2 < DAT_007d6c58);
            }
            (**(code **)(DAT_007d3ff8 + 0x10c))(DAT_007d6c54);
          }
          *(int **)(iVar1 + uVar2 * 4) = param_1;
          DAT_007d6c58 = DAT_007d6c58 + 1;
          DAT_007d6c54 = iVar1;
        }
      }
      for (piVar3 = (int *)param_1[4]; piVar3 != (int *)0x0; piVar3 = (int *)piVar3[0xc]) {
        if (piVar3[2] == param_3) {
          local_8 = 1;
          local_4 = FUN_004d7ff0(0x80000017);
          FUN_004d8480(&local_8);
          return *piVar3;
        }
      }
      iVar1 = (param_2 + 3U & 0xfffffffc) + *param_1;
      if (((param_1[2] == 0) || (iVar1 <= param_1[2])) &&
         (piVar3 = (int *)(**(code **)(DAT_007d3ff8 + 0x118))(DAT_007d6c50,0x40000),
         piVar3 != (int *)0x0)) {
        *piVar3 = *param_1;
        *param_1 = iVar1;
        piVar3[1] = param_2;
        piVar3[2] = param_3;
        piVar3[3] = 0;
        piVar3[4] = 0;
        piVar3[5] = 0;
        piVar3[6] = 0;
        piVar3[7] = 0;
        if (param_4 == (code *)0x0) {
          param_4 = (code *)FUN_004d7ff0;
        }
        piVar3[8] = (int)param_4;
        if (param_5 == (code *)0x0) {
          param_5 = (code *)FUN_004d7ff0;
        }
        piVar3[9] = (int)param_5;
        if (param_6 == (code *)0x0) {
          param_6 = (code *)FUN_004d7ff0;
        }
        piVar3[10] = (int)param_6;
        piVar3[0xb] = 0;
        piVar3[0xc] = 0;
        piVar3[0xd] = 0;
        piVar3[0xe] = (int)param_1;
        if (param_1[4] == 0) {
          param_1[4] = (int)piVar3;
          param_1[5] = (int)piVar3;
          return *piVar3;
        }
        *(int **)(param_1[5] + 0x30) = piVar3;
        piVar3[0xd] = param_1[5];
        param_1[5] = (int)piVar3;
        return *piVar3;
      }
    }
    else {
      local_8 = 1;
      local_4 = FUN_004d7ff0(0x80000017);
      FUN_004d8480(&local_8);
    }
  }
  return -1;
}
