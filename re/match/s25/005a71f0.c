// FLAGS: /O2 /Ob1 /G5
/* 47.9% (regs 69.4%, ops 75.7%), 137 vs 144 insns: structure typed from the stack (VP6 = 6-dword param block at esp+0x18, VP2 at esp+0x10,
   param_3 address-taken as the cursor). Residual: the size sum is reassociated differently (original keeps (n+1)*0x18, *8+0x28 and
   *8+0x1c terms separate, ours folds them to *0x1c+0x5c), plus register allocation. Class (b). */
/* MATCH-S25 0x005a71f0: hand-matched (h1). */
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

typedef struct { uint a; int b; } VP2;
typedef struct { undefined4 mode; undefined4 a; undefined4 b; void *fn; uint *cur; int base; } VP6;
typedef struct { undefined2 u0; undefined2 u2; undefined2 u4; undefined2 u6; undefined2 u8; undefined2 ua; undefined2 uc; undefined2 ue; undefined2 u10; undefined2 u12; undefined2 cnt0; undefined2 cnt1; } VHDR;
MGLOBAL(int, DAT_007dca34, 0x007dca34);
MGLOBAL(int, DAT_007dca20, 0x007dca20);
MGLOBAL(int, DAT_007dca30, 0x007dca30);
MGLOBAL(uint, DAT_007dca38, 0x007dca38);
MGLOBAL(undefined1, DAT_005e61a4, 0x005e61a4);
MGLOBAL(undefined1, DAT_005e6354, 0x005e6354);
void FUN_004522d0(int p0);
int FUN_005a6b70(undefined4 p0, int p1, VP6 *p2);
void FUN_005a6cb0(int p0, undefined4 p1);
void FUN_005a6df0(int p0, int p1, int p2, VP2 *p3);
int FUN_005ad6a0(int p0);
int FUN_005aea00(uint p0, undefined4 p1);
void FUN_005a73a0(void);
// MATCH: 0x005a71f0 FUN_005a71f0
undefined4 FUN_005a71f0(undefined4 param_1,int param_2,uint param_3)
{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  uint uVar6;
  VP2 s2;
  VP6 s6;
  iVar2 = FUN_005ad6a0((int)&DAT_005e6354);
  iVar3 = FUN_005ad6a0((int)&DAT_005e61a4);
  uVar1 = param_3;
  iVar4 = FUN_005aea00(((uint)*(ushort *)(iVar3 + 0x16) + (uint)*(ushort *)(iVar3 + 0x14)) * 8 +
                       param_3 * 0x1c +
                       ((uint)*(ushort *)(iVar2 + 0x16) + (uint)*(ushort *)(iVar2 + 0x14)) * 8 +
                       0x5c,0x3080d);
  if (iVar4 == 0) {
    return 0;
  }
  s6.b = 0;
  s6.a = 0;
  s6.cur = &param_3;
  param_3 = iVar4 + 0x18;
  s6.mode = 2;
  s6.fn = (void *)FUN_005a73a0;
  s6.base = iVar4;
  DAT_007dca34 = FUN_005a6b70(param_1,iVar2,&s6);
  if (DAT_007dca34 != 0) {
    uVar6 = 0;
    DAT_007dca20 = param_3;
    iVar2 = param_3 + uVar1 * 4;
    if (uVar1 != 0) {
      do {
        param_3 = iVar2 + 0x18;
        s6.base = iVar2;
        uVar5 = FUN_005a6b70(*(undefined4 *)(param_2 + uVar6 * 4),iVar3,&s6);
        *(undefined4 *)(DAT_007dca20 + uVar6 * 4) = uVar5;
        if (*(int *)(DAT_007dca20 + uVar6 * 4) == 0) {
          for (; uVar6 != 0; uVar6 = uVar6 - 1) {
            FUN_005a6cb0(*(int *)(DAT_007dca20 + -4 + uVar6 * 4),0);
          }
          FUN_005a6cb0(DAT_007dca34,0);
          DAT_007dca38 = 0;
          DAT_007dca34 = 0;
          return 0;
        }
        uVar6 = uVar6 + 1;
        iVar2 = param_3;
      } while (uVar6 < uVar1);
    }
    DAT_007dca30 = 1;
    DAT_007dca38 = uVar1;
    s2.a = uVar1;
    s2.b = DAT_007dca20;
    FUN_005a6df0(DAT_007dca34,5,1,&s2);
    return 1;
  }
  FUN_004522d0(iVar4);
  return 0;
}
