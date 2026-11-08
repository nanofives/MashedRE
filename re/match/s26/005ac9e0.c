// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005ac9e0: 100% from the raw converter body with one spelling change: (*param_2 >= param_1[2]) instead of (param_1[2] <= *param_2) (cmp operand order). casts added for C++. cxx pending. */
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
int FUN_005adf30(byte * p0, byte * p1);
// MATCH: 0x005ac9e0 FUN_005ac9e0

undefined4 FUN_005ac9e0(undefined4 *param_1,uint *param_2)

{
  int iVar1;
  
  if ((((*(char *)((int)param_2 + 0xd) == *(char *)((int)param_1 + 5)) &&
       ((char)param_2[3] == *(char *)(param_1 + 1))) && (*param_2 >= (uint)param_1[2])) &&
     (*param_2 <= (uint)param_1[3])) {
    iVar1 = FUN_005adf30((byte *)param_2[1],(byte *)*param_1);
    if ((iVar1 == 0) && (((*(byte *)(param_1 + 4) ^ (byte)param_2[6]) & 1) == 0)) {
      return 1;
    }
  }
  return 0;
}
