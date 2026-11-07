// FLAGS: /O2 /Ob1 /G5
/* best 62.9% (regs 63.2%, ops ~97%): permuter result. Header struct SH (4 dwords) gives the 0x1c frame; residual = register allocation (ebx/ebp/edi
   roles) and hoisted loads at entry; 212 vs 211 insns. 5 callees re-prototyped to their real arity (FUN_005ab380 3, FUN_005aea00 2,
   FUN_004522d0 1, FUN_005aea40 1). Class (b). */
/* MATCH-S25 0x005abfa0: hand-matched (h1). */
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

#ifndef __cplusplus
#define true 1
#define false 0
#endif
typedef struct { int id; uint size; int c; int d; } SH;
MGLOBAL(int, DAT_007dcaa8, 0x007dcaa8);
MGLOBAL(uint, DAT_007dcaac, 0x007dcaac);
MGLOBAL(undefined1, DAT_005e6414, 0x005e6414);
MGLOBAL(undefined1, DAT_005e6444, 0x005e6444);
void FUN_004522d0(int p0);
uint FUN_004cbd30(undefined4 p0, int p1, uint p2);
undefined4 FUN_005ab380(undefined4 p0, SH *p1, uint p2);
void FUN_005abcf0(int p0);
undefined4 FUN_005abd30(int p0, undefined4 p1, int p2, undefined4 p3);
undefined4 FUN_005abf80(int p0);
int FUN_005ac210(int p0, undefined4 p1, undefined4 p2, undefined4 p3, uint p4);
int FUN_005adf30(undefined4 p0, undefined4 p1);
int FUN_005aea00(uint p0, undefined4 p1);
uint FUN_005aea10(uint p0, undefined4 p1);
void FUN_005aea40(int p0);
void FUN_005aec30(int p0, uint p1, int p2);
// MATCH: 0x005abfa0 FUN_005abfa0
int FUN_005abfa0(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                uint param_5)
{
  bool bVar1;
  bool bVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  int local_1c;
  SH h;
  uVar9 = param_5 >> 4 & 1;
  local_1c = DAT_007dcaa8;
  bVar2 = false;
  iVar3 = FUN_005ab380(param_1,&h,uVar9);
  iVar8 = DAT_007dcaa8;
  if (((((iVar3 != 0) && (h.id == 0x803)) && (h.c != 0)) &&
      ((iVar3 = DAT_007dcaa8, DAT_007dcaa8 != 0 ||
       (iVar3 = FUN_005aea00(h.size,0x30806), iVar3 != 0)))) &&
     (uVar4 = FUN_004cbd30(param_1,iVar3,h.size), h.size == uVar4)) {
    iVar5 = FUN_005ac210(iVar3,param_2,param_3,param_4,param_5);
    if (DAT_007dcaa8 == 0) {
      FUN_004522d0(iVar3);
    }
    if (iVar5 != 0) {
      iVar3 = FUN_005ab380(param_1,&h,uVar9);
      if (((iVar3 != 0) && (h.id == 0x804)) && (h.c != 0)) {
        iVar3 = FUN_005adf30(*(undefined4 *)(iVar5 + 0x14),*(undefined4 *)(iVar5 + 0x30));
        iVar6 = FUN_005adf30(*(undefined4 *)(iVar5 + 0x14),(undefined4)&DAT_005e6414);
        if ((((iVar6 == 0) ||
             (iVar6 = FUN_005adf30(*(undefined4 *)(iVar5 + 0x14),(undefined4)&DAT_005e6444), iVar6 == 0)) ||
            (iVar6 = FUN_005adf30(*(undefined4 *)(iVar5 + 0x30),(undefined4)&DAT_005e6414), iVar6 == 0)) ||
           (iVar6 = FUN_005adf30(*(undefined4 *)(iVar5 + 0x30),(undefined4)&DAT_005e6444), iVar6 == 0)) {
          bVar1 = true;
        }
        else {
          bVar1 = false;
        }
        if (((iVar3 != 0) && (bVar1)) || (iVar8 == 0)) {
          local_1c = FUN_005aea10(h.size,0x30806);
          bVar2 = true;
        }
        uVar4 = h.size;
        uVar11 = 0;
        if (h.size != 0) {
          do {
            uVar10 = h.size;
            if ((!bVar2) && (uVar10 = DAT_007dcaac, uVar4 < DAT_007dcaac + uVar11)) {
              uVar10 = uVar4 - uVar11;
            }
            uVar7 = FUN_004cbd30(param_1,local_1c,uVar10);
            if (uVar10 != uVar7) break;
            if ((uVar9 != 0) && (8 < *(byte *)(iVar5 + 0x1c))) {
              FUN_005aec30(local_1c,uVar10,*(byte *)(iVar5 + 0x1c) >> 3);
            }
            iVar8 = FUN_005abd30(iVar5,uVar11,local_1c,uVar10);
            if (iVar8 == 0) break;
            do {
              iVar8 = FUN_005abf80(iVar5);
            } while (iVar8 == 1);
            uVar11 = uVar11 + uVar10;
          } while (uVar11 < uVar4);
        }
        if (bVar2) {
          FUN_005aea40(local_1c);
        }
        return iVar5;
      }
      FUN_005abcf0(iVar5);
    }
  }
  return 0;
}
