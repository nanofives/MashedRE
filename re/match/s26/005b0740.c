// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005b0740 (558 B). Best 56.7% (regs 74.9%), residual class (b). ops 98.1% (215/214): recursion with 28-byte Loc struct local (matches orig 0x24 frame layout), for(n=head->next;n!=head;n=n->next) form shares the return-0 exit; residual: orig spills iVar5 into dead param slot [esp+0x4c] (frame 0x24 vs ours 0x28), so every param offset shifts by 4; permuter 57.9% variant rewrote an index expression (not faithful), rejected. */
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
typedef struct Loc { int a; int b; int c; unsigned char d; unsigned char e; int f; int g; unsigned char h; unsigned char i; } Loc;
void FUN_005aaa00(int p0, int p1, int p2);
void FUN_005ac980(int p0, Loc *p1, int p2);
int FUN_005ac9e0(int p0, int *p1);
int FUN_005af860(int p0, int *p1, int *p2, int p3, int p4);
int FUN_005b0a90(int * p0, int p1, int p2, int * p3, int * p4, int p5, int p6);
void FUN_005b0b10(int p0);
// MATCH: 0x005b0740 FUN_005b0740
int FUN_005b0740(Loc *param_1,int *param_2,int param_3,int param_4,int *param_5,
                 int *param_6,int param_7,int *param_8)
{
  int *piVar1;
  int *puVar2;
  int *puVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  unsigned int uVar7;
  int *local_24;
  int local_20;
  Loc loc;

  for (local_24 = (int *)param_5[1]; local_24 != param_5; local_24 = (int *)local_24[1]) {
    iVar5 = local_24[2];
    iVar4 = *(int *)(iVar5 + 0x54);
    for (puVar3 = (int *)*param_6; puVar3 != param_6; puVar3 = (int *)*puVar3) {
      if ((puVar3[-0x12] == iVar5) && (puVar3[-1] == param_3)) goto LAB_005b08ba;
    }
    if (*(int *)(iVar4 + 4) == 0) {
      iVar4 = FUN_005af860(iVar5,(int *)param_1,param_2,param_3,param_7);
      if (iVar4 != 0) {
        FUN_005aaa00(iVar4,*(int *)(param_7 + 0x54),*(int *)(param_7 + 0x58));
        iVar5 = FUN_005b0a90(param_6,iVar5,param_4,(int *)param_1,param_2,param_3,0);
        return iVar5 != 0;
      }
    }
    else {
      uVar7 = 0;
      if (*(int *)(iVar4 + 8) != 0) {
        do {
          iVar6 = FUN_005ac9e0(**(int **)(*(int *)(iVar4 + 4) + uVar7 * 4),(int *)param_1);
          if (iVar6 != 0) {
            iVar6 = FUN_005ac9e0(*(int *)(*(int *)(*(int *)(iVar4 + 4) + uVar7 * 4) + 4),
                                 param_2);
            if (iVar6 != 0) {
              iVar5 = FUN_005b0a90(param_6,iVar5,param_4,(int *)param_1,param_2,param_3,0);
              if (iVar5 == 0) {
                return 0;
              }
LAB_005b0902:
              if (param_8 != 0) {
                *param_8 = iVar5;
              }
              return 1;
            }
            loc.b = 0;
            loc.c = 0;
            loc.d = 0;
            loc.a = param_1->a;
            loc.e = 0;
            loc.h = 0;
            loc.i = 0;
            loc.f = 0;
            loc.g = 0;
            FUN_005ac980(*(int *)(*(int *)(*(int *)(iVar4 + 4) + uVar7 * 4) + 4),&loc,1);
            iVar6 = FUN_005b0740(&loc,param_2,param_3,param_4,param_5,param_6,param_7,&local_20);
            if (iVar6 != 0) {
              iVar5 = FUN_005b0a90(param_6,iVar5,param_4,(int *)param_1,param_2,param_3,local_20);
              if (iVar5 == 0) {
                puVar3 = (int *)*param_6;
                while (puVar3 != param_6) {
                  piVar1 = puVar3 + -1;
                  puVar2 = puVar3 + -0x13;
                  puVar3 = (int *)*puVar3;
                  if (param_3 == *piVar1) {
                    FUN_005b0b10((int)puVar2);
                  }
                }
                return 0;
              }
              goto LAB_005b0902;
            }
          }
          uVar7 = uVar7 + 1;
        } while (uVar7 < *(unsigned int *)(iVar4 + 8));
      }
    }
LAB_005b08ba:
    ;
  }
  return 0;
}
