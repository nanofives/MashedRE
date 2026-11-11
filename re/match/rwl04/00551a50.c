// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00551a50 (h2): 96.4% (regs 98.2). Structured loops (do-while 2 outer, uVar3 >= count). Residue: orig reloads param into eax then lea edx,[eax+8], ours mov edx,[esp+14];add edx,8; tried &p[2], char* arithmetic, init order, permuter 134 compiles. class (b). */
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
typedef unsigned int uint;
undefined4 FUN_004c5770(undefined4 p0);
undefined4 FUN_004c57a0(void);
// MATCH: 0x00551a50 FUN_00551a50
int * FUN_00551a50(int *param_1)
{
  int *piVar1;
  int iVar2;
  uint uVar3;
  int *piVar4;
  uint uVar5;
  uint uVar6;
  int *piVar7;

  uVar5 = 0;
  piVar7 = param_1;
  do {
    uVar6 = 0;
    uVar3 = 0;
    piVar4 = param_1 + 2;
    do {
      piVar1 = (int *)*piVar4;
      if (piVar1 == (int *)0x0) {
        uVar6 = uVar6 + 1;
        piVar4 = piVar4 + 1;
      }
      else {
        if (uVar5 == *(uint *)(*(int *)(*piVar1 + 0x14) + 0x20 + uVar3 * 4)) {
          if (*piVar7 == 0) {
            iVar2 = FUN_004c57a0();
            *piVar7 = iVar2;
            if (iVar2 == 0) {
              return (int *)0x0;
            }
          }
          goto next;
        }
        uVar3 = uVar3 + 1;
        if (uVar3 >= (uint)piVar1[0xb]) {
          uVar3 = 0;
          uVar6 = uVar6 + 1;
          piVar4 = piVar4 + 1;
        }
      }
    } while (uVar6 < 8);
    if (*piVar7 != 0) {
      FUN_004c5770(*piVar7);
      *piVar7 = 0;
    }
next:
    uVar5 = uVar5 + 1;
    piVar7 = piVar7 + 1;
  } while (uVar5 < 2);
  return param_1;
}
