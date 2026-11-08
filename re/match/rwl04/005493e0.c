// FLAGS: /O2 /GS /Ob1
/* h2 note: 64.2% (regs 70.3) raw form, flags /O2 /GS /Ob1 best (/G5 56.8). cxx FAIL: code(__cdecl*) call arity and FUN_004d7ff0 2 args (raw typing, not fixed in budget). class (b). */
/* MATCH-RWL04 0x005493e0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
undefined4 FUN_004f3bc0(undefined4 * p0);
// MATCH: 0x005493e0 FUN_005493e0
undefined4 * FUN_005493e0(uint *param_1)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  uint uVar4;
  int iVar5;
  undefined4 *puVar6;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar1 = *param_1;
  uVar1 = ((((uVar1 >> 1 & 1) + (uVar1 & 1)) * 3 + (uVar1 >> 2 & 1) + param_1[4] * 2) * param_1[1] +
           param_1[3] * 0x11 + param_1[2] * 0xb) * 4 + 0x60;
  puVar2 = (undefined4 *)(**(code **)(DAT_007d3ff8 + 0x108))(uVar1,0x30028);
  if (puVar2 == (undefined4 *)0x0) {
    uStack_8 = 0x123;
    uStack_4 = FUN_004d7ff0(0x80000013,uVar1);
    FUN_004d8480(&uStack_8);
    return (undefined4 *)0x0;
  }
  puVar3 = puVar2;
  for (uVar4 = uVar1 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
    *puVar3 = 0;
    puVar3 = puVar3 + 1;
  }
  for (iVar5 = 0; iVar5 != 0; iVar5 = iVar5 + -1) {
    *(undefined1 *)puVar3 = 0;
    puVar3 = (undefined4 *)((int)puVar3 + 1);
  }
  puVar3 = puVar2 + 0x18;
  puVar2[0x16] = puVar3;
  uVar4 = param_1[3];
  puVar2[0x17] = puVar3 + uVar4 * 0x11;
  puVar3 = puVar3 + uVar4 * 0x11 + param_1[2] * 0xb;
  if ((*param_1 & 1) != 0) {
    *puVar2 = puVar3;
    puVar3 = puVar3 + param_1[1] * 3;
  }
  if ((*param_1 & 2) != 0) {
    puVar2[1] = puVar3;
    puVar3 = puVar3 + param_1[1] * 3;
  }
  if ((*param_1 & 4) != 0) {
    puVar2[2] = puVar3;
    puVar3 = puVar3 + param_1[1];
  }
  if (param_1[4] != 0) {
    uVar4 = 0;
    puVar6 = puVar2 + 3;
    do {
      *puVar6 = puVar3;
      uVar4 = uVar4 + 1;
      puVar6 = puVar6 + 1;
      puVar3 = puVar3 + param_1[1] * 2;
    } while (uVar4 < param_1[4]);
  }
  puVar2[0xb] = *param_1;
  puVar2[0xe] = param_1[3];
  puVar2[0xd] = param_1[2];
  puVar2[0xc] = param_1[1];
  puVar2[0xf] = param_1[4];
  puVar2[0x11] = 1;
  iVar5 = FUN_004f3bc0(puVar2 + 0x12);
  if (iVar5 == 0) {
    uStack_8 = 0x123;
    uStack_4 = FUN_004d7ff0(0x80000013,uVar1);
    FUN_004d8480(&uStack_8);
    (**(code **)(DAT_007d3ff8 + 0x10c))(puVar2);
    return (undefined4 *)0x0;
  }
  puVar2[0x10] = 0xffff;
  return puVar2;
}
