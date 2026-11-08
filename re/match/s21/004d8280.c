// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d8280 (h4): 100% (regs 100%). Raw form with the per-link callback called through the object base (obj = link - 8, fn at obj+0x10) and real pointer casts. Residual: none. */
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
typedef unsigned char byte;
typedef void (*SyncFn)(void *);
undefined4 * FUN_004c4600(undefined4 * p0, undefined4 * p1, undefined4 * p2);
// MATCH: 0x004d8280 FUN_004d8280
void FUN_004d8280(int param_1,uint param_2)

{
  undefined4 *puVar1;
  uint uVar2;
  
  for (; param_1 != 0; param_1 = *(int *)(param_1 + 0x9c)) {
    uVar2 = *(byte *)(param_1 + 3) | param_2;
    if ((uVar2 & 4) != 0) {
      FUN_004c4600((undefined4 *)(param_1 + 0x50),(undefined4 *)(param_1 + 0x10),(undefined4 *)(*(int *)(param_1 + 4) + 0x50));
    }
    for (puVar1 = *(undefined4 **)(param_1 + 0x90); puVar1 != (undefined4 *)(param_1 + 0x90);
        puVar1 = (undefined4 *)*puVar1) {
      undefined4 *obj = puVar1 - 2;
      (*(SyncFn *)(obj + 4))(obj);
    }
    *(byte *)(param_1 + 3) = *(byte *)(param_1 + 3) & 0xf3;
    FUN_004d8280(*(int *)(param_1 + 0x98),uVar2);
  }
  return;
}
