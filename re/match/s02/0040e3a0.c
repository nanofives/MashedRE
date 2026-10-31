// FLAGS: /O2 /GS /Ob1
/* 0x0040e3a0: 100% (regs 100%). Raw g2s was 95.7%: the only fix was declaring the default-case callee
   0x004a332b __declspec(noreturn), which drops the trailing pop ecx; ret. No residual. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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

/* 0x004a332b: push 0; push 0; push [esp+0xc]; call 0x004a3258; ret (CRT exit(code) shape). */
__declspec(noreturn) void FUN_004a332b(int param_1);

// MATCH: 0x0040e3a0 FUN_0040e3a0
void FUN_0040e3a0(int param_1, unsigned char *param_2)
{
  switch (param_1) {
  case 0:
    param_2[0] = 0x98;
    param_2[1] = 0x3a;
    param_2[2] = 0x3d;
    param_2[3] = 0xff;
    return;
  case 1:
    param_2[0] = 0x4e;
    param_2[1] = 0x89;
    param_2[2] = 0xae;
    param_2[3] = 0xff;
    return;
  case 2:
    param_2[0] = 0x61;
    param_2[1] = 0x76;
    param_2[2] = 0x56;
    param_2[3] = 0xff;
    return;
  case 3:
    param_2[0] = 0xdb;
    param_2[1] = 0xc3;
    param_2[2] = 0x62;
    param_2[3] = 0xff;
    return;
  case 4:
    param_2[0] = 0xeb;
    param_2[1] = 0xa7;
    param_2[2] = 0xa7;
    param_2[3] = 0xff;
    return;
  case 5:
    param_2[0] = 0;
    param_2[1] = 0;
    param_2[2] = 0;
    param_2[3] = 0xff;
    return;
  default:
    FUN_004a332b(-14);
  }
}
