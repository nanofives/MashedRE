// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005ad320 (h3): 65.2% exact, regs 78.3%. Residual class (b) register allocation only (ops 98.9%): original keeps the loop offset in ebp and the counter on the stack, ours the reverse; permuter moved one statement (65.2%). 3rd (unused) param added, matching the caller push 1. */
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
typedef unsigned short ushort;
typedef unsigned char byte;
int FUN_005adf30(undefined4 p0, undefined4 p1);
// MATCH: 0x005ad320 FUN_005ad320
uint FUN_005ad320(uint param_1,uint param_2,int param_3)
{
  uint uVar1;
  int iVar2;
  uint ok;
  int iVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  uint local_8;

  iVar2 = *(int *)(param_2 + 0x4c);
  local_8 = (uint)*(ushort *)(param_1 + 0x14);
  uVar1 = (uint)*(ushort *)(iVar2 + 0xc);
  ok = 1;
  if (local_8 > 0) {
    iVar6 = 0;
    do {
      uVar5 = 0;
      if (uVar1 > 0) {
        iVar7 = 0;
        do {
          iVar4 = FUN_005adf30(*(undefined4 *)(iVar7 + *(int *)(iVar2 + 0x10)),
                               *(undefined4 *)(*(int *)(param_1 + 0x10) + iVar6));
          if (iVar4 == 0) goto LAB_005ad38e;
          uVar5 = uVar5 + 1;
          iVar7 = iVar7 + 0x18;
        } while (uVar5 < uVar1);
      }
      ok = 0;
LAB_005ad38e:
      iVar6 = iVar6 + 0x14;
      local_8 = local_8 - 1;
    } while (local_8 != 0);
  }
  local_8 = (uint)*(ushort *)(param_1 + 0x16);
  uVar5 = (uint)*(ushort *)(*(int *)(param_2 + 0x4c) + 0xe);
  if (local_8 > 0) {
    iVar6 = 0;
    param_2 = local_8;
    do {
      uVar8 = 0;
      if (uVar5 > 0) {
        iVar7 = 0;
        do {
          iVar4 = FUN_005adf30(*(undefined4 *)(*(int *)(iVar2 + 0x14) + iVar7),
                               *(undefined4 *)(*(int *)(param_1 + 0xc) + iVar6));
          if (iVar4 == 0) goto LAB_005ad3f7;
          uVar8 = uVar8 + 1;
          iVar7 = iVar7 + 0x18;
        } while (uVar8 < uVar5);
      }
      ok = 0;
LAB_005ad3f7:
      iVar6 = iVar6 + 0x14;
      param_2 = param_2 - 1;
    } while (param_2 != 0);
  }
  return (ok ? -1 : 0) & param_1;
}
