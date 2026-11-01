// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00425a40 (104 B), h9 by hand: 100.0% (regs 100.0%). The raw single compound condition put the
   return-0 block at the end; two separate early returns inside the DAT_007f1a50 == 1 branch keep it inline.
   Callee arities fixed (FUN_00424eb0/FUN_00470c70 take 2 args, FUN_00422ba0 1). */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
MGLOBAL(int, DAT_007f1a50, 0x007f1a50);
MGLOBAL(int, DAT_007f1a54, 0x007f1a54);
MGLOBAL(int, DAT_007f1a74, 0x007f1a74);
MGLOBAL(int, DAT_007f1a70, 0x007f1a70);
MGLOBAL(int, DAT_007f1a78, 0x007f1a78);
MGLOBAL(undefined1, DAT_00803324, 0x00803324);
void FUN_00418860(void);
undefined4 FUN_00422ba0(int p0);
void FUN_00423b00(void);
undefined4 FUN_00424eb0(int p0, void *p1);
void FUN_004252c0(int p0);
undefined4 FUN_00470c70(int p0, void *p1);
// MATCH: 0x00425a40 FUN_00425a40
undefined4 FUN_00425a40(int param_1)
{
  FUN_00423b00();
  if (DAT_007f1a50 == 1) {
    if (DAT_007f1a54 != 4) return 0;
    if (DAT_007f1a74 == 0 && DAT_007f1a70 == 0) return 0;
    DAT_007f1a78 = DAT_007f1a78 + 1;
  }  FUN_00418860();
  FUN_00424eb0(param_1,&DAT_00803324);
  FUN_00470c70(param_1,&DAT_00803324);
  FUN_00422ba0(param_1);
  FUN_004252c0(param_1);
  return 1;
}
