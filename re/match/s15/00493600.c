// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00493600 (h8): 100% (regs 100%). Returns int (0/1), not bool: `return 0` after a zero test reuses eax
   (no xor al,al), and `iVar1 != 0` gives neg/sbb/neg. The callbacks are real function declarations (pushed as
   immediates), not byte globals. Residual: none. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef void (*code)(void);
MGLOBAL(void *, PTR_DAT_005cf748, 0x005cf748);
MGLOBAL(void *, PTR_DAT_005cf74c, 0x005cf74c);
void FUN_00513f90(void);
void FUN_0052d530(void);
void FUN_004935a0(void);
void FUN_004935d0(void);
int FUN_004ce790(void *p0, code p1, code p2);
// MATCH: 0x00493600 FUN_00493600
int FUN_00493600(void)
{
  int iVar1;

  iVar1 = FUN_004ce790(&PTR_DAT_005cf74c, FUN_004935a0, FUN_0052d530);
  if (iVar1 == 0) {
    return 0;
  }
  iVar1 = FUN_004ce790(&PTR_DAT_005cf748, FUN_004935d0, FUN_00513f90);
  return iVar1 != 0;
}
