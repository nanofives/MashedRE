// FLAGS: /O2 /Ob1 /G5
/* 0x005b9f30: 100.0% BYTE-IDENTICAL, source written from the listing (PORT-TOP-AUDIO-5A8F30-STOPS, 2026-11-16). Constructor:
   FUN_005a9e10 descriptor call, the nested bit-length macro HB32(M) of re/match/s25/005a8f30.c (M = 1 << FUN_005aee20(0x28)
   re-evaluated at every use), then the stores in listing order with the +0x40 or one store after +0x28. Previous source
   (29.2%) kept in D:/MashedWT/PORT-TOP-AUDIO-5A8F30-STOPS_data/005b9f30_before.c. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBALA(signed char, DAT_00617ff8, 256, 0x00617ff8);
MGLOBAL(undefined1, DAT_005e6e50, 0x005e6e50);
void FUN_005ba1d0(void);
void FUN_005ba780(void);
void FUN_005ba7f0(void);
void FUN_005bad30(void);
undefined4 FUN_005a9e10(int p0, undefined1 * p1, undefined4 p2);
int FUN_005aee20(uint p0);
#define SH_005b9f30 (1 << FUN_005aee20(0x28))
#define M_005b9f30 (((short)SH_005b9f30 != 0) ? (SH_005b9f30 & 0xffff) : 1)
#define HB16_005b9f30(w) ((((w) & 0xff00) != 0) ? DAT_00617ff8[(w) >> 8] + 7 : DAT_00617ff8[(w)] - 1)
#define HB32_005b9f30(v) ((((v) & 0xffff0000) != 0) ? HB16_005b9f30((v) >> 16) + 16 : HB16_005b9f30(v))
// MATCH: 0x005b9f30 FUN_005b9f30
void FUN_005b9f30(int param_1)

{
  int iVar2;

  FUN_005a9e10(param_1,&DAT_005e6e50,0);
  *(undefined4 *)(param_1 + 0x40) = 0xf8;
  iVar2 = HB32_005b9f30(M_005b9f30);
  *(void (**)(void))(param_1 + 0x28) = FUN_005ba1d0;
  *(uint *)(param_1 + 0x40) = *(uint *)(param_1 + 0x40) | iVar2 << 0x1c;
  *(void (**)(void))(param_1 + 0x34) = FUN_005ba780;
  *(void (**)(void))(param_1 + 0x30) = FUN_005ba7f0;
  *(void (**)(void))(param_1 + 0x3c) = FUN_005bad30;
  *(undefined2 *)(param_1 + 0x44) = 0x28;
  *(undefined2 *)(param_1 + 0x46) = 0x1;
  return;
}
