// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00544ad0 (h2): 46.0% (regs 68.0, ops 84). Typed float DAT, int-returning callees; flags /O2 /Ob1 /G5 beat /O1 (raw 44). Residue: dirty-flag blocks (flags|=3 with ebx=3 hoisted constant, test al,4) and global load order differ; structure only. class (b). */
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
MGLOBAL(int, DAT_007dc66c, 0x007dc66c);
MGLOBAL(int, DAT_007dc670, 0x007dc670);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
void FUN_004c0e50(int p0);
int FUN_004d7ff0(int p0);
void FUN_004d8480(undefined4 * p0);
// MATCH: 0x00544ad0 FUN_00544ad0
int FUN_00544ad0(int param_1,int param_2)
{
  int *piVar1;
  int iVar2;
  float fVar3;
  int iVar4;
  uint uVar5;
  undefined4 local_8;
  undefined4 local_4;

  iVar4 = *(int *)(param_1 + 0x18);
  if (iVar4 != 0) {
    piVar1 = (int *)(DAT_007dc66c + param_1);
    iVar2 = DAT_007dc670 + iVar4;
    if (piVar1[2] != iVar4) {
      piVar1[2] = iVar4;
    }
    iVar4 = *(int *)(iVar2 + 8) + param_2 * 0x14;
    *piVar1 = iVar4;
    *(undefined2 *)(param_1 + 0x50) = *(undefined2 *)(iVar4 + 4);
    uVar5 = *(uint *)(param_1 + 0x4c);
    *(uint *)(param_1 + 0x4c) = uVar5 | 3;
    if (((uVar5 & 4) == 0) && (*(int *)(param_1 + 4) != 0)) {
      FUN_004c0e50(*(int *)(param_1 + 4));
    }
    *(undefined2 *)(param_1 + 0x52) = *(undefined2 *)(*piVar1 + 6);
    uVar5 = *(uint *)(param_1 + 0x4c);
    *(uint *)(param_1 + 0x4c) = uVar5 | 3;
    if (((uVar5 & 4) == 0) && (*(int *)(param_1 + 4) != 0)) {
      FUN_004c0e50(*(int *)(param_1 + 4));
    }
    uVar5 = *(uint *)(param_1 + 0x4c);
    *(undefined4 *)(param_1 + 0x5c) = 0;
    *(uint *)(param_1 + 0x4c) = uVar5 | 3;
    if (((uVar5 & 4) == 0) && (*(int *)(param_1 + 4) != 0)) {
      FUN_004c0e50(*(int *)(param_1 + 4));
    }
    uVar5 = *(uint *)(param_1 + 0x4c);
    fVar3 = *(float *)(*piVar1 + 8);
    *(uint *)(param_1 + 0x4c) = uVar5 | 3;
    *(float *)(param_1 + 0x54) = fVar3;
    *(float *)(param_1 + 0x58) = DAT_005cc320 / fVar3;
    if (((uVar5 & 4) == 0) && (*(int *)(param_1 + 4) != 0)) {
      FUN_004c0e50(*(int *)(param_1 + 4));
    }
    return param_1;
  }
  local_8 = 0x105;
  local_4 = FUN_004d7ff0(1);
  FUN_004d8480(&local_8);
  return 0;
}
