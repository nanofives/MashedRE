// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
// h2: 45.5% via match_permute (2080 compiles; Ghidra form 43.2%). 1156 B, not rebuilt (budget). Class (b).
/* MATCH-RWL02 0x004e99b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl02_20261107/results.tsv. */
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_006189e4, 0x006189e4);
MGLOBAL(undefined4, DAT_0061864c, 0x0061864c);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined1, DAT_005cc33c, 0x005cc33c);
MGLOBAL(int, DAT_007d7278, 0x007d7278);
MGLOBAL(undefined4, DAT_007d7274, 0x007d7274);
uint FUN_004cbd30(undefined4 * p0, undefined4 * p1, uint p2);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, undefined4 * p2, uint * p3);
undefined4 FUN_004d7ff0(undefined4 p0);
int FUN_004d8000(int p0, undefined4 p1);
undefined4 * FUN_004d8480(undefined4 * p0);
int FUN_004e1b60(int p0, int p1, int p2);
uint FUN_004e1c90(uint p0, int p1, undefined4 p2, undefined4 p3);
int FUN_004e51e0(int p0);
void FUN_004e5280(undefined4 p0, undefined4 p1);
void FUN_004e5820(int p0, undefined * p1);
int * FUN_004e8090(int * p0, int p1);
undefined4 * FUN_004e9e40(undefined4 p0, int * p1, int p2, uint p3);
undefined4 * FUN_004ea220(uint p0, int * p1, int p2, uint p3);
int * FUN_004f3e90(undefined4 p0, int * p1);
undefined4 RpWorldDestroy(int p0);
int RpWorldInstance(int p0);
MGLOBAL(int, _DAT_005cc33c, 0x005cc33c);
// MATCH: 0x004e99b0 FUN_004e99b0
/* [C1 2026-05-18] Signature `undefined4 *FUN_004e99b0(undefined4 stream)`. **RpWorldStreamRead**
   (or
 */

undefined4 * FUN_004e99b0(undefined4 param_1)

{
  int iVar1;
  unsigned iVar2;
  uint uVar4;
  int *piVar8;
  uint uVar6;
  uint local_4c;
  undefined4 *puVar7;
  uint uVar5;
  int local_54;
  undefined4 *puVar9;
  undefined1 *puStack_50;
  undefined4 *puVar3;
  uint uStack_58;
  uint local_48;
  undefined4 local_44;
  int local_40;
  float fStack_3c;
  int local_30;
  float fStack_38;
  float fStack_34;
  int local_2c;
  undefined4 auStack_18 [6];
  int local_28;
  int local_24;
  int local_20;
  uint local_1c;
  
  iVar2 = FUN_004cc5e0(param_1,1,&local_54,&local_4c);
  if (iVar2 != 0) {
    if ((local_4c >= 0x35000) && (local_4c < 0x37003)) {
      piVar8 = &local_40;
      for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *piVar8 = 0;
        piVar8 = piVar8 + 1;
      }
      iVar2 = FUN_004cbd30(param_1,&local_40,local_54);
      if (iVar2 != local_54) {
        return (undefined4 *)0x0;
      }
      if (local_20 < 1) {
        uVar6 = DAT_0061864c + DAT_006189e4 * local_24 + 0x18 * local_28;
        local_48 = local_1c;
        if (!((local_1c & 0xff0000) == 0)) {
          uVar5 = local_1c >> 0x10 & 0xff;
        } else {
          if ((char)local_1c < '\0') {
            uVar5 = 2;
          }
          else {
            uVar5 = local_1c >> 2 & 1;
          }
        }
        if ((local_1c & 0x1000000) == 0) {
          iVar2 = uVar6 + local_2c * 0xc;
          if (0 != (local_1c & 0x10)) {
            iVar2 = iVar2 + local_2c * 4;
          }
          if ((local_1c & 8) != 0) {
            iVar2 = iVar2 + local_2c * 4;
          }
          if (uVar5 != 0) {
            iVar2 = iVar2 + uVar5 * local_2c * 8;
          }
          uVar6 = iVar2 + local_30 * 8;
        }
        puVar3 = (undefined4 *)(**(code **)(DAT_007d3ff8 + 0x108))(uVar6,0x3000b);
        if (puVar3 == (undefined4 *)0x0) {
          local_48 = 2;
          local_44 = FUN_004d7ff0(0x80000013);
          FUN_004d8480(&local_48);
          return (undefined4 *)0x0;
        }
        puVar7 = puVar3;
        for (uVar4 = uVar6 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
          *puVar7 = 0;
          puVar7 = puVar7 + 1;
        }
        for (uVar4 = uVar6 & 3; uVar4 != 0; uVar4 = uVar4 - 1) {
          *(undefined1 *)puVar7 = 0;
          puVar7 = (undefined4 *)((int)puVar7 + 1);
        }
        puStack_50 = (undefined1 *)(DAT_0061864c + (int)puVar3);
        *(undefined1 *)puVar3 = 7;
        *(undefined1 *)((int)puVar3 + 1) = 0;
        *(undefined1 *)((int)puVar3 + 2) = 0;
        puVar3[1] = 0;
        *(undefined1 *)((int)puVar3 + 3) = 1;
        FUN_004e5280(puVar3,uVar6);
        uVar6 = local_48;
        puVar3[2] = local_48;
        puVar3[0x11] = fStack_3c * _DAT_005cc33c;
        puVar3[3] = 2;
        puVar3[0x12] = fStack_38 * _DAT_005cc33c;
        puVar7 = puVar3 + 0xd;
        fStack_34 = fStack_34 * _DAT_005cc33c;
        puVar9 = puVar3 + 0xb;
        puVar3[8] = uVar5;
        *puVar7 = puVar7;
        puVar3[0xe] = puVar7;
        puVar7 = puVar3 + 0xf;
        puVar3[0x13] = fStack_34;
        puVar3[0xc] = puVar9;
        puVar3[0x10] = puVar7;
        puVar3[7] = 0;
        puVar3[0x1b] = 0;
        *puVar9 = puVar9;
        *puVar7 = puVar7;
        iVar2 = FUN_004cc5e0(param_1,8,0,&uStack_58);
        if (iVar2 == 0) {
          RpWorldDestroy(puVar3);
          return (undefined4 *)0x0;
        }
        if ((uStack_58 < 0x35000) || (0x37002 < uStack_58)) {
          local_44 = FUN_004d7ff0(0x80000004);
          FUN_004d8480(&local_48);
          local_48 = 2;
          RpWorldDestroy(puVar3);
          return (undefined4 *)0x0;
        }
        iVar2 = FUN_004f3e90(param_1,puVar3 + 4);
        if (iVar2 == 0) {
          RpWorldDestroy(puVar3);
          return (undefined4 *)0x0;
        }
        if (local_40 == 0) {
          iVar2 = FUN_004cc5e0(param_1,10,0,&uStack_58);
          if (iVar2 == 0) {
            RpWorldDestroy(puVar3);
            return (undefined4 *)0x0;
          }
          if ((uStack_58 < 0x35000) || (0x37002 < uStack_58)) goto LAB_004e9da1;
          iVar2 = FUN_004ea220(param_1,&puStack_50,puVar3,uVar6);
          puVar3[7] = iVar2;
          if (iVar2 == 0) {
            RpWorldDestroy(puVar3);
            return (undefined4 *)0x0;
          }
        }
        else {
          iVar2 = FUN_004cc5e0(param_1,9,0,&uStack_58);
          if (iVar2 == 0) {
            RpWorldDestroy(puVar3);
            return (undefined4 *)0x0;
          }
          if ((uStack_58 < 0x35000) || (0x37002 < uStack_58)) {
LAB_004e9da1:
            local_48 = 2;
            local_44 = FUN_004d7ff0(0x80000004);
            FUN_004d8480(&local_48);
            RpWorldDestroy(puVar3);
            return (undefined4 *)0x0;
          }
          iVar2 = FUN_004e9e40(param_1,&puStack_50,puVar3,uVar6);
          puVar3[7] = iVar2;
          if (iVar2 == 0) {
            RpWorldDestroy(puVar3);
            return (undefined4 *)0x0;
          }
        }
        puVar3[10] = puVar9;
        puVar3[9] = 0;
        puVar7 = auStack_18;
        puVar9 = puVar3 + 0x14;
        for (iVar2 = 6; iVar2 != 0; iVar2 = iVar2 + -1) {
          *puVar9 = *puVar7;
          puVar7 = puVar7 + 1;
          puVar9 = puVar9 + 1;
        }
        FUN_004e51e0(puVar3);
        FUN_004e5820(puVar3,0);
        iVar2 = 0;
        if (0 < (int)puVar3[5]) {
          do {
            piVar8 = *(int **)(puVar3[4] + iVar2 * 4);
            iVar1 = *piVar8;
            if (iVar1 != 0) {
              FUN_004e8090(piVar8,iVar1);
            }
            iVar2 = iVar2 + 1;
          } while (iVar2 < (int)puVar3[5]);
        }
        FUN_004d8000(&DAT_0061864c,puVar3);
        DAT_007d7278 = 0;
        DAT_007d7274 = 0;
        iVar2 = FUN_004e1b60(&DAT_0061864c,param_1,puVar3);
        if (iVar2 != 0) {
          if (DAT_007d7278 != 0) {
            FUN_004e1c90(&DAT_0061864c,DAT_007d7278,puVar3,DAT_007d7274);
          }
          iVar2 = RpWorldInstance(puVar3);
          if (iVar2 != 0) {
            return puVar3;
          }
          RpWorldDestroy(puVar3);
          return (undefined4 *)0x0;
        }
        RpWorldDestroy(puVar3);
        return (undefined4 *)0x0;
      }
    }
    local_48 = 2;
    local_44 = FUN_004d7ff0(0x80000004);
    FUN_004d8480(&local_48);
  }
  return (undefined4 *)0x0;
}
