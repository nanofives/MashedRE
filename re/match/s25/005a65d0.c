// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a65d0 (h2): best 84.6% (regs 84.6%), class b. memset intrinsic + base-relative stores; 1.0f register choice (ecx vs eax) and load order. relocheck BAD only because the code differs from the original. */
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
undefined4 FUN_005a7420(int p0);
void * __cdecl memset(void *, int, unsigned int);
#pragma intrinsic(memset)
// MATCH: 0x005a65d0 FUN_005a65d0
int FUN_005a65d0(int param_1,undefined4 param_2,uint *param_3)

{
  uint uVar1;
  int p;

  p = param_1 + 0x28;
  memset((void *)p,0,0x64);
  *(undefined4 *)(p + 4) = 0;
  *(undefined4 *)p = 0;
  FUN_005a7420(param_1);

  *(undefined4 *)(p + 0x18) = 0x3f800000;
  *(undefined4 *)(p + 0x48) = 0x3f800000;
  *(undefined4 *)(p + 0x1c) = 0;
  *(undefined4 *)(p + 0x2c) = 0;
  *(undefined4 *)(p + 0x20) = 0;
  *(undefined4 *)(p + 0x24) = 0;
  *(undefined4 *)(p + 0x58) = 0;
  *(undefined4 *)(p + 8) = 0;
  *(undefined4 *)(p + 0x5c) = 0;
  *(uint *)(p + 0xc) = *param_3 & 0x18;
  *(undefined4 *)(p + 0x14) = 0x46ac4400;
  *(undefined4 *)(p + 0x4c) = 0x461c4000;
  *(undefined4 *)(p + 0x50) = 0x3f000000;
  *(undefined4 *)(p + 0x54) = 0x3f000000;
  return param_1;
}
