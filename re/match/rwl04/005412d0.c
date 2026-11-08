// FLAGS: /O2 /Ob1 /G5
/* h2 note: 44.2% (regs 53.9). Fixed compile errors of raw (param_2 float->int, dead param_2=fVar4 removed, ROUND->(uint)cast, FUN_00541b50 proto int). cxx FAIL: FUN_00540340 arg4 int* vs undefined4, code() arity. class (b). */
/* MATCH-RWL04 0x005412d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007dc5dc, 0x007dc5dc);
MGLOBAL(undefined1, DAT_005cd04c, 0x005cd04c);
MGLOBAL(int, DAT_006181d0, 0x006181d0);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(int, DAT_007dc600, 0x007dc600);
MGLOBAL(int, DAT_006181d4, 0x006181d4);
MGLOBAL(int, DAT_007dc604, 0x007dc604);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00623d34, 0x00623d34);
MGLOBAL(int, DAT_007dc620, 0x007dc620);
MGLOBAL(int, DAT_007dc618, 0x007dc618);
int FUN_004cfee0(int * p0);
void FUN_004d3b00(undefined4 * p0);
void FUN_004d53b0(void);
void FUN_004d5480(int p0, int p1);
void FUN_004d54d0(int p0, undefined4 * p1);
void FUN_004d54f0(int p0, int p1, int p2);
undefined4 FUN_004d6c40(int p0);
undefined4 FUN_004d6c90(int p0);
undefined4 FUN_004d6e70(undefined4 * p0, int p1);
void FUN_004d7100(int p0);
undefined4 FUN_004d71f0(void);
void FUN_00540340(int p0, int p1, byte p2, undefined4 p3);
undefined4 FUN_00541b50(int p0, int p1, int p2);
MGLOBAL(int, _DAT_005cd04c, 0x005cd04c);
// MATCH: 0x005412d0 FUN_005412d0
void FUN_005412d0(int param_1,int param_2,uint param_3,uint param_4,int *param_5,int *param_6)

{
  int *piVar1;
  undefined4 *puVar2;
  uint uVar3;
  float fVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined4 unaff_EBX;
  undefined4 unaff_EBP;
  uint uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  int iStack_10;
  int iStack_c;
  int iStack_8;
  int iStack_4;
  
  iVar7 = param_4;
  iVar5 = param_2;
  iVar6 = *(int *)(DAT_007dc5dc + *(int *)(param_2 + 8));
  fVar4 = *(float *)(iVar6 + 8 + param_4 * 0x18) * _DAT_005cd04c;
  puVar2 = (undefined4 *)(iVar6 + param_4 * 0x18);
  uVar3 = (uint)fVar4;
  uVar8 = uVar3 & 0xff;
  if (((uVar8 == 0) || (param_6 == (int *)0x0)) || (*param_6 == 0)) {
    if (param_4 == 0) {
      param_4 = uVar3;
      FUN_00540340(param_1,iVar5,param_3,param_5);
    }
    return;
  }
  piVar1 = (int *)(param_2 + 0x10);
  param_4 = 0;
  param_2 = 0;
  if (DAT_006181d0 != *piVar1) {
    DAT_006181d0 = *piVar1;
    (**(code **)(*DAT_007d4110 + 0x170))(DAT_007d4110,*(undefined4 *)(iVar5 + 0x10));
  }
  piVar1 = param_5;
  iVar6 = param_1;
  if (iVar7 == 0) {
    if (((param_3 & 0x84) == 0) || (param_5 == (int *)0x0)) {
      FUN_004d6e70(0,0);
    }
    else {
      FUN_004d6e70(param_5,0);
      param_2 = 1;
      if ((((*(byte *)(*piVar1 + 0x23) & 0x20) != 0) && ((*(byte *)(*param_6 + 0x23) & 0x20) != 0))
         && (piVar1 != param_6)) {
        FUN_004d3b00(*param_6);
      }
    }
    if ((puVar2[3] != 0) ||
       (((*(int *)(iVar5 + 0xc) == 0 && (*(char *)(*(int *)(iVar5 + 8) + 7) == -1)) &&
        ((param_2 == 0 || (iVar6 = FUN_004cfee0(piVar1), iVar6 == 0)))))) {
      iVar6 = FUN_004cfee0(param_6);
      if (iVar6 == 0) {
        if ((DAT_007dc618 != 0) ||
           ((uVar8 == 0xff && (((param_2 == 0 || (DAT_007dc620 != 0)) || (DAT_00623d34 != 0))))))
        {
          param_4 = 1;
        }
      }
      else if (DAT_007dc600 != 0) {
        iStack_10 = puVar2[2];
        iStack_c = iStack_10;
        iStack_8 = iStack_10;
        iStack_4 = iStack_10;
        (**(code **)(*DAT_007d4110 + 0x1b4))(DAT_007d4110,0,&iStack_10,1);
        if (iStack_8 == 0) {
          FUN_00541b50(param_2,0,*puVar2);
          if (DAT_006181d4 != DAT_007dc604) {
            DAT_006181d4 = DAT_007dc604;
            (**(code **)(*DAT_007d4110 + 0x1ac))(DAT_007d4110,DAT_007dc604);
          }
          iVar6 = iStack_c;
          if (*(int *)(iStack_c + 8) == 0) {
            FUN_004d53b0();
            (**(code **)(*DAT_007d4110 + 0x144))
                      (DAT_007d4110,*(undefined4 *)(iVar6 + 0xc),*(undefined4 *)(iVar5 + 0x14),
                       *(undefined4 *)(iVar5 + 0x20));
          }
          else {
            FUN_004d53b0();
            (**(code **)(*DAT_007d4110 + 0x148))
                      (DAT_007d4110,*(undefined4 *)(iVar6 + 0xc),*(undefined4 *)(iVar5 + 0x14),0,
                       *(undefined4 *)(iVar5 + 0x18),*(undefined4 *)(iVar5 + 0x1c),
                       *(undefined4 *)(iVar5 + 0x20));
          }
          FUN_004d6e70(0,0);
          FUN_004d54f0(0,0x18,0);
          FUN_004d54f0(0,0xb,0);
          return;
        }
        FUN_00541b50(param_2,1,*puVar2);
        if (DAT_006181d4 != DAT_007dc600) {
          DAT_006181d4 = DAT_007dc600;
          (**(code **)(*DAT_007d4110 + 0x1ac))(DAT_007d4110,DAT_007dc600);
        }
        iVar6 = iStack_c;
        if (*(int *)(iStack_c + 8) == 0) {
          FUN_004d53b0();
          (**(code **)(*DAT_007d4110 + 0x144))
                    (DAT_007d4110,*(undefined4 *)(iVar6 + 0xc),*(undefined4 *)(iVar5 + 0x14),
                     *(undefined4 *)(iVar5 + 0x20));
        }
        else {
          FUN_004d53b0();
          (**(code **)(*DAT_007d4110 + 0x148))
                    (DAT_007d4110,*(undefined4 *)(iVar6 + 0xc),*(undefined4 *)(iVar5 + 0x14),0,
                     *(undefined4 *)(iVar5 + 0x18),*(undefined4 *)(iVar5 + 0x1c),
                     *(undefined4 *)(iVar5 + 0x20));
        }
        FUN_004d6e70(0,1);
        FUN_004d54f0(1,0x18,0);
        FUN_004d54f0(1,0xb,1);
        return;
      }
    }
    if (DAT_006181d4 != 0) {
      DAT_006181d4 = 0;
      (**(code **)(*DAT_007d4110 + 0x1ac))(DAT_007d4110,0);
    }
    if (param_2 == 0) {
      FUN_004d54f0(0,1,3);
      FUN_004d54f0(0,3,0);
      FUN_004d54f0(0,4,3);
      FUN_004d54f0(0,6,0);
    }
    else {
      FUN_004d54f0(0,1,4);
      FUN_004d54f0(0,2,2);
      FUN_004d54f0(0,3,0);
      FUN_004d54f0(0,4,4);
      FUN_004d54f0(0,5,2);
      FUN_004d54f0(0,6,0);
    }
    iVar6 = param_1;
    if (param_4 != 0) {
      if ((uVar8 == 0xff) && (param_2 == 0)) {
        FUN_00541b50(param_6,0,*puVar2);
        FUN_004d54f0(0,1,7);
        iVar6 = param_1;
        if (*(int *)(param_1 + 8) == 0) {
          FUN_004d53b0();
          (**(code **)(*DAT_007d4110 + 0x144))
                    (DAT_007d4110,*(undefined4 *)(iVar6 + 0xc),*(undefined4 *)(iVar5 + 0x14),
                     *(undefined4 *)(iVar5 + 0x20));
        }
        else {
          FUN_004d53b0();
          (**(code **)(*DAT_007d4110 + 0x148))
                    (DAT_007d4110,*(undefined4 *)(iVar6 + 0xc),*(undefined4 *)(iVar5 + 0x14),0,
                     *(undefined4 *)(iVar5 + 0x18),*(undefined4 *)(iVar5 + 0x1c),
                     *(undefined4 *)(iVar5 + 0x20));
        }
        FUN_004d6e70(0,0);
        FUN_004d54f0(0,0x18,0);
        FUN_004d54f0(0,0xb,0);
        return;
      }
      FUN_00541b50(param_6,1,*puVar2);
      if (uVar8 == 0xff) {
        FUN_004d54f0(1,1,7);
        FUN_004d54f0(1,2,2);
        FUN_004d54f0(1,3,1);
        if (DAT_00623d34 != 0) {
          param_6 = (int *)0x0;
          FUN_004d53b0();
          iVar6 = (**(code **)(*DAT_007d4110 + 0x118))(DAT_007d4110,&param_6);
          if ((iVar6 < 0) || (DAT_007dc620 = 1, param_6 != (int *)0x1)) {
            DAT_007dc620 = 0;
          }
          DAT_00623d34 = 0;
        }
      }
      else {
        FUN_004d5480(0x3c,((uVar8 << 8 | uVar8) << 8 | uVar8) << 8 | uVar8);
        FUN_004d54f0(1,1,0x19);
        FUN_004d54f0(1,0x1a,1);
        FUN_004d54f0(1,2,2);
        FUN_004d54f0(1,3,3);
      }
      iVar6 = param_1;
      if (*(int *)(param_1 + 8) == 0) {
        FUN_004d53b0();
        (**(code **)(*DAT_007d4110 + 0x144))
                  (DAT_007d4110,*(undefined4 *)(iVar6 + 0xc),*(undefined4 *)(iVar5 + 0x14),
                   *(undefined4 *)(iVar5 + 0x20));
      }
      else {
        FUN_004d53b0();
        (**(code **)(*DAT_007d4110 + 0x148))
                  (DAT_007d4110,*(undefined4 *)(iVar6 + 0xc),*(undefined4 *)(iVar5 + 0x14),0,
                   *(undefined4 *)(iVar5 + 0x18),*(undefined4 *)(iVar5 + 0x1c),
                   *(undefined4 *)(iVar5 + 0x20));
      }
      FUN_004d6e70(0,1);
      FUN_004d54f0(1,1,1);
      FUN_004d54f0(1,4,1);
      FUN_004d54f0(1,0x18,0);
      FUN_004d54f0(1,0xb,1);
      return;
    }
    if (*(int *)(param_1 + 8) == 0) {
      FUN_004d53b0();
      (**(code **)(*DAT_007d4110 + 0x144))
                (DAT_007d4110,*(undefined4 *)(iVar6 + 0xc),*(undefined4 *)(iVar5 + 0x14),
                 *(undefined4 *)(iVar5 + 0x20));
    }
    else {
      FUN_004d53b0();
      (**(code **)(*DAT_007d4110 + 0x148))
                (DAT_007d4110,*(undefined4 *)(iVar6 + 0xc),*(undefined4 *)(iVar5 + 0x14),0,
                 *(undefined4 *)(iVar5 + 0x18),*(undefined4 *)(iVar5 + 0x1c),
                 *(undefined4 *)(iVar5 + 0x20));
    }
  }
  FUN_00541b50(param_6,0,*puVar2);
  iVar7 = FUN_004d71f0();
  if (iVar7 == 0) {
    FUN_004d7100(1);
  }
  (**(code **)(DAT_007d3ff8 + 0x24))(10,&param_2);
  (**(code **)(DAT_007d3ff8 + 0x24))(0xb,&param_4);
  FUN_004d6c40(5);
  FUN_004d6c90(2);
  uVar8 = uVar8 | ((uVar8 << 8 | uVar8) << 8 | uVar8) << 8;
  if (uVar8 == 0xffffffff) {
    FUN_004d54f0(0,1,2);
    FUN_004d54f0(0,2,2);
    FUN_004d54f0(0,4,2);
    FUN_004d54f0(0,5,2);
  }
  else {
    FUN_004d5480(0x3c,uVar8);
    FUN_004d54f0(0,1,4);
    FUN_004d54f0(0,2,2);
    FUN_004d54f0(0,3,3);
    FUN_004d54f0(0,4,2);
    FUN_004d54f0(0,5,2);
    FUN_004d54f0(0,6,3);
  }
  FUN_004d54d0(0x89,&param_3);
  FUN_004d54d0(0xe,&param_5);
  FUN_004d54d0(0x1c,&param_6);
  FUN_004d5480(0x89,0);
  FUN_004d5480(0xe,0);
  if (param_6 != (int *)0x0) {
    FUN_004d54d0(0x22,&param_1);
    FUN_004d5480(0x22,0);
  }
  if (*(int *)(iVar6 + 8) == 0) {
    FUN_004d53b0();
    (**(code **)(*DAT_007d4110 + 0x144))
              (DAT_007d4110,*(undefined4 *)(iVar6 + 0xc),*(undefined4 *)(iVar5 + 0x14),
               *(undefined4 *)(iVar5 + 0x20));
  }
  else {
    FUN_004d53b0();
    (**(code **)(*DAT_007d4110 + 0x148))
              (DAT_007d4110,*(undefined4 *)(iVar6 + 0xc),*(undefined4 *)(iVar5 + 0x14),0,
               *(undefined4 *)(iVar5 + 0x18),*(undefined4 *)(iVar5 + 0x1c),
               *(undefined4 *)(iVar5 + 0x20));
  }
  if (iStack_4 != 0) {
    FUN_004d5480(0x22,unaff_EBP);
  }
  FUN_004d5480(0xe,iStack_8);
  FUN_004d5480(0x89,iStack_10);
  if (uVar8 == 0xffffffff) {
    FUN_004d54f0(0,1,4);
    uVar10 = 4;
    uVar9 = 4;
  }
  else {
    FUN_004d54f0(0,3,0);
    uVar10 = 0;
    uVar9 = 6;
  }
  FUN_004d54f0(0,uVar9,uVar10);
  FUN_004d7100(0);
  (**(code **)(DAT_007d3ff8 + 0x20))(10,unaff_EBX);
  (**(code **)(DAT_007d3ff8 + 0x20))(0xb,iStack_c);
  FUN_004d54f0(0,0x18,0);
  FUN_004d54f0(0,0xb,0);
  return;
}
