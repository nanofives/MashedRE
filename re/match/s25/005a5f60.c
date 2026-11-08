// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005a5f60: by hand. 100%: unsigned guards (uVar2 < param_2, 0 < uVar2, while (0 < uVar2)) for jbe/ja, puVar3 = param_1 moved inside the if, puVar3 += 2 directly after uVar2++ (before the goto test). Function-pointer casts use typedefs for the C++ check. */
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
typedef int (*S25_fn0)(void);
typedef int (*S25_fn2)(undefined4 *, uint);
typedef int (*S25_fn1)(void);
// MATCH: 0x005a5f60 FUN_005a5f60
undefined4 FUN_005a5f60(undefined4 *param_1,uint param_2,S25_fn2 param_3,S25_fn2 param_4)
{
  int iVar1;
  uint uVar2;
  undefined4 *puVar3;
  uVar2 = 0;
  if (uVar2 < param_2) {
    puVar3 = param_1;
    do {
      iVar1 = (*(S25_fn0)*puVar3)();
      if (iVar1 == 0) {
LAB_005a5f9d:
        if (0 < uVar2) {
          param_1 = param_1 + uVar2 * 2 + -2;
          do {
            (*(S25_fn1)param_1[1])();
            if (param_4 != (S25_fn2)0) {
              (*param_4)(param_1,uVar2 - 1);
            }
            uVar2 = uVar2 - 1;
            param_1 = param_1 + -2;
          } while (0 < uVar2);
        }
        return 0;
      }
      if (param_3 != (S25_fn2)0) {
        iVar1 = (*param_3)(puVar3,uVar2);
      }
      uVar2 = uVar2 + 1;
      puVar3 = puVar3 + 2;
      if (iVar1 == 0) goto LAB_005a5f9d;
    } while (uVar2 < param_2);
  }
  return 1;
}
