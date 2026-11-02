// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x0047a130: 100% (regs 100%) by hand (h6), raw converter 46.7%.
   Fix: DAT_006bf1bc/c0/c4 are float globals stored 0.0f (mov [mem], imm 0, not xor/reg); FUN_00440bc0 is passed as a
   function (push imm), not a byte global; local_4 is a dword local. Residual: none. */
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
MGLOBAL(float, DAT_006bf1bc, 0x006bf1bc);
MGLOBAL(float, DAT_006bf1c0, 0x006bf1c0);
MGLOBAL(float, DAT_006bf1c4, 0x006bf1c4);
MGLOBAL(undefined4, DAT_006132fc, 0x006132fc);
void FUN_00440bc0(void);
undefined4 FUN_0047b9b0(undefined4 p0, void *p1, void (*p2)(void));
// MATCH: 0x0047a130 FUN_0047a130
void FUN_0047a130(undefined4 param_1)
{
  undefined4 local_4;
  DAT_006bf1bc = 0.0f;
  DAT_006bf1c0 = 0.0f;
  DAT_006bf1c4 = 0.0f;
  DAT_006132fc = 0xffffffff;
  FUN_0047b9b0(param_1,&local_4,FUN_00440bc0);
}
