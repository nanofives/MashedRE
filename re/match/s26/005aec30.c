// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005aec30: 73.2% (regs 75.6%, ops 95.1%). Structure matches (switch 4 then 2, same 41 insns); residual: original loads ecx=param_1 before the shr/je, schedules shl/and of the 32-bit swap in the other order, and dh/dl byte loads swapped. Tried operand order of the OR, do-while forms, pointer copies, permuter (96 compiles). Class (b). cxx pending. */
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
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
// MATCH: 0x005aec30 FUN_005aec30
void FUN_005aec30(uint *param_1,uint param_2,int param_3)
{
  uint uVar1;
  ushort *ps;

  switch (param_3) {
  case 4:
    param_2 = param_2 >> 2;
    if (param_2 != 0) {
      do {
        uVar1 = *param_1;
        *param_1 = (uVar1 << 0x10 | uVar1 & 0xff00 | uVar1 >> 0x10 & 0xff) << 8 |
                   (uint)*(byte *)((int)param_1 + 3);
        param_1 = param_1 + 1;
      } while (--param_2 != 0);
    }
    break;
  case 2:
    ps = (ushort *)param_1;
    for (param_2 = param_2 >> 1; param_2 != 0; param_2 = param_2 - 1) {
      *ps = (ushort)(((uint)*(byte *)ps << 8) | *((byte *)ps + 1));
      ps = ps + 1;
    }
    break;
  }
}
