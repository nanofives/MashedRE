// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d5190: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs21_20261107/results.tsv. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifndef __cplusplus
#define false 0
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ae0, 0x00911ae0);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004d5190 FUN_004d5190
bool FUN_004d5190(int param_1,int *param_2,int param_3)

{
  uint uVar1;
  undefined4 *puVar2;
  int iVar3;
  uint uVar4;
  bool bVar5;
  int *piVar6;
  undefined4 local_8;
  undefined4 local_4;
  
  uVar1 = *(uint *)(param_3 + 0x2c);
  piVar6 = param_2;
  for (iVar3 = 10; iVar3 != 0; iVar3 = iVar3 + -1) {
    *piVar6 = 0;
    piVar6 = piVar6 + 1;
  }
  if (uVar1 >= 0x21) {
    local_8 = 1;
    local_4 = FUN_004d7ff0(0x29);
    FUN_004d8480(&local_8);
  }
  bVar5 = 0x20 < *(uint *)(param_3 + 0x20);
  if (bVar5) {
    local_8 = 1;
    local_4 = FUN_004d7ff0(0x28);
    FUN_004d8480(&local_8);
  }
  bVar5 = !bVar5 && uVar1 < 0x21;
  uVar4 = *(uint *)(DAT_00911ae0 + 0x38 + DAT_007d3ff8);
  if (uVar4 <= uVar1) {
    local_8 = 1;
    local_4 = FUN_004d7ff0(0x2a);
    FUN_004d8480(&local_8);
    return false;
  }
  if (bVar5) {
    puVar2 = (undefined4 *)(*(int *)(param_1 + 4) * 0x80 + uVar4 * 0x28 + *(int *)(param_1 + 8));
    uVar4 = 0;
    param_2[2] = (int)puVar2;
    param_2[1] = uVar1;
    if (uVar1 != 0) {
      do {
        *puVar2 = 0xffffffff;
        puVar2 = puVar2 + 1;
        uVar4 = uVar4 + 1;
      } while (uVar4 < (uint)param_2[1]);
    }
    puVar2 = (undefined4 *)
             (*(int *)(param_1 + 8) +
             (*(int *)(param_1 + 4) + *(int *)(DAT_00911ae0 + 0x38 + DAT_007d3ff8) * 0xe) * 0xc);
    *puVar2 = 0;
    puVar2[1] = 0;
    puVar2[2] = 0;
    param_2[7] = (int)puVar2;
    param_2[8] = 0;
    param_2[9] = 0;
    *param_2 = param_3;
    *(int *)(param_1 + 4) = *(int *)(param_1 + 4) + 1;
  }
  return bVar5;
}
