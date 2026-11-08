// FLAGS: /O2 /Ob1 /G5
/* h2 note: 30.7% (regs 36.9). Fixed compile error (FUN_005412d0 proto/arg types) over raw 0 score. cxx FAIL: FUN_00540340 arity, int** to undefined4*. class (b). */
/* MATCH-RWL04 0x00541d40: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_006181c8, 0x006181c8);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(int, DAT_006181cc, 0x006181cc);
MGLOBAL(int, DAT_006181d0, 0x006181d0);
MGLOBAL(int, DAT_007dc608, 0x007dc608);
MGLOBAL(int, DAT_007dc5fc, 0x007dc5fc);
MGLOBAL(int, DAT_007dc5dc, 0x007dc5dc);
MGLOBAL(undefined1, DAT_005cd04c, 0x005cd04c);
MGLOBAL(int, DAT_006181d4, 0x006181d4);
MGLOBAL(int, DAT_007dc60c, 0x007dc60c);
MGLOBAL(int, DAT_007dc610, 0x007dc610);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
void FUN_004cb130();
void FUN_004cb190(int p0, int p1);
int FUN_004cfee0();
void FUN_004d53b0(void);
void FUN_004d5480();
void FUN_004d54d0(int p0, undefined4 * p1);
void FUN_004d54f0();
undefined4 FUN_004d6c40(int p0);
undefined4 FUN_004d6c90(int p0);
undefined4 FUN_004d6e70();
void FUN_004d7100();
undefined4 FUN_004d71f0(void);
bool FUN_004dcbb0();
bool FUN_004dcd30(int * p0);
void FUN_00540340();
void FUN_005412d0(int p0, int p1, uint p2, uint p3, int * p4, int * p5);
undefined4 FUN_00541b50();
undefined4 FUN_005422c0();
MGLOBAL(int, _DAT_005cd04c, 0x005cd04c);
// MATCH: 0x00541d40 FUN_00541d40
void FUN_00541d40(int param_1,int *param_2,int *param_3,int param_4,int param_5,int param_6)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  undefined4 unaff_EBP;
  int *piStack_38;
  undefined4 *puStack_34;
  int local_20;
  undefined4 uStack_1c;
  float fStack_18;
  int iStack_14;
  int iStack_10;
  int iStack_c;
  int iStack_8;
  int iStack_4;
  
  iVar1 = param_4;
  iVar4 = param_5;
  if (param_5 == 0) {
    if (param_4 == 0) {
LAB_00542296:
      puStack_34 = (undefined4 *)iVar1;
      piStack_38 = param_3;
      FUN_00540340(param_1,param_2);
      return;
    }
    puStack_34 = (undefined4 *)param_4;
    piStack_38 = (int *)0x541d61;
    iVar3 = FUN_004cfee0();
    iVar4 = iVar1;
    if (iVar3 == 0) goto LAB_00542296;
  }
  piVar2 = param_2;
  puStack_34 = &param_2;
  piStack_38 = &local_20;
  iVar3 = FUN_004dcbb0(0x2c,param_2[6],&param_5);
  iVar1 = param_1;
  if (iVar3 == 0) {
    return;
  }
  puStack_34 = (undefined4 *)local_20;
  piStack_38 = piVar2;
  FUN_005422c0(param_1);
  FUN_004dcd30(param_5);
  FUN_004d6e70(iVar4,0);
  if (DAT_006181c8 != 0x252) {
    DAT_006181c8 = 0x252;
    DAT_006181cc = -1;
    puStack_34 = (undefined4 *)0x252;
    piStack_38 = DAT_007d4110;
    (**(code **)(*DAT_007d4110 + 0x164))();
  }
  puStack_34 = (undefined4 *)0x2c;
  piStack_38 = (int *)0x0;
  FUN_004cb130(0,param_5);
  if (DAT_006181d0 != piVar2[4]) {
    puStack_34 = (undefined4 *)piVar2[4];
    piStack_38 = DAT_007d4110;
    DAT_006181d0 = piVar2[4];
    (**(code **)(*DAT_007d4110 + 0x170))();
  }
  iVar3 = param_6;
  if (((param_6 == 0) || (DAT_007dc608 == 0)) && (DAT_007dc5fc == 0)) {
    puStack_34 = (undefined4 *)0x4;
    piStack_38 = (int *)0x1;
    FUN_004d54f0(0);
    FUN_004d54f0(0,2,2);
    FUN_004d54f0(0,3,0);
    FUN_004d54f0(0,4,4);
    FUN_004d54f0(0,5,2);
    FUN_004d54f0(0,6,0);
    if (DAT_006181d4 != 0) {
      DAT_006181d4 = 0;
      puStack_34 = (undefined4 *)0x0;
      piStack_38 = DAT_007d4110;
      (**(code **)(*DAT_007d4110 + 0x1ac))();
    }
    puStack_34 = (undefined4 *)0x5420c4;
    iVar4 = FUN_004d71f0();
    if (iVar4 == 0) {
      puStack_34 = (undefined4 *)0x1;
      piStack_38 = (int *)0x5420cf;
      FUN_004d7100();
    }
    puStack_34 = (undefined4 *)0x0;
    piStack_38 = (int *)0xf;
    FUN_004d5480();
    (**(code **)(DAT_007d3ff8 + 0x24))(10,&param_1);
    (**(code **)(DAT_007d3ff8 + 0x24))(0xb,&param_6);
    FUN_004d6c40(6);
    FUN_004d6c90(1);
    if (*(int *)(iVar1 + 8) == 0) {
      puStack_34 = (undefined4 *)0x542143;
      FUN_004d53b0();
      puStack_34 = (undefined4 *)piVar2[8];
      piStack_38 = param_2;
      (**(code **)(*DAT_007d4110 + 0x144))(DAT_007d4110,*(undefined4 *)(iVar1 + 0xc));
    }
    else {
      puStack_34 = (undefined4 *)0x542117;
      FUN_004d53b0();
      puStack_34 = (undefined4 *)piVar2[8];
      piStack_38 = (int *)piVar2[7];
      (**(code **)(*DAT_007d4110 + 0x148))
                (DAT_007d4110,*(undefined4 *)(iVar1 + 0xc),param_2,0,piVar2[6]);
    }
    FUN_004d5480(0xf,1);
    FUN_004d6c40(5);
    FUN_004d6c90(2);
    FUN_004d54d0(0xe,&piStack_38);
    FUN_004d5480(0xe,0);
    FUN_004d54f0(0,0xb,1);
    if (*(int *)(iVar1 + 8) == 0) {
      FUN_004d53b0();
      iVar4 = iStack_14;
      (**(code **)(*DAT_007d4110 + 0x144))
                (DAT_007d4110,*(undefined4 *)(iVar1 + 0xc),iStack_14,piVar2[8]);
    }
    else {
      FUN_004d53b0();
      iVar4 = piVar2[7];
      (**(code **)(*DAT_007d4110 + 0x148))
                (DAT_007d4110,*(undefined4 *)(iVar1 + 0xc),iStack_14,0,piVar2[6],iVar4,piVar2[8]);
    }
    FUN_004d54f0(0,0xb,0);
    FUN_004d5480(0xe,iVar4);
    FUN_004d7100(0);
    (**(code **)(DAT_007d3ff8 + 0x20))(10,puStack_34);
    (**(code **)(DAT_007d3ff8 + 0x20))(0xb,local_20);
    goto LAB_0054222c;
  }
  puStack_34 = (undefined4 *)0x0;
  piStack_38 = (int *)0x541e43;
  FUN_004d7100();
  piStack_38 = (int *)0x1;
  FUN_004d6e70(iVar4);
  FUN_004d54d0(0x1b,&param_1);
  FUN_004d54d0(0xf,&param_6);
  FUN_004d5480(0x1b,0);
  FUN_004d5480(0xf,0);
  if ((iVar3 == 0) || (DAT_007dc608 == 0)) {
LAB_00541fb7:
    if (DAT_006181d4 != DAT_007dc5fc) {
      DAT_006181d4 = DAT_007dc5fc;
      puStack_34 = (undefined4 *)DAT_007dc5fc;
      piStack_38 = DAT_007d4110;
      (**(code **)(*DAT_007d4110 + 0x1ac))();
    }
    if (*(int *)(iVar1 + 8) == 0) {
      puStack_34 = (undefined4 *)0x542013;
      FUN_004d53b0();
      puStack_34 = (undefined4 *)piVar2[8];
      piStack_38 = param_2;
      (**(code **)(*DAT_007d4110 + 0x144))(DAT_007d4110,*(undefined4 *)(iVar1 + 0xc));
    }
    else {
      puStack_34 = (undefined4 *)0x541fe7;
      FUN_004d53b0();
      puStack_34 = (undefined4 *)piVar2[8];
      piStack_38 = (int *)piVar2[7];
      (**(code **)(*DAT_007d4110 + 0x148))
                (DAT_007d4110,*(undefined4 *)(iVar1 + 0xc),param_2,0,piVar2[6]);
    }
  }
  else {
    iVar4 = *(int *)(piVar2[2] + DAT_007dc5dc);
    fStack_18 = *(float *)(iVar4 + 0x20) * _DAT_005cd04c;
    iStack_14 = (int)ROUND(fStack_18);
    if ((char)iStack_14 == '\0') goto LAB_00541fb7;
    puStack_34 = *(undefined4 **)(iVar4 + 0x18);
    iStack_10 = *(int *)(iVar4 + 0x20);
    piStack_38 = (int *)0x2;
    iStack_c = iStack_10;
    iStack_8 = iStack_10;
    iStack_4 = iStack_10;
    FUN_00541b50(iVar3);
    puStack_34 = (undefined4 *)DAT_007dc608;
    if (*(int *)(iVar4 + 0x24) != 0) {
      puStack_34 = (undefined4 *)iVar3;
      piStack_38 = (int *)0x541eea;
      iVar4 = FUN_004cfee0();
      puStack_34 = (undefined4 *)DAT_007dc610;
      if (iVar4 != 0) {
        puStack_34 = (undefined4 *)DAT_007dc60c;
      }
    }
    if ((undefined4 *)DAT_006181d4 != puStack_34) {
      piStack_38 = DAT_007d4110;
      DAT_006181d4 = (int)puStack_34;
      (**(code **)(*DAT_007d4110 + 0x1ac))();
    }
    piStack_38 = &iStack_10;
    puStack_34 = (undefined4 *)0x1;
    (**(code **)(*DAT_007d4110 + 0x1b4))(DAT_007d4110,0);
    if (*(int *)(iVar1 + 8) == 0) {
      FUN_004d53b0();
      (**(code **)(*DAT_007d4110 + 0x144))
                (DAT_007d4110,*(undefined4 *)(iVar1 + 0xc),iStack_8,piVar2[8]);
    }
    else {
      FUN_004d53b0();
      (**(code **)(*DAT_007d4110 + 0x148))
                (DAT_007d4110,*(undefined4 *)(iVar1 + 0xc),iStack_8,0,piVar2[6],piVar2[7],piVar2[8])
      ;
    }
    FUN_004d6e70(0,2);
    FUN_004d54f0(2,0x18,0);
    FUN_004d54f0(2,0xb,2);
  }
  FUN_004d5480(0x1b,unaff_EBP);
  FUN_004d5480(0xf,iStack_14);
  FUN_004d6e70(0,1);
LAB_0054222c:
  FUN_004cb190(iVar1 + 0x10,*(undefined4 *)(iVar1 + 0x30));
  if (DAT_006181cc != *(int *)(iVar1 + 0x34)) {
    DAT_006181c8 = -1;
    DAT_006181cc = *(int *)(iVar1 + 0x34);
    (**(code **)(*DAT_007d4110 + 0x15c))(DAT_007d4110,*(undefined4 *)(iVar1 + 0x34));
  }
  if ((iVar3 != 0) && (DAT_007dc608 == 0)) {
    FUN_005412d0(iVar1,(int)piVar2,local_20,1,(int *)uStack_1c,(int *)iVar3);
    return;
  }
  return;
}
