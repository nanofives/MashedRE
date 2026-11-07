// FLAGS: /O2 /Ob1 /G5
/* best 91.7% (regs 95.0%, ops 96.7%): residual = x87-free but add-chain order: original computes (iVar1 + w0xe*8 + 8) then + (w0xc*3)*4 with
   the 0xe word loaded second; ours loads the words in the other order. Tried three association orders and the permuter. The
   unsigned-gt-0 form "(x & 0xfffffff) > 0" is what gives the original jbe layout. Class (b). */
/* MATCH-S25 0x005aa7e0: hand-matched (h1). */
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

MGLOBAL(char, DAT_00617ffc, 0x00617ffc);
int FUN_005aabe0(int p0);
// MATCH: 0x005aa7e0 FUN_005aa7e0
int FUN_005aa7e0(int param_1,byte param_2,uint *param_3)
{
  int iVar1;
  uint uVar2;
  uint *puVar3;
  puVar3 = param_3;
  if (param_3 == (uint *)0x0) {
    puVar3 = (uint *)&param_3;
  }
  iVar1 = 0x28;
  if ((*(uint *)(param_1 + 0x40) & 0xfffffff) > 0) {
    iVar1 = FUN_005aabe0(param_1);
    if ((iVar1 == 0) && (1 << (byte)(*(uint *)(param_1 + 0x40) >> 0x1c) == 1)) {
      uVar2 = *(uint *)(param_1 + 0x40) & 0xfffffff;
      *(uint *)(param_1 + 0x40) = uVar2;
      *(uint *)(param_1 + 0x40) = (DAT_00617ffc + -1) * 0x10000000 | uVar2;
    }
    iVar1 = 1 << (byte)((uint)*(undefined4 *)(param_1 + 0x40) >> 0x1c);
    uVar2 = ~(iVar1 - 1U) & iVar1 + 0x27U;
    *puVar3 = uVar2;
    iVar1 = (*(uint *)(param_1 + 0x40) & 0xfffffff) + uVar2;
  }
  else {
    *puVar3 = 0;
  }
  if ((param_2 & 2) != 0) {
    iVar1 = iVar1 + 4;
  }
  if ((param_2 & 1) != 0) {
    iVar1 = iVar1 + 8 + (uint)*(ushort *)(*(int *)(param_1 + 0x4c) + 0xe) * 8 +
            (uint)*(ushort *)(*(int *)(param_1 + 0x4c) + 0xc) * 0xc;
  }
  return iVar1;
}
