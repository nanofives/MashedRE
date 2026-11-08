// FLAGS: /O2 /GS /Ob1
// MATCH-S27 0x005b5e00: 16.0% (regs 36.8%) with /O2 /GS /Ob1, raw Ghidra shape (C++ cast fixes only); CONCAT11 and pseudo-ops from Ghidra remain. Not iterated. Class: b.
/* MATCH-S27 0x005b5e00: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs27_20261107/results.tsv. */
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
#ifndef __cplusplus
#define true 1
#endif
#define CONCAT11(a,b) ((ushort)(((uint)(uchar)(a)<<8)|(uchar)(b)))
void FUN_005b5f70(int p0, byte * p1, int * p2, int * p3, float * p4, float * p5);
// MATCH: 0x005b5e00 FUN_005b5e00
int FUN_005b5e00(int param_1,uint param_2,undefined4 *param_3,char param_4,char param_5,int param_6)

{
  undefined2 uVar1;
  uint uVar2;
  int iVar3;
  undefined2 *puVar4;
  uint uVar5;
  undefined4 *puVar6;
  int local_60;
  int local_5c;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38 [14];
  
  uVar2 = param_2 >> 1;
  local_4c = 0;
  local_50 = 0;
  local_60 = 0;
  local_54 = 0;
  local_58 = 0;
  param_2 = 0;
  if (param_6 < 0x10) {
    return 0;
  }
  local_5c = 0x10;
  do {
    uVar5 = uVar2;
    if (0x1c < (int)uVar2) {
      uVar5 = 0x1c;
    }
    iVar3 = 0;
    if ((int)uVar5 < 1) {
LAB_005b5eae:
      puVar6 = (undefined4 *)((int)local_38 + iVar3 * 2);
      for (uVar5 = 0x1cU - iVar3 >> 1; uVar5 != 0; uVar5 = uVar5 - 1) {
        *puVar6 = 0;
        puVar6 = puVar6 + 1;
      }
      for (uVar5 = (uint)((0x1cU - iVar3 & 1) != 0); uVar5 != 0; uVar5 = uVar5 - 1) {
        *(undefined2 *)puVar6 = 0;
        puVar6 = (undefined4 *)((int)puVar6 + 2);
      }
    }
    else {
      puVar4 = (undefined2 *)(param_1 + ((int)param_5 + param_2) * 2);
      do {
        uVar1 = *puVar4;
        puVar4 = puVar4 + param_4;
        *(undefined2 *)((int)local_38 + iVar3 * 2) = uVar1;
        iVar3 = iVar3 + 1;
      } while (iVar3 < (int)uVar5);
      if (iVar3 < 0x1c) goto LAB_005b5eae;
    }
    FUN_005b5f70((int)local_38,(byte *)&local_48,(int *)&local_4c,(int *)&local_50,(float *)&local_54,(float *)&local_58);
    (*(ushort *)((char *)&local_48 + 0)) = CONCAT11(2,(undefined1)local_48);
    *param_3 = local_48;
    param_3[1] = local_44;
    local_5c = local_5c + 0x10;
    param_3[2] = local_40;
    local_60 = local_60 + 0x10;
    param_3[3] = local_3c;
    uVar2 = uVar2 + param_4 * -0x1c;
    param_2 = param_2 + param_4 * 0x1c;
    param_3 = param_3 + 4;
    if (param_6 < local_5c) {
      return local_60;
    }
  } while( true );
}
