// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004d3db0: 100% regs 100%. Failure block (free + zero + return 0) written first as if (== 0), success path after. class -. */
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
MGLOBAL(int, DAT_007d470c, 0x007d470c);
MGLOBAL(undefined4, DAT_00618410, 0x00618410);
MGLOBAL(int, DAT_007d4710, 0x007d4710);
MGLOBAL(undefined4, DAT_0061841c, 0x0061841c);
MGLOBAL(undefined4, DAT_00618418, 0x00618418);
MGLOBAL(int, DAT_00911ae0, 0x00911ae0);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_00618414, 0x00618414);
MGLOBAL(undefined1, DAT_007d46e8, 0x007d46e8);
void FUN_004e1780(int p0);
uint * FUN_004e17e0(uint p0);
undefined4 * FUN_004e1e70(undefined4 * p0);
uint * thunk_FUN_004cc820(int p0, uint p1, uint p2, int p3, uint * p4, uint p5);
// MATCH: 0x004d3db0 FUN_004d3db0
undefined4 FUN_004d3db0(void)
{
  undefined4 uVar1;

  if (DAT_007d470c == 0) {
    DAT_007d4710 = (int)FUN_004e17e0(DAT_00618410);
    if (DAT_007d4710 != 0) {
      uVar1 = (undefined4)thunk_FUN_004cc820(0x34,DAT_00618418,4,DAT_0061841c,(uint *)&DAT_007d46e8,0x40409);
      *(undefined4 *)(DAT_007d3ff8 + DAT_00911ae0) = uVar1;
      if (*(int *)(DAT_007d3ff8 + DAT_00911ae0) == 0) {
        FUN_004e1780(DAT_007d4710);
        DAT_007d4710 = 0;
        return 0;
      }
      *(undefined4 *)(DAT_007d3ff8 + 0x38 + DAT_00911ae0) = DAT_00618414;
      FUN_004e1e70((undefined4 *)(DAT_00911ae0 + 4 + DAT_007d3ff8));
      *(undefined4 *)(DAT_007d3ff8 + 0x34 + DAT_00911ae0) = 0;
      *(undefined4 *)(DAT_007d3ff8 + 0x30 + DAT_00911ae0) = 0;
      DAT_007d470c = 1;
      return 1;
    }
  }
  return 0;
}
