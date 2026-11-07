// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a79a0 (h2): best 100.0% (regs 100.0%), class -. C++ fix: list-head casts. relocheck. */
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
MGLOBAL(undefined4, DAT_007dca7c, 0x007dca7c);
MGLOBAL(int, DAT_007dcabc, 0x007dcabc);
MGLOBAL(int, DAT_007dcab8, 0x007dcab8);
uint FUN_005a7a40(uint p0, undefined4 p1);
void FUN_005a7ea0(int p0);
void FUN_005abcf0(int p0);
void FUN_005ade90(int p0);
// MATCH: 0x005a79a0 FUN_005a79a0
void FUN_005a79a0(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  
  **(undefined4 **)(param_1 + 0x20) = *(undefined4 *)(param_1 + 0x1c);
  *(undefined4 *)(*(int *)(param_1 + 0x1c) + 4) = *(undefined4 *)(param_1 + 0x20);
  iVar3 = *(int *)(param_1 + 0x10);
  if (iVar3 != param_1 + 0xc) {
    do {
      uVar1 = *(undefined4 *)(iVar3 + 8);
      puVar4 = (undefined4 *)DAT_007dca7c;
      if (puVar4 == (undefined4 *)&DAT_007dca7c) {
        FUN_005abcf0(uVar1);
      }
      else {
        do {
          iVar2 = FUN_005a7a40((uint)(puVar4 + -7),uVar1);
          if (iVar2 != 0) goto LAB_005a7a03;
          puVar4 = (undefined4 *)*puVar4;
        } while (puVar4 != (undefined4 *)&DAT_007dca7c);
      }
      FUN_005abcf0(uVar1);
LAB_005a7a03:
      iVar3 = *(int *)(iVar3 + 4);
    } while (iVar3 != param_1 + 0xc);
  }
  FUN_005ade90(param_1 + 0xc);
  if (param_1 == DAT_007dcabc) {
    DAT_007dcabc = DAT_007dcab8;
  }
  FUN_005a7ea0(param_1);
  return;
}
