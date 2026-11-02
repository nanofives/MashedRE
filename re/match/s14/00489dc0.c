// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00489dc0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs14_20261102/results.tsv. */
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
typedef int code();
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007067ec, 0x007067ec);
MGLOBAL(int, DAT_007dc57c, 0x007dc57c);
MGLOBAL(undefined1, DAT_007067b8, 0x007067b8);
MGLOBAL(undefined1, DAT_007067dc, 0x007067dc);
MGLOBAL(undefined1, DAT_007067fc, 0x007067fc);
void FUN_00489da0(int p0);
int FUN_004c0b30(void);
int FUN_004e7e30(int p0, undefined4 p1);
int * FUN_004e8090(int * p0, int p1);
undefined4 FUN_00534b60(undefined4 p0, uint p1, undefined4 p2);
undefined4 FUN_00535700(int p0, int * p1, uint p2, uint p3);
// MATCH: 0x00489dc0 FUN_00489dc0
void FUN_00489dc0(int param_1,int param_2)

{
  uint *puVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  undefined1 *puStack_30;
  int iStack_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  uVar2 = FUN_004c0b30();
  local_10 = 0;
  local_c = 0;
  local_8 = 0x3f800000;
  local_4 = 0x3f800000;
  local_28 = 0x3dcccccd;
  local_24 = 0x3dcccccd;
  if (*(int *)(&DAT_007067fc + param_1 * 4) == 0) {
    uVar3 = (**(code **)(DAT_007d3ff8 + 0x108))(0x4000,0);
    *(undefined4 *)(&DAT_007067fc + param_1 * 4) = uVar3;
  }
  if (*(int *)(&DAT_007067dc + param_1 * 4) == 0) {
    local_20 = 0;
    local_1c = 0;
    local_18 = 0;
    local_14 = 0x447a0000;
    DAT_007067ec = param_2;
    iVar4 = FUN_00534b60(0x200,0x80007,0);
    iVar5 = DAT_007dc57c;
    *(int *)(&DAT_007067dc + param_1 * 4) = iVar4;
    *(undefined4 *)(*(int *)(iVar4 + iVar5) + 0xa4) = 5;
    *(undefined4 *)(*(int *)(DAT_007dc57c + *(int *)(&DAT_007067dc + param_1 * 4)) + 0xa8) = 6;
    puVar1 = (uint *)(*(int *)(DAT_007dc57c + *(int *)(&DAT_007067dc + param_1 * 4)) + 0x40);
    *puVar1 = *puVar1 | 0x10000000;
    puVar1 = (uint *)(*(int *)(DAT_007dc57c + *(int *)(&DAT_007067dc + param_1 * 4)) + 0x40);
    *puVar1 = *puVar1 | 0x800000;
    *(undefined4 *)(*(int *)(DAT_007dc57c + *(int *)(&DAT_007067dc + param_1 * 4)) + 4) = 0x200;
    FUN_004e7e30(*(undefined4 *)(&DAT_007067dc + param_1 * 4),uVar2);
    iVar5 = *(int *)(*(int *)(*(int *)(&DAT_007067dc + param_1 * 4) + 0x18) + 0x5c);
    *(undefined4 *)(iVar5 + 4) = local_20;
    *(undefined4 *)(iVar5 + 8) = local_1c;
    *(undefined4 *)(iVar5 + 0xc) = local_18;
    *(undefined4 *)(iVar5 + 0x10) = local_14;
    *(undefined4 *)(*(int *)(DAT_007dc57c + *(int *)(&DAT_007067dc + param_1 * 4)) + 0xac) = 1;
    iVar5 = *(int *)(DAT_007dc57c + *(int *)(&DAT_007067dc + param_1 * 4));
    *(undefined4 *)(iVar5 + 0xd8) = local_10;
    *(undefined4 *)(iVar5 + 0xdc) = local_c;
    *(undefined4 *)(iVar5 + 0xe0) = local_8;
    *(undefined4 *)(iVar5 + 0xe4) = local_4;
    *(uint *)(*(int *)(DAT_007dc57c + *(int *)(&DAT_007067dc + param_1 * 4)) + 0x40) =
         *(uint *)(*(int *)(DAT_007dc57c + *(int *)(&DAT_007067dc + param_1 * 4)) + 0x40) | 0x80000;
    iVar5 = *(int *)(DAT_007dc57c + *(int *)(&DAT_007067dc + param_1 * 4));
    *(undefined4 *)(iVar5 + 0xb8) = local_28;
    *(undefined4 *)(iVar5 + 0xbc) = local_24;
    puVar1 = (uint *)(*(int *)(DAT_007dc57c + *(int *)(&DAT_007067dc + param_1 * 4)) + 0x40);
    *puVar1 = *puVar1 | 0x4000;
    iVar5 = DAT_007067ec;
    if (DAT_007067ec != 0) {
      *(undefined1 *)(DAT_007067ec + 0x50) = 2;
      FUN_004e8090(**(undefined4 **)(*(int *)(*(int *)(&DAT_007067dc + param_1 * 4) + 0x18) + 0x20),
                   iVar5);
    }
    FUN_00535700(*(undefined4 *)(&DAT_007067dc + param_1 * 4),&puStack_30,2,0x40000000);
    iVar5 = 0x200;
    do {
      *puStack_30 = 0xff;
      puStack_30[1] = 0xff;
      puStack_30[2] = 0xff;
      puStack_30[3] = 0xff;
      puStack_30 = puStack_30 + iStack_2c;
      iVar5 = iVar5 + -1;
    } while (iVar5 != 0);
  }
  *(undefined4 *)(&DAT_007067b8 + param_1 * 4) = 0;
  FUN_00489da0(param_1);
  return;
}
