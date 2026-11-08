// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055a9a0 (h2): raw form, 14.7% after fixing callee FUN_00564040 return type (was a compile failure); not worked, class (b). */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
uint FUN_00564040(int p0, ushort p1, int p2, int p3, int p4);
// MATCH: 0x0055a9a0 FUN_0055a9a0
int FUN_0055a9a0(uint param_1,int param_2,uint param_3,int param_4,int param_5,uint param_6)

{
  uint *puVar1;
  ushort uVar2;
  ushort uVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  int iVar10;
  int local_10;
  uint local_8;
  
  iVar5 = param_1;
  local_10 = 0;
  if (param_6 == 0) {
    return 0;
  }
  uVar6 = 0;
  if (*(int *)(param_1 + 4) != 0) {
    do {
      uVar6 = uVar6 + 1;
      *(undefined4 *)(*(int *)(param_1 + 0x70) + -4 + uVar6 * 4) = 0;
    } while (uVar6 < *(uint *)(param_1 + 4));
  }
  local_8 = 0;
  if (param_3 != 0) {
    do {
      uVar2 = *(ushort *)(*(int *)(param_2 + local_8 * 4) + 0x20);
      uVar6 = FUN_00564040(*(undefined4 *)(iVar5 + 0x54),uVar2,*(undefined4 *)(iVar5 + 0x80),
                           *(undefined4 *)(iVar5 + 0x70),*(uint *)(iVar5 + 8) & 1);
      puVar1 = (uint *)(*(int *)(iVar5 + 0x70) + (uint)(uVar2 >> 5) * 4);
      param_1 = 0;
      *puVar1 = *puVar1 | 1 << ((byte)uVar2 & 0x1f);
      if (uVar6 != 0) {
        puVar8 = (undefined4 *)(param_4 + 8 + local_10 * 0x14);
        do {
          uVar3 = *(ushort *)(*(int *)(iVar5 + 0x80) + param_1 * 2);
          uVar7 = 1 << ((byte)uVar3 & 0x1f);
          iVar10 = (uint)(uVar3 >> 5) * 4;
          puVar9 = puVar8;
          if (((*(uint *)(iVar10 + *(int *)(iVar5 + 0x70)) & uVar7) == 0) &&
             ((param_6 == 0xf ||
              ((param_6 &
               2 - (uint)((*(uint *)(iVar10 + *(int *)(iVar5 + 100)) & uVar7) != 0) <<
               (('\x01' - ((*(uint *)(iVar10 + *(int *)(iVar5 + 0x5c)) & uVar7) != 0)) * '\x02' &
               0x1fU)) != 0)))) {
            puVar9 = puVar8 + 5;
            puVar8[-2] = *(undefined4 *)(*(int *)(iVar5 + 0x50) + (uint)uVar2 * 4);
            *(undefined2 *)(puVar8 + -1) = 0x8000;
            uVar4 = *(undefined4 *)(*(int *)(iVar5 + 0x50) + (uint)uVar3 * 4);
            *(undefined2 *)(puVar8 + 1) = 0x8000;
            *puVar8 = uVar4;
            puVar8[2] = 0;
            local_10 = local_10 + 1;
            if (local_10 == param_5) {
              return local_10;
            }
          }
          param_1 = param_1 + 1;
          puVar8 = puVar9;
        } while (param_1 < uVar6);
      }
      local_8 = local_8 + 1;
    } while (local_8 < param_3);
  }
  return local_10;
}
