// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004ddd60: best 38.8% (regs ~55%, ops 77%), class (b). Pixel-format packer as a switch on (flags & 0xf00): the compare tree and case order match the original (it is not an if-chain). The per-case expressions differ: the compiler distributes the shifts of the RGB555/565 packers ((x*2|y)<<5|z)<<2) where the original keeps the stages separate; ushort/uchar stage casts did not help (35-39%). */
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
#define CONCAT11(a,b) ((ushort)(((uint)(uchar)(a)<<8)|(uchar)(b)))
#define CONCAT21(a,b) ((uint)(((uint)(ushort)(a)<<8)|(uchar)(b)))
#define CONCAT31(a,b) ((uint)(((uint)(a)<<8)|(uchar)(b)))
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004ddd60 FUN_004ddd60
undefined4 FUN_004ddd60(uint *param_1,byte *param_2,uint param_3)
{
  int iVar2;
  uint uVar1;
  undefined4 loc[2];
  switch ((int)(param_3 & 0xf00)) {
  case 0:
    iVar2 = FUN_004ddd60((uint *)&param_2,param_2,0x500);
    if (iVar2 == 0) {
      return 0;
    }
    *param_1 = (uint)param_2;
    return 1;
  case 0x100:
    uVar1 = (ushort)((param_2[3] & 0x80) * 2 | *param_2 & 0xf8);
    *param_1 = (uVar1 << 5 | param_2[1] & 0xf8) << 2 | (uint)(param_2[2] >> 3);
    return 1;
  case 0x200:
    *param_1 = ((*param_2 & 0xf8) << 5 | param_2[1] & 0xfc) << 3 | (uint)(param_2[2] >> 3);
    return 1;
  case 0x300:
    *param_1 = ((param_2[3] & 0xf0) << 4 | *param_2 & 0xf0) << 4 |
               param_2[1] & 0xf0 | (uint)(param_2[2] >> 4);
    return 1;
  case 0x400:
    *param_1 = ((((uint)param_2[1] * 0x3b + (uint)param_2[2] * 0xb + (uint)*param_2 * 0x1e) / 100)
               * (uint)param_2[3]) / 0xff;
    return 1;
  case 0x500:
    *param_1 = ((((uint)param_2[3] << 8 | (uint)*param_2) << 8 | (uint)param_2[1]) << 8) | (uint)param_2[2];
    return 1;
  case 0x600:
    *param_1 = ((*param_2 | 0xffffff00) << 8 | (uint)param_2[1]) << 8 | (uint)param_2[2];
    return 1;
  case 0xa00:
    *param_1 = (((*param_2 & 0xf8) << 5 | param_2[1] & 0xf8) << 2) | (uint)(param_2[2] >> 3);
    return 1;
  }
  loc[0] = 1;
  loc[1] = FUN_004d7ff0(0x8000000d);
  FUN_004d8480(loc);
  return 0;
}
