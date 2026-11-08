// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 53.5% (regs 69.0%, permuter best (150 s). Residual not classified beyond register/schedule differences. Class: b. */
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
typedef unsigned int uint;
typedef unsigned __int64 ulonglong;
void FUN_005aeda0(uint p0, uint p1, uint * p2, uint * p3);
// MATCH: 0x005b3020 FUN_005b3020
uint FUN_005b3020(int param_1)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint local_10;
  uint *local_c;
  int local_8;
  undefined1 local_4 [4];
  
  local_8 = *(int *)(param_1 + 0x13c);
  uVar4 = 0;
  uVar3 = 0;
  uVar2 = 0;
  if (*(int *)(*(int *)(param_1 + 0x118) + 0x14) != 0) {
    local_c = (uint *)(*(int *)(*(int *)(param_1 + 0x118) + 0x18) + 0x20);
    do {
      uVar1 = *(uint *)(*(int *)(local_8 + 0xc) + uVar3 * 4);
      if (uVar4 < uVar1) {
        uVar4 = uVar1;
        local_10 = uVar1 % *local_c;
        FUN_005aeda0(local_10,(int)(0xffffffff / (ulonglong)*local_c),(uint *)local_4,&local_10);
        if (uVar2 < local_10 + 1) {
          uVar2 = local_10 + 1;
        }
      }
      uVar3 = uVar3 + 1;
      local_c = local_c + 10;
    } while (uVar3 < *(uint *)(*(int *)(param_1 + 0x118) + 0x14));
    if (uVar2 == 1) {
      return 0xffffffff;
    }
  }
  return uVar2;
}
