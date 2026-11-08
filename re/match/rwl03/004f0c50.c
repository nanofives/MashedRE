// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x004f0c50: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int (__cdecl code)(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_004d7ff0(undefined4 p0, undefined4 p1);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004f0c50 FUN_004f0c50
uint * FUN_004f0c50(uint *param_1,undefined4 param_2,undefined2 param_3,undefined2 param_4,
                   undefined2 param_5,undefined2 param_6,undefined2 param_7,undefined2 param_8,
                   undefined2 param_9)

{
  uint uVar1;
  uint uVar2;
  uint sz;
  struct { undefined4 a; undefined4 b; } e;
  
  uVar2 = param_1[1];
  if (uVar2 >= *param_1) {
    sz = (uVar2 * 5 + 5) * 4;
    if (uVar2 != 0) {
      uVar1 = (**(code **)(DAT_007d3ff8 + 0x110))(param_1[2],sz,0x1030502);
    }
    else {
      uVar1 = (**(code **)(DAT_007d3ff8 + 0x108))(sz,0x1030502);
    }
    if (uVar1 == 0) {
      e.a = 2;
      e.b = FUN_004d7ff0(0x80000013,sz);
      FUN_004d8480(&e.a);
      return (uint *)0x0;
    }
    param_1[2] = uVar1;
    uVar2 = param_1[1];
    *param_1 = uVar2 + 1;
  }
  *(undefined4 *)(param_1[2] + 8 + uVar2 * 0x14) = param_2;
  *(undefined2 *)(param_1[2] + param_1[1] * 0x14) = param_3;
  *(undefined2 *)(param_1[2] + 2 + param_1[1] * 0x14) = param_4;
  *(undefined2 *)(param_1[2] + 4 + param_1[1] * 0x14) = param_5;
  *(undefined2 *)(param_1[2] + 0xc + param_1[1] * 0x14) = param_6;
  *(undefined2 *)(param_1[2] + 0xe + param_1[1] * 0x14) = param_7;
  *(undefined2 *)(param_1[2] + 0x10 + param_1[1] * 0x14) = param_8;
  *(undefined2 *)(param_1[2] + 0x12 + param_1[1] * 0x14) = param_9;
  param_1[1] = param_1[1] + 1;
  return param_1;
}
