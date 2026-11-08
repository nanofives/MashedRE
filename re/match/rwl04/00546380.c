// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00546380: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef undefined1 *(*PFN_al)(int,int);
typedef void (*PFN_fr2)(undefined1 *);
void *__cdecl memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_004d7ff0(undefined4 p0, ...);
undefined4 * FUN_004d8480(undefined4 * p0);
int FUN_00545340(int p0, int p1);
// MATCH: 0x00546380 FUN_00546380
undefined1 * FUN_00546380(int param_1,int param_2,undefined4 *param_3)
{
  undefined1 *puVar1;
  undefined4 *puVar2;
  uint uVar3;
  int iVar4;
  undefined4 local_8[2];
  switch (param_2) {
  case 1:
    iVar4 = param_1 * 0xc + 0x50;
    puVar1 = (undefined1 *)(**(PFN_al *)(DAT_007d3ff8 + 0x108))(iVar4,0x3000c);
    if (puVar1 == (undefined1 *)0x0) {
      local_8[0] = 0x102;
      local_8[1] = FUN_004d7ff0(0x80000013,iVar4);
      FUN_004d8480(local_8);
      return (undefined1 *)0x0;
    }
    iVar4 = param_1 + 2;
    break;
  case 2:
    iVar4 = param_1 * 0xc + 0x5c;
    puVar1 = (undefined1 *)(**(PFN_al *)(DAT_007d3ff8 + 0x108))(iVar4,0x3000c);
    if (puVar1 == (undefined1 *)0x0) {
      local_8[0] = 0x102;
      local_8[1] = FUN_004d7ff0(0x80000013,iVar4);
      FUN_004d8480(local_8);
      return (undefined1 *)0x0;
    }
    iVar4 = param_1 + 3;
    break;
  default:
    local_8[0] = 0x102;
    local_8[1] = FUN_004d7ff0(1);
    FUN_004d8480(local_8);
    return (undefined1 *)0x0;
  }
  uVar3 = param_1 * 0xc;
  *(int *)(puVar1 + 8) = iVar4;
  *(undefined4 *)(puVar1 + 0x10) = 0;
  puVar1[0x18] = 0;
  *(int *)(puVar1 + 0xc) = param_2;
  *puVar1 = 0xc;
  iVar4 = FUN_00545340((int)puVar1,(int)param_3);
  if (iVar4 == 0) {
    (**(PFN_fr2 *)(DAT_007d3ff8 + 0x10c))(puVar1);
    return (undefined1 *)0x0;
  }
  *puVar1 = 0xc;
  puVar2 = (undefined4 *)(**(PFN_al *)(DAT_007d3ff8 + 0x108))(uVar3,0x3000c);
  *(undefined4 **)(puVar1 + 0x14) = puVar2;
  if (puVar2 == (undefined4 *)0x0) {
    (**(PFN_fr2 *)(DAT_007d3ff8 + 0x10c))(puVar1);
    local_8[0] = 0x102;
    local_8[1] = FUN_004d7ff0(0x80000013,uVar3);
    FUN_004d8480(local_8);
    return (undefined1 *)0x0;
  }
  memcpy(puVar2,param_3,uVar3);
  return puVar1;
}
