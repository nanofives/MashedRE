// FLAGS: /O2 /Ob1 /G5
// h3: 25.8% regs 60.3% (raw converter form, flags only). cxx FAIL: FUN_004cdca0/FUN_004cdd60 prototypes need pointer types (uint*), not touched in time; class (b).
/* MATCH-RWL03 0x0053e830: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
#define CONCAT22(a,b) ((uint)(((uint)(ushort)(a)<<16)|(ushort)(b)))
undefined4 * FUN_004c5a00(undefined4 p0);
int FUN_004c5ae0(int p0, int p1);
int FUN_004c77c0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
undefined4 * FUN_004cdca0(undefined4 p0, undefined4 p1, undefined4 p2);
undefined4 FUN_004cdd00(uint * p0);
uint * FUN_004cdd60(uint * p0);
int FUN_004cde50(int p0);
uint * FUN_004cdf20(uint * p0, int p1);
uint * FUN_004d52d0(uint * p0, int p1);
int FUN_004d5310(int p0, byte * p1);
undefined4 FUN_004d5340(undefined4 p0, undefined4 p1, undefined4 * p2, undefined4 * p3, undefined4 * p4, uint * p5);
uint * FUN_004dabd0(uint * p0, uint * p1);
// MATCH: 0x0053e830 FUN_0053e830
int FUN_0053e830(int *param_1,int *param_2)

{
  char cVar7;
  uint uVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;
  int iVar8;
  int iVar9;
  uint uVar10;
  char *pcVar11;
  int *piVar12;
  undefined4 *puVar13;
  int local_38;
  uint local_34;
  int local_30;
  int local_2c;
  int *local_28 [2];
  char local_20;
  undefined4 local_1f;
  
  iVar5 = *param_2;
  local_38 = 0;
  local_28[0] = *(int **)(iVar5 + 0x10);
  local_2c = *(int *)(iVar5 + 0xc);
  local_30 = iVar5;
  uVar1 = FUN_004cdca0(local_2c,local_28[0],0x20);
  local_34 = uVar1;
  FUN_004cdd60(uVar1);
  FUN_004d52d0(uVar1,iVar5);
  if (param_1 == (int *)0x0) {
    piVar12 = *(int **)(iVar5 + 0x10);
    iVar9 = *(int *)(iVar5 + 0xc);
    iVar2 = FUN_004cdca0(iVar9,piVar12,0x20);
    FUN_004cdd60(iVar2);
    iVar8 = 0;
    if (0 < (int)piVar12) {
      do {
        iVar3 = 0;
        if (0 < iVar9) {
          do {
            iVar5 = iVar3 * 4;
            iVar3 = iVar3 + 1;
            *(undefined4 *)(*(int *)(iVar2 + 0x10) * iVar8 + iVar5 + *(int *)(iVar2 + 0x14)) =
                 0xffffffff;
            iVar5 = local_30;
          } while (iVar3 < iVar9);
        }
        iVar8 = iVar8 + 1;
      } while (iVar8 < (int)piVar12);
    }
  }
  else {
    local_38 = *param_1;
    piVar12 = *(int **)(local_38 + 0x10);
    iVar9 = *(int *)(local_38 + 0xc);
    iVar2 = FUN_004cdca0(iVar9,piVar12,0x20);
    FUN_004cdd60(iVar2);
    FUN_004d52d0(iVar2,local_38);
  }
  if ((iVar9 != local_2c) || (uVar1 = local_34, piVar12 != local_28[0])) {
    uVar1 = FUN_004cdca0(iVar9,piVar12,0x20);
    FUN_004cdd60(uVar1);
    uVar6 = local_34;
    FUN_004dabd0(uVar1,local_34);
    FUN_004cdd00(uVar6);
  }
  FUN_004cde50(uVar1);
  FUN_004cdf20(iVar2,uVar1);
  FUN_004d5340(iVar2,4,&local_30,&local_2c,local_28,&local_34);
  if (param_1 == (int *)0x0) {
    cVar7 = *(char *)(iVar5 + 0x23);
  }
  else {
    cVar7 = *(char *)(local_38 + 0x23);
  }
  if (cVar7 < '\0') {
    local_34 = local_34 | 0x9000;
  }
  uVar4 = FUN_004c77c0(local_30,local_2c,local_28[0],local_34);
  FUN_004d5310(uVar4,iVar2);
  iVar5 = FUN_004c5a00(uVar4);
  if (param_1 == (int *)0x0) {
    uVar6 = param_2[0x14];
  }
  else {
    uVar6 = param_1[0x14];
  }
  uVar10 = uVar6 >> 0xc & 0xf;
  uVar10 = ~-(uint)((uVar6 >> 8 & 0xf) != uVar10) & uVar10;
  *(uint *)(iVar5 + 0x50) =
       (uVar10 << 4 | uVar10) << 8 |
       CONCAT22((short)((uint)*(undefined4 *)(iVar5 + 0x50) >> 0x10),
                (ushort)(byte)*(undefined4 *)(iVar5 + 0x50));
  *(char *)(iVar5 + 0x50) = (char)uVar6;
  FUN_004cdd00(iVar2);
  FUN_004cdd00(uVar1);
  local_20 = '\0';
  puVar13 = &local_1f;
  for (iVar9 = 7; iVar9 != 0; iVar9 = iVar9 + -1) {
    *puVar13 = 0;
    puVar13 = puVar13 + 1;
  }
  *(undefined2 *)puVar13 = 0;
  *(undefined1 *)((int)puVar13 + 2) = 0;
  iVar9 = 0;
  pcVar11 = &local_20;
  if (param_1 == (int *)0x0) {
    local_28[1] = param_2 + 4;
    local_28[0] = param_2 + 4;
  }
  else {
    local_28[0] = param_1 + 4;
    local_28[1] = param_2 + 4;
  }
  do {
    if (((char)*local_28[0] == '\0') && ((char)*local_28[1] == '\0')) break;
    iVar2 = 0;
    do {
      piVar12 = local_28[iVar2];
      cVar7 = (char)*piVar12;
      if (cVar7 != '\0') {
        *pcVar11 = cVar7;
        pcVar11 = pcVar11 + 1;
        iVar9 = iVar9 + 1;
        local_28[iVar2] = (int *)((int)piVar12 + 1);
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < 2);
  } while (iVar9 < 0x1e);
  *pcVar11 = '\0';
  FUN_004c5ae0(iVar5,&local_20);
  return iVar5;
}
