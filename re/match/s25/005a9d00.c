// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005a9d00: by hand, Ghidra structure. Best 75.9% (regs 75.9%, ops 91.6%, 83/83 ins). Residual (b): original reads [esi+0xc] as a dword into ecx once (test cl,0x40 / test cl,cl) and places the return-0 epilogue between the loop and the flag tests; ours uses a byte load and different block order. A uint temp for the flags gave ops 94% but exact 69.9% (register swap eax/ecx on [esi+0x48]); permuter 133 compiles no gain. */
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
typedef void (*S25_fn1)(undefined *);
MGLOBALA(undefined, DAT_007dcc40, 1, 0x007dcc40);
undefined4 FUN_005a9d00(undefined *param_1);
void FUN_005ad2e0(undefined4 * p0);
void FUN_005b3550(undefined4 * p0);
void FUN_005ae330(int p0);
void FUN_005ae7e0(int p0);
void FUN_005a9be0(undefined * p0);
// MATCH: 0x005a9d00 FUN_005a9d00
undefined4 FUN_005a9d00(undefined *param_1)
{
  undefined4 *puVar1;
  undefined4 *puVar2;
  int iVar3;
  puVar1 = *(undefined4 **)(param_1 + 0x10);
  while( 1 ) {
    if (puVar1 == (undefined4 *)(param_1 + 0x10)) {
      if (*(S25_fn1 *)(param_1 + 0x38) != (S25_fn1)0) {
        (**(S25_fn1 *)(param_1 + 0x38))(param_1);
      }
      if ((param_1[0xc] & 4) != 0) {
        puVar1 = (undefined4 *)(*(int *)(param_1 + 0x4c) + 4);
        puVar2 = *(undefined4 **)(*(int *)(param_1 + 0x4c) + 4);
        while (puVar2 != puVar1) {
          FUN_005ad2e0(puVar2 + -5);
          puVar2 = (undefined4 *)*puVar1;
        }
      }
      if ((param_1[0xc] & 8) != 0) {
        puVar1 = *(undefined4 **)(param_1 + 0x50);
        puVar2 = (undefined4 *)*puVar1;
        while (puVar2 != puVar1) {
          FUN_005b3550(puVar2 + -8);
          puVar2 = (undefined4 *)*puVar1;
        }
      }
      iVar3 = *(int *)(param_1 + 0x48);
      if (iVar3 != 0) {
        if ((*(uint *)(param_1 + 0xc) & 0x40) == 0) {
          if ((char)*(uint *)(param_1 + 0xc) < '\0') {
            FUN_005ae330(iVar3);
          }
        }
        else {
          FUN_005ae7e0(iVar3);
        }
        *(undefined4 *)(param_1 + 0x48) = 0;
      }
      if (param_1 != (undefined *)&DAT_007dcc40) {
        **(undefined4 **)(param_1 + 0x1c) = *(undefined4 *)(param_1 + 0x18);
        *(undefined4 *)(*(int *)(param_1 + 0x18) + 4) = *(undefined4 *)(param_1 + 0x1c);
      }
      FUN_005a9be0(param_1);
      return 1;
    }
    iVar3 = FUN_005a9d00((undefined *)(puVar1 + -6));
    if (iVar3 == 0) break;
    puVar1 = *(undefined4 **)(param_1 + 0x10);
  }
  return 0;
}
