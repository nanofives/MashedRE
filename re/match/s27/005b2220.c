// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b2220: 18.0% (regs 40.9%). Raw converter body kept; file did not compile (LPLONG undefined) so LPLONG p2 -> LONG *p2 and the (LPLONG) casts -> (LONG *). Residual (b): 1512 B, original frame 0x80 vs 0x98 here, register allocation and local layout not reproduced; no structural pass made (budget).  C++ fixes: local_64 is uint[1], piVar1 is undefined4 *. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef unsigned __int64 ulonglong;
typedef void *HANDLE;
typedef int BOOL;
typedef unsigned long DWORD;
typedef long LONG;
#define CONCAT22(a,b) ((uint)(((uint)(ushort)(a)<<16)|(ushort)(b)))
#define CONCAT31(a,b) ((uint)(((uint)(a)<<8)|(uchar)(b)))
void FUN_005aeda0(uint p0, uint p1, uint * p2, uint * p3);
__declspec(dllimport) BOOL __stdcall ReleaseSemaphore(HANDLE p0, LONG p1, LONG *p2);
__declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE p0, DWORD p1);
/* converter noise: extern int LPLONG; dropped (no address) */
// MATCH: 0x005b2220 FUN_005b2220
int FUN_005b2220(int param_1)

{
  undefined4 *piVar1;
  undefined1 uVar2;
  ushort uVar3;
  undefined2 uVar4;
  undefined4 uVar5;
  undefined4 *puVar6;
  uint uVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  uint uVar10;
  undefined4 *puVar11;
  short sVar12;
  int iVar13;
  uint uVar14;
  undefined4 *puVar15;
  uint uVar16;
  int iVar17;
  int iVar18;
  uint uVar19;
  uint uVar20;
  uint local_7c;
  undefined4 local_78;
  int local_74;
  undefined4 *local_70;
  uint local_6c;
  int local_68;
  uint local_64 [1];
  uint local_60;
  uint local_5c;
  uint local_58;
  uint local_54;
  int local_50;
  uint local_4c;
  int local_48;
  uint local_44;
  int local_40;
  uint local_3c;
  undefined4 *local_38;
  int local_34;
  int local_30;
  int local_2c;
  undefined4 local_28;
  undefined4 local_24;
  int local_20;
  int local_1c;
  int local_18;
  uint local_14;
  undefined4 *local_10;
  uint local_c;
  uint local_8;
  int local_4;
  
  iVar18 = 0;
  local_2c = *(int *)(param_1 + 0x118);
  local_30 = 0;
  local_4 = *(int *)(local_2c + 0x20);
  uVar16 = *(uint *)(local_2c + 0x14);
  iVar17 = local_4;
  local_14 = uVar16;
  do {
    if ((*(byte *)(*(int *)(param_1 + 0x148) + 0x10 + *(int *)(param_1 + 0x15c) * 0x14) & 2) == 0) {
      return iVar18;
    }
    local_20 = *(int *)(param_1 + 0x15c) * iVar17 + *(int *)(param_1 + 0x144);
    local_34 = *(int *)(param_1 + 0x174) * iVar17 + *(int *)(param_1 + 0x168);
    local_38 = (undefined4 *)(*(int *)(param_1 + 0x148) + *(int *)(param_1 + 0x15c) * 0x14);
    local_28 = local_38[3];
    local_10 = (undefined4 *)(*(int *)(param_1 + 0x16c) + *(int *)(param_1 + 0x174) * 0x14);
    local_24 = local_10[3];
    local_60 = 0;
    iVar13 = iVar17;
    if (uVar16 != 0) {
      local_3c = uVar16 - 1;
      local_50 = 0;
      do {
        uVar14 = *(int *)(local_2c + 0x18) + local_50;
        local_74 = CONCAT22((short)((uint)iVar17 >> 0x10),*(ushort *)(uVar14 + 0x18));
        local_4c = CONCAT31(((*(uint *)((char *)&local_4c + 1)) & 0xffffff),*(undefined1 *)(*(int *)(uVar14 + 0x14) + 0xd));
        uVar3 = *(ushort *)(uVar14 + 0x1a);
        local_40 = CONCAT22((short)((uint)*(int *)(uVar14 + 0x14) >> 0x10),uVar3);
        uVar16 = (uint)uVar3;
        if ((uVar3 & 1) == 0) {
          local_78 = 2;
          uVar19 = (uint)(uVar3 >> 1);
        }
        else if ((uVar3 & 3) == 0) {
          local_78 = 3;
          uVar19 = (uint)(uVar3 >> 2);
        }
        else if ((uVar3 & 7) == 0) {
          local_78 = 4;
          uVar19 = (uint)(uVar3 >> 3);
        }
        else if ((uVar3 & 0xf) == 0) {
          local_78 = 5;
          uVar19 = (uint)(uVar3 >> 4);
        }
        else {
          local_78 = 1;
          uVar19 = uVar16;
        }
        local_5c = (uint)*(ushort *)(uVar14 + 0x18);
        uVar10 = *(ushort *)(*(int *)(local_2c + 0x18) + 0x1c + local_50) * local_5c;
        local_44 = uVar14;
        FUN_005aeda0(local_28,uVar10,&local_54,local_64);
        FUN_005aeda0(local_24,uVar10,&local_58,local_64);
        uVar7 = uVar10;
        if ((*(byte *)(local_38 + 4) & 0x10) != 0) {
          FUN_005aeda0(*(undefined4 *)(param_1 + 0xec),uVar10,&local_7c,local_64);
          uVar7 = (((uVar16 - 1) + local_7c) / uVar16) * uVar16;
        }
        uVar16 = local_14;
        puVar6 = local_38;
        uVar20 = (uint)*(ushort *)(uVar14 + 0x1a);
        local_54 = (((uVar20 - 1) + local_54) / uVar20) * uVar20;
        uVar14 = (uint)*(ushort *)(uVar14 + 0x1a);
        local_58 = (((uVar14 - 1) + local_58) / uVar14) * uVar14;
        local_6c = uVar7 - local_54;
        if (uVar10 - local_58 < uVar7 - local_54) {
          local_6c = uVar10 - local_58;
        }
        if (local_3c == local_60) {
          local_8 = (uint)(uVar7 == local_6c + local_54);
          local_c = (uint)(uVar10 == local_6c + local_58);
        }
        uVar20 = local_54 - (local_54 / local_5c) * local_74 & 0xffff;
        local_3c = local_4c & 0xff;
        uVar14 = local_58 / local_5c & 0xffff;
        iVar17 = local_58 - uVar14 * local_5c;
        local_48 = (local_3c - 1) * local_5c;
        local_70 = (undefined4 *)
                   ((local_54 / local_5c & 0xffff) * local_3c * local_5c + *(int *)(local_44 + 0x24)
                    + uVar20 + local_20);
        puVar15 = (undefined4 *)
                  (local_3c * uVar14 * local_5c + *(int *)(local_44 + 0x24) + iVar17 + local_34);
        if ((char)local_4c != '\0') {
          local_1c = local_74 - uVar20;
          local_18 = local_74 - iVar17;
          do {
            sVar12 = (short)local_6c;
            iVar17 = local_1c;
            local_68 = local_18;
            local_44 = local_6c;
            puVar11 = puVar15;
            puVar8 = local_70;
            while (sVar12 != 0) {
              if ((short)iVar17 == 0) {
                puVar8 = (undefined4 *)((int)puVar8 + local_48);
                iVar17 = local_74;
              }
              if ((short)local_68 == 0) {
                local_68 = local_74;
                puVar11 = (undefined4 *)((int)puVar11 + local_48);
              }
              switch(local_78) {
              case 1:
                uVar14 = 0;
                if (uVar19 != 0) {
                  do {
                    uVar2 = *(undefined1 *)puVar8;
                    puVar8 = (undefined4 *)((int)puVar8 + 1);
                    *(undefined1 *)puVar11 = uVar2;
                    puVar11 = (undefined4 *)((int)puVar11 + 1);
                    uVar14 = uVar14 + 1;
                  } while ((uVar14 & 0xffff) < uVar19);
                }
                break;
              case 2:
                uVar14 = 0;
                if (uVar19 != 0) {
                  do {
                    uVar4 = *(undefined2 *)puVar8;
                    puVar8 = (undefined4 *)((int)puVar8 + 2);
                    *(undefined2 *)puVar11 = uVar4;
                    puVar11 = (undefined4 *)((int)puVar11 + 2);
                    uVar14 = uVar14 + 1;
                  } while ((uVar14 & 0xffff) < uVar19);
                }
                break;
              case 3:
                uVar14 = 0;
                if (uVar19 != 0) {
                  do {
                    uVar5 = *puVar8;
                    puVar8 = puVar8 + 1;
                    *puVar11 = uVar5;
                    puVar11 = puVar11 + 1;
                    uVar14 = uVar14 + 1;
                  } while ((uVar14 & 0xffff) < uVar19);
                }
                break;
              case 4:
                uVar14 = 0;
                puVar9 = puVar8;
                if (uVar19 != 0) {
                  do {
                    puVar8 = puVar9 + 2;
                    *puVar11 = *puVar9;
                    puVar11[1] = puVar9[1];
                    puVar11 = puVar11 + 2;
                    uVar14 = uVar14 + 1;
                    puVar9 = puVar8;
                  } while ((uVar14 & 0xffff) < uVar19);
                }
                break;
              case 5:
                uVar14 = 0;
                puVar9 = puVar8;
                if (uVar19 != 0) {
                  do {
                    *puVar11 = *puVar9;
                    puVar11[1] = puVar9[1];
                    puVar8 = puVar9 + 4;
                    puVar11[2] = puVar9[2];
                    puVar11[3] = puVar9[3];
                    puVar11 = puVar11 + 4;
                    uVar14 = uVar14 + 1;
                    puVar9 = puVar8;
                  } while ((uVar14 & 0xffff) < uVar19);
                }
              }
              local_68 = local_68 - local_40;
              iVar17 = iVar17 - local_40;
              local_44 = local_44 - local_40;
              sVar12 = (short)local_44;
            }
            uVar14 = local_5c;
            if ((ushort)local_74 < (ushort)local_40) {
              uVar14 = local_6c;
            }
            local_70 = (undefined4 *)((int)local_70 + uVar14);
            puVar15 = (undefined4 *)((int)puVar15 + uVar14);
            local_3c = local_3c - 1;
          } while (local_3c != 0);
        }
        local_3c = local_14 - 1;
        local_7c = uVar7;
        if (local_3c == local_60) {
          *local_10 = *local_38;
          local_10[1] = local_38[1];
          local_10[2] = *(undefined4 *)(param_1 + 0xf0);
          if (local_c == 0) {
            piVar1 = local_10 + 3;
            FUN_005aeda0(local_6c + local_58,(int)(0xffffffff / (ulonglong)uVar10),local_64,piVar1);
            *piVar1 = *piVar1 + 1;
          }
          else {
            local_10[3] = 0;
            local_10[4] = 2;
            local_30 = local_34;
            *(int *)(param_1 + 0x174) = *(int *)(param_1 + 0x174) + 1;
            if (*(int *)(param_1 + 0x174) == *(int *)(param_1 + 0x17c)) {
              *(undefined4 *)(param_1 + 0x174) = 0;
            }
          }
          if (local_8 == 0) {
            piVar1 = puVar6 + 3;
            FUN_005aeda0(local_6c + local_54,(int)(0xffffffff / (ulonglong)uVar10),local_64,piVar1);
            *piVar1 = *piVar1 + 1;
          }
          else {
            WaitForSingleObject(*(HANDLE *)(param_1 + 0x18c),0xffffffff);
            puVar6[3] = 0;
            puVar6[4] = 1;
            iVar18 = *(int *)(param_1 + 0x15c);
            iVar17 = iVar18 + 1;
            if (iVar17 == *(int *)(param_1 + 0x164)) {
              iVar17 = 0;
            }
            *(int *)(param_1 + 0x15c) = iVar17;
            if (*(int *)(param_1 + 0x154) == iVar18) {
              *(int *)(param_1 + 0x154) = iVar17;
              *(int *)(param_1 + 0x158) = iVar17;
            }
            ReleaseSemaphore(*(HANDLE *)(param_1 + 0x18c),1,(LONG *)0x0);
          }
        }
        local_60 = local_60 + 1;
        iVar17 = local_50 + 0x28;
        iVar13 = local_4;
        iVar18 = local_30;
        local_50 = iVar17;
      } while (local_60 < uVar16);
    }
    iVar17 = iVar13;
  } while (iVar18 == 0);
  return iVar18;
}
