// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00558550: hand-matched (h1). 100%. Count global typed unsigned with '> 0' gives jbe (raw != 0 gave je). code typedef is variadic under C++. */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(unsigned int, DAT_007dc8b4, 0x007dc8b4);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_007dc8b0, 0x007dc8b0);
MGLOBAL(undefined4, DAT_007dc8bc, 0x007dc8bc);
MGLOBAL(undefined4, DAT_007dc8b8, 0x007dc8b8);
// MATCH: 0x00558550 FUN_00558550
undefined4 FUN_00558550(void)

{
  undefined4 local_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  if (DAT_007dc8b4 > 0) {
    (**(code **)(DAT_007d3ff8 + 0x24))(6,&local_1c);
    (**(code **)(DAT_007d3ff8 + 0x24))(8,&uStack_18);
    (**(code **)(DAT_007d3ff8 + 0x24))(10,&uStack_14);
    (**(code **)(DAT_007d3ff8 + 0x24))(0xb,&uStack_10);
    (**(code **)(DAT_007d3ff8 + 0x24))(1,&uStack_c);
    (**(code **)(DAT_007d3ff8 + 0x24))(9,&uStack_8);
    (**(code **)(DAT_007d3ff8 + 0x24))(0x14,&uStack_4);
    (**(code **)(DAT_007d3ff8 + 0x20))(6,0);
    (**(code **)(DAT_007d3ff8 + 0x20))(8,0);
    (**(code **)(DAT_007d3ff8 + 0x20))(9,1);
    (**(code **)(DAT_007d3ff8 + 0x20))(1,DAT_007dc8b0);
    (**(code **)(DAT_007d3ff8 + 0x20))(10,5);
    (**(code **)(DAT_007d3ff8 + 0x20))(0xb,6);
    (**(code **)(DAT_007d3ff8 + 0x20))(0x14,1);
    (**(code **)(DAT_007d3ff8 + 0x34))
              (3,DAT_007dc8b8,DAT_007dc8b4 << 2,DAT_007dc8bc,DAT_007dc8b4 * 6);
    (**(code **)(DAT_007d3ff8 + 0x20))(6,local_1c);
    (**(code **)(DAT_007d3ff8 + 0x20))(8,uStack_18);
    (**(code **)(DAT_007d3ff8 + 0x20))(10,uStack_14);
    (**(code **)(DAT_007d3ff8 + 0x20))(0xb,uStack_10);
    (**(code **)(DAT_007d3ff8 + 0x20))(1,uStack_c);
    (**(code **)(DAT_007d3ff8 + 0x20))(9,uStack_8);
    (**(code **)(DAT_007d3ff8 + 0x20))(0x14,uStack_4);
  }
  DAT_007dc8b4 = 0;
  DAT_007dc8b0 = 0;
  return 1;
}
