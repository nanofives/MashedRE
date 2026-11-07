// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005ab710: by hand from the Ghidra structure (gotos kept, base pointer piVar4 = param_5 + 4, true arities: 00550950 4 args, 005509b0 3, 00550bc0 1, 005ab980 1). 44.9% (regs 62.6%, ops 74.2%, 196/198 ins). Residual (b): large function, structure only: original keeps param_2/param_3 in ecx/edi with a dec/dec switch on param_4 (1/2/5) where ours emits cmp chains; budget spent, no permuter. */
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
typedef void (*S25_cb5)(undefined4, int, int, int, undefined4);
__declspec(dllimport) unsigned long __stdcall WaitForSingleObject(void * hHandle, unsigned long dwMilliseconds);
__declspec(dllimport) int __stdcall ReleaseSemaphore(void * hSemaphore, long lReleaseCount, long * lpPreviousCount);
MGLOBAL(void *, DAT_007dd618, 0x007dd618);
MGLOBAL(int, DAT_007dd614, 0x007dd614);
MGLOBAL(int *, DAT_007dd608, 0x007dd608);
MGLOBAL(undefined4, DAT_007dd60c, 0x007dd60c);
MGLOBAL(uint, DAT_007dd610, 0x007dd610);
MGLOBAL(void *, DAT_007dd620, 0x007dd620);
MGLOBAL(int *, DAT_007dd624, 0x007dd624);
MGLOBAL(int *, DAT_007dd62c, 0x007dd62c);
MGLOBAL(void *, DAT_007dd61c, 0x007dd61c);
MGLOBAL(undefined1, DAT_007dce08, 0x007dce08);
void FUN_00550950(int p0, int p1, uint p2, int p3);
void FUN_005509b0(int p0, uint p1, int p2);
uint FUN_00550bc0(int p0);
int FUN_005ab980(int * p0);
// MATCH: 0x005ab710 FUN_005ab710
void FUN_005ab710(undefined4 param_1,int param_2,int param_3,int param_4,int param_5)
{
  int iVar1;
  uint uVar2;
  uint uVar3;
  int *piVar4;
  uint uVar5;
  piVar4 = (int *)(param_5 + 4);
  if (param_4 != 1) {
    if (param_4 == 2) {
      *(int *)(param_5 + 0x28) = param_2;
    }
    else if (param_4 == 5) goto LAB_005ab736;
    if (*(int *)(param_5 + 0xc) != 5) {
      (**(S25_cb5 *)(param_5 + 0x38))(param_1,param_2,param_3,param_4,*(undefined4 *)(param_5 + 0x3c));
      return;
    }
    *(undefined4 *)(param_5 + 0xc) = 3;
  }
LAB_005ab736:
  WaitForSingleObject(DAT_007dd618,0xffffffff);
  do {
    if (piVar4[2] == 4) {
      piVar4[2] = 3;
      if ((param_3 != 4) && (param_2 == piVar4[0xb])) {
        if ((undefined1 *)piVar4[0xc] == &DAT_007dce08) {
          DAT_007dd60c = param_1;
          DAT_007dd610 = piVar4[10];
          DAT_007dd608 = piVar4;
          DAT_007dd614 = param_2;
        }
        else {
          piVar4[6] = piVar4[6] + param_2;
          piVar4[3] = piVar4[3] + param_2;
          piVar4[5] = piVar4[5] - param_2;
        }
        goto LAB_005ab7d2;
      }
    }
    else {
LAB_005ab7d2:
      uVar3 = piVar4[3];
      uVar5 = uVar3 & 0xfffff800;
      if ((DAT_007dd608 == piVar4) && (DAT_007dd610 == uVar5)) {
        iVar1 = FUN_005ab980(piVar4);
        uVar3 = uVar3 + iVar1;
        piVar4[3] = piVar4[3] + iVar1;
        uVar5 = uVar3 & 0xfffff800;
        param_3 = 2;
      }
      if (piVar4[5] != 0) {
        if ((((piVar4[6] & 3U) == 0) && (uVar3 == uVar5)) && (0x7ff < (uint)piVar4[5])) {
          uVar3 = piVar4[5] & 0xfffff800;
          piVar4[0xc] = piVar4[6];
          if (piVar4 == DAT_007dd608) {
            DAT_007dd608 = (int *)0x0;
            DAT_007dd60c = 0;
          }
        }
        else {
          DAT_007dd608 = (int *)0x0;
          DAT_007dd60c = 0;
          DAT_007dd610 = 0xffffffff;
          uVar3 = 0x800;
          piVar4[0xc] = (int)&DAT_007dce08;
        }
        uVar2 = FUN_00550bc0(piVar4[7]);
        if (uVar2 == uVar5) {
          piVar4[2] = 4;
          piVar4[10] = uVar5;
          piVar4[0xb] = uVar3;
          FUN_00550950(piVar4[0xc],1,uVar3,piVar4[7]);
        }
        else {
          piVar4[2] = 5;
          FUN_005509b0(piVar4[7],uVar5,0);
        }
        break;
      }
    }
    piVar4[2] = 1;
    (*(S25_cb5)piVar4[0xd])(piVar4[7],piVar4[4],param_3,param_4,piVar4[0xe]);
    piVar4 = (int *)0x0;
    WaitForSingleObject(DAT_007dd620,0xffffffff);
    if ((int **)DAT_007dd624 == (int **)&DAT_007dd624) {
      ReleaseSemaphore(DAT_007dd61c,1,(long *)0);
    }
    else {
      piVar4 = DAT_007dd624;
      if ((DAT_007dd608 != (int *)0x0) && (*DAT_007dd608 != 0)) {
        piVar4 = DAT_007dd608;
      }
      *(int *)piVar4[1] = *piVar4;
      *(int *)(*piVar4 + 4) = piVar4[1];
      *piVar4 = 0;
      piVar4[2] = 3;
    }
    ReleaseSemaphore(DAT_007dd620,1,(long *)0);
    DAT_007dd62c = piVar4;
  } while (piVar4 != (int *)0x0);
  ReleaseSemaphore(DAT_007dd618,1,(long *)0);
  return;
}
