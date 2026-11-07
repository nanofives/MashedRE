// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005b3a00: 84.7% (regs ~86%). Struct copies of 7 ints (S7 assignment gives rep movsd; 53.7 -> 79.7%), swapped if/else on the +8 bit-0 test (-> 83.9%), uVar5 = 0 after the copy (permuter, -> 84.7%). Residual: original pushes ebp/edi before the first call (no shrink-wrap) and a jbe loop guard. Class: a.
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
typedef unsigned char byte;
typedef int (code)(void *, void *, int, void *, void *, void *);
typedef struct S7 { int a[7]; } S7;
undefined4 FUN_005ac5f0(int * p0, int * p1);
void FUN_005ac980(undefined4 p0, int *p1, int p2);
undefined4 FUN_005b36f0(int *p0, int p1);
uint FUN_005b3760(int p0, int p1, int p2, int p3, int * p4, int * p5, uint p6, uint p7, uint p8);
// MATCH: 0x005b3a00 FUN_005b3a00
int FUN_005b3a00(int *param_1,undefined4 param_2,int *param_3,int *param_4,undefined4 *param_5)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  int *piVar6;
  int *piVar7;
  int local_48 [4];
  int local_38 [7];
  int local_1c [7];
  
  piVar7 = param_3;
  piVar6 = param_1;
  iVar1 = FUN_005ac5f0(param_1,param_3);
  if (iVar1 != 0) {
    if (param_5 != (undefined4 *)0x0) {
      *param_5 = (undefined4)param_4;
    }
    return piVar6[2];
  }
  uVar2 = FUN_005b36f0(piVar6,1);
  uVar3 = FUN_005b36f0(piVar7,0);
  uVar4 = FUN_005b3760(uVar2,*piVar6,uVar3,*piVar7,local_48,local_38,4,0,0xffffffff);
  *(S7 *)local_1c = *(S7 *)piVar6;
  uVar5 = 0;
  if (uVar4 != 0) {
    do {
      FUN_005ac980(*(undefined4 *)(*(int *)local_48[uVar5] + 4),local_38,1);
      if ((*(byte *)(*(int *)local_48[uVar5] + 8) & 1) != 0) {
        local_38[0] = *param_3;
      }
      else {
        local_38[0] = local_1c[0];
      }
      local_38[2] = (*(code *)((int *)local_48[uVar5])[1])
                              (local_1c,(void *)param_2,(int)param_4,local_38,0,&param_1);
      if (local_38[2] == 0) {
        return 0;
      }
      uVar5 = uVar5 + 1;
      param_4 = param_1;
      *(S7 *)local_1c = *(S7 *)local_38;
    } while (uVar5 < uVar4);
  }
  if (param_5 != (undefined4 *)0x0) {
    *param_5 = (undefined4)param_1;
  }
  return local_38[2];
}
