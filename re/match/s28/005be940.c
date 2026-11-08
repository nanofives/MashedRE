// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005be940 : 100.0% (regs 100). By hand from raw g2s, stdcall ret 0xc, callee 0x005bf6d0 stdcall 3 args, vtbl call __stdcall. Residual: none. */

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

MGLOBAL(unsigned int, DAT_005d0b9c, 0x005d0b9c);
int FUN_005adf30(unsigned char * p0, unsigned char * p1);
unsigned int __stdcall FUN_005bf6d0(int * p0, unsigned char * p1, unsigned int * p2);
// MATCH: 0x005be940 FUN_005be940
unsigned int __stdcall FUN_005be940(int *param_1,unsigned char *param_2,unsigned int *param_3)
{
  int iVar1;

  *param_3 = 0;
  iVar1 = FUN_005adf30(param_2,(unsigned char *)&DAT_005d0b9c);
  if (iVar1 == 0) {
    *param_3 = (unsigned int)param_1;
    (**(int (__stdcall **)(int *))(*param_1 + 4))(param_1);
    return 0;
  }
  return FUN_005bf6d0(param_1,param_2,param_3);
}
