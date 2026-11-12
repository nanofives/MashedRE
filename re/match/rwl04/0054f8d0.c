// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* h2 note: 54.9% raw form. cxx FAIL: undefined4 vs undefined4* arg to FUN_004cbd30, undefined1* to int. class (b). */
/* MATCH-RWL04 0x0054f8d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned short ushort;
typedef unsigned char byte;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_00618138, 0x00618138);
undefined4 * FUN_004c5860(undefined4 * p0, int p1);
undefined1 * FUN_004c5890(void);
void FUN_004c5bc0(int p0, int p1);
void FUN_004c5c80(undefined4 p0);
undefined4 FUN_004c5ca0(void);
undefined4 FUN_004c7600(undefined4 p0);
undefined4 FUN_004c7650(undefined4 p0);
int FUN_004c76f0(int p0);
int FUN_004c77c0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
uint FUN_004c7860(undefined4 p0, uint p1, int p2);
uint FUN_004cbd30(undefined4 * p0, undefined4 * p1, uint p2);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, undefined4 * p2, uint * p3);
undefined4 FUN_004cdd00(uint * p0);
undefined4 * FUN_004cee90(int p0);
uint * FUN_004cefd0(uint * p0);
int FUN_004d5310(int p0, byte * p1);
undefined4 FUN_004d5340(undefined4 p0, undefined4 p1, undefined4 * p2, undefined4 * p3, undefined4 * p4, uint * p5);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
int FUN_004db2e0(int p0, undefined4 p1, undefined4 p2);
int FUN_004e1b60(int p0, int p1, int p2);
undefined4 FUN_0054fd60(int p0, undefined4 * p1);
int FUN_00550130(undefined4 p0);
// MATCH: 0x0054f8d0 FUN_0054f8d0
int FUN_0054f8d0(undefined4 param_1)

{
  uint uVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  uint *puVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  int local_40;
  int iStack_3c;
  undefined4 local_38;
  int iStack_34;
  int local_30;
  undefined4 uStack_2c;
  undefined4 uStack_28;
  uint local_24;
  uint local_20;
  undefined4 local_1c;
  uint uStack_18;
  int iStack_14;
  int iStack_10;
  int iStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar1 = FUN_004cbd30(param_1,&local_38,4);
  if (uVar1 < 4) {
    return 0;
  }
  local_30 = FUN_004c5890();
  uVar1 = 0;
  if (local_30 == 0) {
    return 0;
  }
  if ((*(ushort *)((char *)&local_38 + 2)) == 0) {
    if ((short)local_38 != 0) {
      do {
        FUN_0054fd60(param_1,&local_30);
        uVar1 = uVar1 + 1;
      } while (uVar1 < (local_38 & 0xffff));
      return local_30;
    }
  }
  else {
    local_1c = FUN_004c5ca0();
    FUN_004c5c80(local_30);
    local_20 = 0;
    if ((short)local_38 != 0) {
      do {
        local_24 = 0;
        iVar2 = FUN_004cbd30(param_1,&local_40,4);
        if (iVar2 != 4) goto LAB_0054fd22;
        piVar3 = (int *)(**(code **)(DAT_007d3ff8 + 0x108))(local_40 * 4,0x101b3);
        if (piVar3 == (int *)0x0) {
          uStack_8 = 0x1b3;
          uStack_4 = FUN_004d7ff0(0x80000013,local_40 * 4);
          FUN_004d8480(&uStack_8);
          FUN_004c5c80(local_1c);
          return 0;
        }
        iVar2 = 0;
        if (0 < local_40) {
          do {
            iVar4 = FUN_004cc5e0(param_1,0x18,0,0);
            if (iVar4 == 0) goto LAB_0054fd22;
            puVar5 = (uint *)FUN_004cee90(param_1);
            piVar3[iVar2] = (int)puVar5;
            if (puVar5 == (uint *)0x0) goto LAB_0054fd22;
            if ((*puVar5 & 2) == 0) {
              FUN_004cefd0(puVar5);
            }
            iVar2 = iVar2 + 1;
          } while (iVar2 < local_40);
        }
        iVar2 = FUN_004cc5e0(param_1,6,0,0);
        if ((iVar2 == 0) || (iStack_c = FUN_00550130(param_1), iStack_c == 0)) goto LAB_0054fd22;
        uVar1 = 4;
        uStack_28 = *(undefined4 *)(*piVar3 + 4);
        uStack_2c = *(undefined4 *)(*piVar3 + 8);
        iStack_34 = *(int *)(*piVar3 + 0xc);
        if (local_40 < 2) {
          uVar6 = *(uint *)(iStack_c + 0x50) & 0xff;
          if ((uVar6 != 1) && (uVar6 != 2)) {
            uVar1 = 0x9004;
          }
        }
        else {
          uVar1 = 0x8004;
        }
        FUN_004d5340(*piVar3,uVar1,&uStack_28,&uStack_2c,&iStack_34,&uStack_18);
        iVar2 = FUN_004c77c0(uStack_28,uStack_2c,iStack_34,uStack_18 | uVar1);
        if (iVar2 == 0) {
          iVar2 = 0;
          if (0 < local_40) {
            do {
              FUN_004cdd00(piVar3[iVar2]);
              iVar2 = iVar2 + 1;
            } while (iVar2 < local_40);
          }
          (**(code **)(DAT_007d3ff8 + 0x10c))(piVar3);
LAB_0054fd22:
          FUN_004c5c80(local_1c);
          return 0;
        }
        iVar4 = *(int *)(iVar2 + 0xc);
        if ((*(int *)(*piVar3 + 4) != iVar4) || (*(int *)(*piVar3 + 8) != *(int *)(iVar2 + 0x10))) {
          iStack_3c = *(int *)(iVar2 + 0x10);
          if (iStack_34 == 8) {
            FUN_004c7650(iVar2);
            iVar2 = FUN_004c77c0(iVar4,iStack_3c,0x20,uVar1 | 0x500);
            if (iVar2 == 0) {
              iVar2 = FUN_004c77c0(iVar4,iStack_3c,0x10,uVar1 | 0x100);
            }
          }
          iStack_10 = FUN_004c76f0(iVar2);
          iVar8 = 0;
          if (0 < iStack_10) {
            do {
              iStack_14 = FUN_004db2e0(piVar3[iVar8],iVar4,iStack_3c);
              if (iStack_14 == 0) goto LAB_0054fd22;
              FUN_004cdd00(piVar3[iVar8]);
              piVar3[iVar8] = iStack_14;
              if (1 < iVar4) {
                iVar4 = iVar4 >> 1;
              }
              if (1 < iStack_3c) {
                iStack_3c = iStack_3c >> 1;
              }
              iVar8 = iVar8 + 1;
            } while (iVar8 < iStack_10);
          }
        }
        if (1 < local_40) {
          local_24 = *(byte *)(iVar2 + 0x23) & 0x10;
          *(byte *)(iVar2 + 0x23) = ~(byte)local_24 & *(byte *)(iVar2 + 0x23);
        }
        iVar4 = FUN_004c76f0(iVar2);
        iVar8 = 0;
        if (0 < iVar4) {
          do {
            if (iVar8 == local_40) break;
            iVar7 = FUN_004c7860(iVar2,iVar8,1);
            if (((iVar7 == 0) || (iVar7 = FUN_004d5310(iVar2,piVar3[iVar8]), iVar7 == 0)) ||
               (iVar7 = FUN_004c7600(iVar2), iVar7 == 0)) goto LAB_0054fd22;
            FUN_004cdd00(piVar3[iVar8]);
            iVar8 = iVar8 + 1;
          } while (iVar8 < iVar4);
        }
        if (1 < local_40) {
          *(byte *)(iVar2 + 0x23) = *(byte *)(iVar2 + 0x23) | (byte)local_24;
        }
        if (iVar8 < local_40) {
          do {
            FUN_004cdd00(piVar3[iVar8]);
            iVar8 = iVar8 + 1;
          } while (iVar8 < local_40);
        }
        (**(code **)(DAT_007d3ff8 + 0x10c))(piVar3);
        iVar4 = iStack_c;
        FUN_004c5860(iStack_c,iVar2);
        iVar2 = FUN_004e1b60(&DAT_00618138,param_1,iVar4);
        if (iVar2 == 0) goto LAB_0054fd22;
        iVar2 = FUN_004c5ca0();
        if (iVar2 != 0) {
          FUN_004c5bc0(iVar2,iVar4);
        }
        local_20 = local_20 + 1;
      } while (local_20 < (local_38 & 0xffff));
    }
    FUN_004c5c80(local_1c);
  }
  return local_30;
}
