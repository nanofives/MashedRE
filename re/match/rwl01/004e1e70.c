// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004e1e70: 56.5% regs 56.5%.  class (b) Structure right (null test outermost, 44-byte struct copy via rep movs, 0xffffffff via 4-byte uchar local, err blocks last). Original keeps a zero in esi across the function and orders the error-struct stores after the lea; permuter 66.7% variant reorders the 1/err stores (semantically wrong) so rejected.. */
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ae0, 0x00911ae0);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
typedef struct { undefined4 w[11]; } S44;
// MATCH: 0x004e1e70 FUN_004e1e70
undefined4 * FUN_004e1e70(undefined4 *param_1)
{
  undefined4 local_8;
  undefined4 local_4;
  unsigned char c[4];

  if (param_1 != (undefined4 *)0) {
    if (*(int *)(DAT_007d3ff8 + 0x124) == 3) {
      *(S44 *)param_1 = *(S44 *)(DAT_00911ae0 + 4 + DAT_007d3ff8);
      return param_1;
    }
    if (param_1 != (undefined4 *)(DAT_00911ae0 + 4 + DAT_007d3ff8)) {
      local_8 = 1;
      local_4 = FUN_004d7ff0(0x80000018);
      FUN_004d8480(&local_8);
      return (undefined4 *)0;
    }
    c[0] = 0xff; c[1] = 0xff; c[2] = 0xff; c[3] = 0xff;
    param_1[4] = 0;
    param_1[1] = 2;
    param_1[7] = 2;
    param_1[9] = 0;
    *param_1 = 7;
    param_1[2] = 5;
    param_1[3] = 6;
    param_1[5] = 1;
    param_1[6] = 1;
    param_1[8] = *(undefined4 *)c;
    param_1[10] = *(undefined4 *)c;
    return param_1;
  }
  local_8 = 1;
  local_4 = FUN_004d7ff0(0x80000016);
  FUN_004d8480(&local_8);
  return (undefined4 *)0;
}
