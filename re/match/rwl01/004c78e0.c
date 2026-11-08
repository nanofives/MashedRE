// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004c78e0: 100% regs 100%. The 13-dword zero fill is memset(p,0,0x34) with #pragma intrinsic(memset) (rep stosd); store of DAT_007d40a8 first. class -. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d40a8, 0x007d40a8);
MGLOBAL(undefined4, DAT_0061819c, 0x0061819c);
MGLOBAL(undefined4, DAT_00618198, 0x00618198);
MGLOBAL(undefined4, DAT_00618180, 0x00618180);
MGLOBAL(int, DAT_007d40ac, 0x007d40ac);
MGLOBAL(undefined1, DAT_007d4084, 0x007d4084);
uint * thunk_FUN_004cc820(int p0, uint p1, uint p2, int p3, uint * p4, uint p5);
void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)
// MATCH: 0x004c78e0 FUN_004c78e0
undefined4 FUN_004c78e0(undefined4 param_1,int param_2)
{
  undefined4 uVar1;
  DAT_007d40a8 = param_2;
  memset((void *)(DAT_007d3ff8 + 0x2c + param_2), 0, 0x34);
  *(undefined4 *)(DAT_007d3ff8 + 0x38 + DAT_007d40a8) = 0;
  *(undefined4 *)(DAT_007d3ff8 + 0x3c + DAT_007d40a8) = 0;
  *(undefined4 *)(DAT_007d3ff8 + 0x40 + DAT_007d40a8) = 0;
  *(undefined1 *)(DAT_007d3ff8 + 0x4d + DAT_007d40a8) = 0x80;
  *(undefined4 *)(DAT_007d3ff8 + 0x30 + DAT_007d40a8) = 0;
  *(undefined4 *)(DAT_007d3ff8 + 0x34 + DAT_007d40a8) = 0;
  *(undefined1 *)(DAT_007d3ff8 + 0x4c + DAT_007d40a8) = 0;
  *(undefined4 *)(DAT_007d3ff8 + 0x28 + DAT_007d40a8) = 0;
  *(int *)(DAT_007d3ff8 + DAT_007d40a8) = DAT_007d3ff8 + 0x2c + DAT_007d40a8;
  uVar1 = (undefined4)thunk_FUN_004cc820(DAT_00618180,DAT_00618198,4,DAT_0061819c,(uint *)&DAT_007d4084,0x40407);
  *(undefined4 *)(DAT_007d3ff8 + 0x60 + DAT_007d40a8) = uVar1;
  if (*(int *)(DAT_007d3ff8 + 0x60 + DAT_007d40a8) == 0) {
    return 0;
  }
  DAT_007d40ac = DAT_007d40ac + 1;
  return param_1;
}
