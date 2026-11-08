// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004e1b60: best 86.5% (regs 90%, ops 98.2%), class (b). Unsigned count tested as 0 < n, param_3 copied after the range check, dead store dropped; remaining diff is which of eax/ecx/edx and esi/edx carry the first call's args (permuter 239 compiles). */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#ifndef __cplusplus
#define true 1
#endif
undefined4 * FUN_004cc050(int p0, int p1);
undefined4 FUN_004cc400(undefined4 p0, int * p1, int * p2, uint * p3, uint * p4);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, undefined4 * p2, uint * p3);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004e1b60 FUN_004e1b60
/* [C1 2026-05-07] FUN_004e1b60 (RW plugin extension stream reader) */

int FUN_004e1b60(int param_1,int param_2,int param_3)

{
  undefined4 *puVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint local_10;
  uint local_c [3];
  
  iVar3 = FUN_004cc5e0(param_2,3,&local_10,local_c);
  iVar4 = param_2;
  if (iVar3 != 0) {
    if ((local_c[0] < 0x35000) || (0x37002 < local_c[0])) {
      local_c[2] = FUN_004d7ff0(0x80000004);
      FUN_004d8480(local_c + 1);
      local_c[1] = 1;
    }
    else {
      iVar2 = param_3;
      for (; 0 < local_10; local_10 = local_10 + (-0xc - param_2)) {
        iVar3 = FUN_004cc400(iVar4,&param_3,&param_2,0,0);
        if (iVar3 == 0) {
          return 0;
        }
        for (puVar1 = *(undefined4 **)(param_1 + 0x10); puVar1 != (undefined4 *)0x0;
            puVar1 = (undefined4 *)puVar1[0xc]) {
          if (puVar1[2] == param_3) {
            if ((code *)puVar1[3] != (code *)0x0) {
              iVar3 = (*(code *)puVar1[3])(iVar4,param_2,iVar2,*puVar1,puVar1[1]);
              goto LAB_004e1bf5;
            }
            break;
          }
        }
        iVar3 = (int)FUN_004cc050(iVar4,param_2);
LAB_004e1bf5:
        if (iVar3 == 0) {
          return 0;
        }
      }
      puVar1 = *(undefined4 **)(param_1 + 0x10);
      while( true ) {
        if (puVar1 == (undefined4 *)0x0) {
          return param_1;
        }
        if (((code *)puVar1[6] != (code *)0x0) &&
           (iVar4 = (*(code *)puVar1[6])(iVar2,*puVar1,puVar1[1]), iVar4 == 0)) break;
        puVar1 = (undefined4 *)puVar1[0xc];
      }
    }
  }
  return 0;
}
