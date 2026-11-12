// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a7b60 (h2): best 53.5% (regs 59.0%), class b. 770 B, structure transcribed, locals as array and int flags; ran out of budget. relocheck BAD only because the code differs from the original. */
/* PROV-FIX-9 2026-11-12 (verify/provfix9_20261112/RESULT_PROV-FIX-9.md, diagnosis verify/provcrash_20261112/RESULT_PROV-CRASH-12.md): hdr holds 5 dwords for the same FUN_005ab380 writer (original frame sub esp,0x24). Best now 60.9%. */
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
MGLOBAL(int, DAT_007dcaa8, 0x007dcaa8);
MGLOBAL(undefined4, DAT_007dcaac, 0x007dcaac);
void FUN_004522d0(uint p0);
uint FUN_004cbd30(undefined4 p0, uint p1, uint p2);
int FUN_004cc050(undefined4 p0, int p1);
void FUN_005a79a0(uint p0);
undefined4 FUN_005a7b40(undefined4 p0);
undefined4 FUN_005a7b50(void);
int FUN_005a7ee0(uint p0, undefined4 p1);
int FUN_005ab380(undefined4 p0, int * p1, uint p2);
int FUN_005ab410(undefined4 p0, int p1, undefined4 p2, undefined4 p3, uint p4);
void FUN_005abcf0(int p0);
int FUN_005abfa0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, uint p4);
int FUN_005ade10(uint p0, int p1);
uint FUN_005aea00(int p0, undefined4 p1);
void FUN_005aea40(int p0);
uint FUN_005aea10(int p0, undefined4 p1);
void FUN_005aec00(uint * p0, uint p1);
// MATCH: 0x005a7b60 FUN_005a7b60
uint FUN_005a7b60(uint param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5,uint param_6)

{
  undefined4 uVar1;
  int bVar2;
  int bVar3;
  int bVar4;
  undefined4 uVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  int iStack_20;
  uint uStack_18;
  int hdr[5];

  uVar1 = param_2;
  uVar8 = param_6 >> 4 & 1;
  bVar2 = 0;
  bVar4 = 0;
  uVar7 = param_1;
  iVar6 = FUN_005ab380(param_2,hdr,uVar8);
  if (((iVar6 != 0) && (hdr[0] == 0x80a)) && (hdr[2] != 0)) {
    if (param_1 == 0) {
      uVar7 = FUN_005aea00(hdr[1],0x30808);
      if (uVar7 == 0) {
        return 0;
      }
      iVar6 = FUN_004cbd30(uVar1,uVar7,hdr[1]);
      if (hdr[1] != iVar6) {
        FUN_004522d0(uVar7);
        return 0;
      }
      uVar8 = (*(byte *)(uVar7 + 0x1b) & 8) >> 3;
      if (uVar8 != 0) {
        param_6 = param_6 | 0x10;
      }
      iVar6 = FUN_005a7ee0(uVar7,uVar8);
      if (iVar6 == 0) {
        FUN_004522d0(uVar7);
        return 0;
      }
      bVar4 = 1;
      bVar2 = 1;
    }
    else {
      iVar6 = FUN_004cc050(uVar1,hdr[1]);
      if (iVar6 == 0) {
        return 0;
      }
    }
    iVar6 = FUN_005ab380(uVar1,hdr,uVar8);
    if (((iVar6 == 0) || (hdr[0] != 0x80c)) || (hdr[2] == 0)) {
      if (bVar2) {
        FUN_005a79a0(uVar7);
      }
    }
    else {
      bVar3 = 0;
      iStack_20 = 0;
      param_2 = FUN_005a7b50();
      FUN_005a7b40(uVar7);
      iVar6 = FUN_004cbd30(uVar1,(uint)&param_1,4);
      if (iVar6 == 4) {
        if (uVar8 != 0) {
          FUN_005aec00(&param_1,4);
        }
        if (DAT_007dcaa8 == 0) {
          iStack_20 = FUN_005aea10(0x10000,0x30808);
          if (iStack_20 == 0) {
            if (!bVar2) {
              return 0;
            }
            FUN_005a79a0(uVar7);
            FUN_005a7b40(param_2);
            return 0;
          }
          DAT_007dcaa8 = iStack_20;
          DAT_007dcaac = 0x10000;
        }
        uVar5 = param_5;
        uStack_18 = 0;
        if (param_1 != 0) {
          do {
            iVar6 = FUN_005ab410(uVar1,0x802,0,0,uVar8);
            if (iVar6 == 0) {
              bVar3 = 1;
            }
            iVar6 = FUN_005abfa0(uVar1,param_4,param_3,uVar5,param_6);
            if ((iVar6 == 0) || (bVar3)) {
              iVar6 = *(int *)(uVar7 + 0x10);
              while (iVar6 != uVar7 + 0xc) {
                uVar1 = *(undefined4 *)(iVar6 + 8);
                FUN_005ade10(uVar7 + 0xc,uVar1);
                FUN_005abcf0(uVar1);
                iVar6 = *(int *)(uVar7 + 0x10);
              }
              if (bVar4) {
                FUN_005a79a0(uVar7);
                FUN_005a7b40(param_2);
              }
              if (iStack_20 == 0) {
                return 0;
              }
              DAT_007dcaa8 = 0;
              DAT_007dcaac = 0;
              FUN_005aea40(iStack_20);
              return 0;
            }
            uStack_18 = uStack_18 + 1;
          } while (uStack_18 < param_1);
        }
        if (iStack_20 != 0) {
          DAT_007dcaa8 = 0;
          DAT_007dcaac = 0;
          FUN_005aea40(iStack_20);
        }
        FUN_005a7b40(param_2);
        return uVar7;
      }
      if (bVar2) {
        FUN_005a7b40(param_2);
        FUN_005a79a0(uVar7);
        return 0;
      }
    }
  }
  return 0;
}
