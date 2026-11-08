// FLAGS: /O2 /Ob1 /G5
/* RESULT: pending */
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
int FUN_0055abb0(int *p0, int p1, void *p2);
void FUN_0055bd80(int p0, int p1, int p2, void *p3);
int FUN_00575560(int *p0, int p1, uint p2, int p3, uint p4);
void FUN_00575880(int *p0, int p1, int p2, int p3);
int FUN_00575c60(int *p0, int *p1, int p2, void *p3, void *p4, int *p5);
uint FUN_00576640(int *p0, int p1, int p2, int p3, int p4);
// MATCH: 0x00568990 FUN_00568990
uint FUN_00568990(int param_1,int param_2,uint param_3,int param_4,uint param_5)

{
  int *puVar1;
  int *piVar2;
  int uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  short *psVar8;
  uint uVar9;
  int *piVar10;
  int *puVar11;
  uint local_54;
  uint local_50;
  uint local_48;
  char local_40 [32];
  char local_20 [32];
  
  local_54 = 0;
  if ((param_3 == 0) || (param_5 == 0)) {
    return 0;
  }
  piVar10 = (int *)(param_1 + 0x37c);
  *(int *)(param_1 + 0x3ec) = *(int *)(*piVar2 + 0xc014);
  *(int *)(param_1 + 0x3f0) = piVar2[3];
  piVar2 = (int *)**(int **)(param_1 + 0x70);
  uVar3 = *(int *)(*piVar2 + 0xc018);
  local_50 = 0;
  uVar7 = 0;
  if (param_3 != 0) {
    do {
      uVar7 = local_50;
      if (param_5 <= local_54) {
        return local_54;
      }
      *(undefined4 *)(param_1 + 0x3ac) = 0;
      *(undefined4 *)(param_1 + 0x3b8) = 0;
      *(undefined4 *)(param_1 + 0x3f4) = 0;
      if (local_50 < param_3) {
        puVar11 = (int *)(param_2 + 8 + local_50 * 0x14);
        do {
          if (*(int *)(param_1 + 0x3f4) != 0) break;
          puVar1 = puVar11 + -2;
          if (*(short *)(puVar11 + -1) == -1) {
            FUN_0055bd80(*puVar1,0,0,local_40);
          }
          else {
            FUN_0055abb0(piVar2,*puVar1,local_40);
          }
          if (*(short *)(puVar11 + 1) == -1) {
            FUN_0055bd80(*puVar11,0,0,local_20);
          }
          else {
            FUN_0055abb0(piVar2,*puVar11,local_20);
          }
          *(undefined4 *)(param_1 + 0x380) = 0;
          *(undefined4 *)(param_1 + 0x398) = 0;
          *(undefined4 *)(param_1 + 0x38c) = 0;
          *(undefined4 *)(param_1 + 0x3d8) = 0;
          iVar4 = FUN_00575c60(piVar10,puVar1,0,local_40,local_20,puVar11);
          if ((iVar4 != 0) &&
             (iVar5 = FUN_00575c60(piVar10,puVar11,0,local_20,local_40,puVar1), iVar5 != 0)) {
            uVar6 = FUN_00576640(piVar10,iVar4,iVar5,puVar11[2],uVar3);
            if ((*(int *)(param_1 + 0x3f4) != 0) && (uVar7 < local_50)) break;
            uVar9 = 0;
            if (uVar6 != 0) {
              do {
                FUN_00575880(piVar10,param_1,*piVar10 + uVar9 * 8,*(int *)(param_1 + 0x3ec));
                uVar9 = uVar9 + 1;
              } while (uVar9 < uVar6);
            }
          }
          local_50 = local_50 + 1;
          puVar11 = puVar11 + 5;
        } while (local_50 < param_3);
      }
      uVar7 = *(uint *)(param_1 + 0x3ac);
      iVar4 = *(int *)(param_1 + 0x3a8);
      local_48 = 0;
      if (uVar7 != 0) {
        do {
          if (param_5 <= local_54) break;
          uVar6 = 1;
          if (1 < uVar7 - local_48) {
            psVar8 = (short *)(iVar4 + 0x17c);
            do {
              if ((((*(short *)(iVar4 + 0x5c) != -1) || (*psVar8 != -1)) &&
                  ((*(int *)(iVar4 + 0x58) != *(int *)(psVar8 + -2) ||
                   (*(short *)(iVar4 + 0x5c) != *psVar8)))) ||
                 (((*(short *)(iVar4 + 0xdc) != -1 || (psVar8[0x40] != -1)) &&
                  ((*(int *)(iVar4 + 0xd8) != *(int *)(psVar8 + 0x3e) ||
                   (*(short *)(iVar4 + 0xdc) != psVar8[0x40])))))) break;
              uVar6 = uVar6 + 1;
              psVar8 = psVar8 + 0x90;
            } while (uVar6 < uVar7 - local_48);
          }
          iVar5 = FUN_00575560(piVar10,iVar4,uVar6,local_54 * 0xe0 + param_4,param_5 - local_54);
          local_54 = local_54 + iVar5;
          iVar4 = iVar4 + uVar6 * 0x120;
          local_48 = local_48 + uVar6;
          uVar7 = *(uint *)(param_1 + 0x3ac);
        } while (local_48 < uVar7);
      }
      uVar7 = local_54;
    } while (local_50 < param_3);
  }
  return uVar7;
}
