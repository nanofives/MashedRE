// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c6ba0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs19_20261102/results.tsv. */
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
typedef unsigned char byte;
typedef int code();
#define CONCAT11(a,b) ((ushort)(((uint)(uchar)(a)<<8)|(uchar)(b)))
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4054, 0x007d4054);
MGLOBAL(char, DAT_007d3e8b, 0x007d3e8b);
MGLOBAL(undefined4, DAT_00618138, 0x00618138);
MGLOBAL(undefined1, DAT_007d3e6c, 0x007d3e6c);
undefined4 FUN_004c6580(int * p0, int p1, int * p2, int p3, undefined4 p4);
int FUN_004c6750(int p0, char * p1, undefined4 p2, int * p3, int * p4, undefined4 p5, undefined4 p6);
undefined4 FUN_004c7600(undefined4 p0);
undefined4 FUN_004c7650(undefined4 p0);
int FUN_004c76f0(int p0);
int FUN_004c77c0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
uint FUN_004c7860(undefined4 p0, uint p1, int p2);
undefined4 FUN_004cdd00(uint * p0);
uint * FUN_004cefd0(uint * p0);
int FUN_004d5310(int p0, byte * p1);
undefined4 FUN_004d7ff0(undefined4 p0);
int FUN_004d8000(int p0, undefined4 p1);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004c6ba0 FUN_004c6ba0
int * FUN_004c6ba0(int param_1,char *param_2)

{
  code *pcVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int *piVar7;
  undefined4 uVar8;
  int iStack_668;
  undefined4 uStack_664;
  undefined4 uStack_660;
  undefined4 uStack_65c;
  undefined4 uStack_658;
  uint uStack_654;
  undefined4 uStack_650;
  undefined4 uStack_64c;
  undefined4 uStack_648;
  undefined4 uStack_644;
  int aiStack_640 [16];
  undefined1 auStack_600 [255];
  undefined1 uStack_501;
  undefined1 local_500 [255];
  undefined1 uStack_401;
  undefined1 auStack_400 [1024];
  
  (**(code **)(DAT_007d3ff8 + 0xd0))(local_500,param_1,0x100);
  uVar2 = (**(code **)(DAT_007d3ff8 + 0xf4))(param_1);
  if (0xff < uVar2) {
    iStack_668 = 1;
    uStack_664 = FUN_004d7ff0(0x8000001e,param_1,0x100,0xff,(int)*(char *)(param_1 + 0xff));
    FUN_004d8480(&iStack_668);
    uStack_401 = 0;
  }
  auStack_600[0] = 0;
  if ((param_2 != (char *)0x0) && (*param_2 != '\0')) {
    (**(code **)(DAT_007d3ff8 + 0xd0))(auStack_600,param_2,0x100);
    uVar2 = (**(code **)(DAT_007d3ff8 + 0xf4))(param_2);
    if (0xff < uVar2) {
      iStack_668 = 1;
      uStack_664 = FUN_004d7ff0(0x8000001e,param_2,0x100,0xff,(int)param_2[0xff]);
      FUN_004d8480(&iStack_668);
      uStack_501 = 0;
    }
  }
  uVar8 = 4;
  if ((*(int *)(DAT_007d3ff8 + DAT_007d4054 + 0x1c) != 0) &&
     (uVar8 = 0x8004, *(int *)(DAT_007d3ff8 + DAT_007d4054 + 0x20) != 0)) {
    uVar8 = 0x9004;
  }
  pcVar1 = *(code **)(DAT_007d3ff8 + DAT_007d4054 + 0x30);
  if (pcVar1 != (code *)0x0) {
    (*pcVar1)(local_500,auStack_600,0,uVar8);
  }
  uStack_650 = 0;
  uStack_658 = 0;
  iVar3 = FUN_004c6750(local_500,auStack_600,uVar8,&uStack_650,&uStack_658,&uStack_64c,&uStack_654);
  if (iVar3 == 0) {
    return (int *)0x0;
  }
  aiStack_640[0] = iVar3;
  iVar4 = FUN_004c77c0(uStack_650,uStack_658,uStack_64c,uStack_654);
  if (iVar4 == 0) {
    FUN_004cdd00(iVar3);
    return (int *)0x0;
  }
  if ((char)(uStack_654 >> 8) < '\0') {
    if ((uStack_654 & 0x1000) != 0) {
      iVar5 = FUN_004d5310(iVar4,iVar3);
      goto LAB_004c7060;
    }
    iStack_668 = FUN_004c76f0(iVar4);
    iVar3 = 1;
    if (1 < iStack_668) {
      do {
        (**(code **)(DAT_007d3ff8 + 0xd0))(local_500,param_1,0x100);
        uVar2 = (**(code **)(DAT_007d3ff8 + 0xf4))(param_1);
        if (0xff < uVar2) {
          uStack_648 = 1;
          uStack_644 = FUN_004d7ff0(0x8000001e,param_1,0x100,0xff,(int)*(char *)(param_1 + 0xff));
          FUN_004d8480(&uStack_648);
          uStack_401 = 0;
        }
        auStack_600[0] = 0;
        if ((param_2 != (char *)0x0) && (*param_2 != '\0')) {
          (**(code **)(DAT_007d3ff8 + 0xd0))(auStack_600,param_2,0x100);
          uVar2 = (**(code **)(DAT_007d3ff8 + 0xf4))(param_2);
          if (0xff < uVar2) {
            uStack_660 = 1;
            uStack_65c = FUN_004d7ff0(0x8000001e,param_2,0x100,0xff,(int)param_2[0xff]);
            FUN_004d8480(&uStack_660);
            uStack_501 = 0;
          }
        }
        pcVar1 = *(code **)(DAT_007d3ff8 + DAT_007d4054 + 0x30);
        if (pcVar1 != (code *)0x0) {
          (*pcVar1)(local_500,auStack_600,iVar3,uVar8);
        }
        FUN_004c7860(iVar4,iVar3,5);
        uStack_650 = *(undefined4 *)(iVar4 + 0xc);
        uStack_658 = *(undefined4 *)(iVar4 + 0x10);
        uStack_64c = *(undefined4 *)(iVar4 + 0x14);
        uStack_654 = (uint)CONCAT11(*(undefined1 *)(iVar4 + 0x23),*(undefined1 *)(iVar4 + 0x20));
        FUN_004c7600(iVar4);
        iVar5 = FUN_004c6750(local_500,auStack_600,uVar8,&uStack_650,&uStack_658,&uStack_64c,
                             &uStack_654);
        aiStack_640[iVar3] = iVar5;
        if (iVar5 == 0) {
          while (iVar3 = iVar3 + -1, -1 < iVar3) {
            FUN_004cdd00(aiStack_640[iVar3]);
          }
          FUN_004c7650(iVar4);
          return (int *)0x0;
        }
        iVar3 = iVar3 + 1;
      } while (iVar3 < iStack_668);
    }
    if ((*(byte *)(iVar4 + 0x23) & 0x60) == 0) {
      iVar3 = 0;
      if (0 < iStack_668) {
        do {
          FUN_004cefd0(aiStack_640[iVar3]);
          iVar3 = iVar3 + 1;
        } while (iVar3 < iStack_668);
      }
    }
    else {
      if ((*(byte *)(iVar4 + 0x23) & 0x40) == 0) {
        uVar8 = 8;
      }
      else {
        uVar8 = 4;
      }
      FUN_004c6580(auStack_400,0,aiStack_640,iStack_668,uVar8);
      FUN_004cefd0(aiStack_640[0]);
    }
    iVar3 = 0;
    if (0 < iStack_668) {
      do {
        iVar5 = FUN_004c7860(iVar4,iVar3,5);
        if (iVar5 != 0) {
          iVar6 = FUN_004d5310(iVar4,aiStack_640[iVar3]);
          iVar5 = iStack_668;
          if (iVar6 == 0) goto joined_r0x004c7026;
          FUN_004c7600(iVar4);
        }
        FUN_004cdd00(aiStack_640[iVar3]);
        iVar3 = iVar3 + 1;
      } while (iVar3 < iStack_668);
    }
  }
  else {
    FUN_004cefd0(iVar3);
    iVar5 = FUN_004d5310(iVar4,iVar3);
LAB_004c7060:
    if (iVar5 == 0) {
      FUN_004c7650(iVar4);
      FUN_004cdd00(iVar3);
      return (int *)0x0;
    }
    FUN_004cdd00(iVar3);
  }
  piVar7 = (int *)(**(code **)(DAT_007d3ff8 + 0x118))
                            (*(undefined4 *)(DAT_007d3ff8 + DAT_007d4054 + 8),0x30006);
  if (piVar7 != (int *)0x0) {
    piVar7[0x14] = 0;
    *(undefined1 *)((int)piVar7 + 0x51) = 0x11;
    piVar7[1] = 0;
    *(undefined1 *)(piVar7 + 4) = 0;
    *(undefined1 *)(piVar7 + 0xc) = 0;
    *piVar7 = iVar4;
    piVar7[0x15] = 1;
    *(undefined1 *)(piVar7 + 0x14) = 1;
    FUN_004d8000(&DAT_00618138,piVar7);
    (**(code **)(DAT_007d3ff8 + 0xd0))(piVar7 + 4,param_1,0x20);
    uVar2 = (**(code **)(DAT_007d3ff8 + 0xf4))(param_1);
    if (0x1f < uVar2) {
      uStack_660 = 1;
      uStack_65c = FUN_004d7ff0(0x8000001e,param_1,0x20,0x1f,(int)*(char *)(param_1 + 0x1f));
      FUN_004d8480(&uStack_660);
      *(undefined1 *)((int)piVar7 + 0x2f) = 0;
    }
    if (param_2 == (char *)0x0) {
      (**(code **)(DAT_007d3ff8 + 0xd0))(piVar7 + 0xc,&DAT_007d3e6c,0x20);
      uVar2 = (**(code **)(DAT_007d3ff8 + 0xf4))(&DAT_007d3e6c);
      if (uVar2 < 0x20) {
        return piVar7;
      }
      uStack_660 = 1;
      uStack_65c = FUN_004d7ff0(0x8000001e,&DAT_007d3e6c,0x20,0x1f,(int)DAT_007d3e8b);
    }
    else {
      (**(code **)(DAT_007d3ff8 + 0xd0))(piVar7 + 0xc,param_2);
      uVar2 = (**(code **)(DAT_007d3ff8 + 0xf4))(param_2);
      if (uVar2 < 0x20) {
        return piVar7;
      }
      uStack_660 = 1;
      uStack_65c = FUN_004d7ff0(0x8000001e,param_2,0x20,0x1f,(int)param_2[0x1f]);
    }
    FUN_004d8480(&uStack_660);
    *(undefined1 *)((int)piVar7 + 0x4f) = 0;
    return piVar7;
  }
LAB_004c703a:
  FUN_004c7650(iVar4);
  return (int *)0x0;
joined_r0x004c7026:
  for (; iVar3 < iVar5; iVar3 = iVar3 + 1) {
    FUN_004cdd00(aiStack_640[iVar3]);
  }
  goto LAB_004c703a;
}
