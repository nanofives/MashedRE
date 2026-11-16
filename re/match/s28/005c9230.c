// FLAGS: /O2 /Ob1 /G5
/* PORT-TOP-AUDIO-5A8F30-STOPS 0x005c9230: code pointer 0x006351e0 (table 0x006351dc, walked by 0x005ad080 from
   0x005bbb20). Ghidra had no function; written from the bytes of MASHED.exe.unpatched 0x005c9230 (101 B). match.py --bytes: 100.0% (regs 100.0%, ops 100.0%)  31/31 ins  flags=/O2 /Ob1 /G5 tc=ddk, BYTE-IDENTICAL */
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
MGLOBAL(undefined1, DAT_007de2b8, 0x007de2b8);
MGLOBAL(undefined1, DAT_005e7a5c, 0x005e7a5c);
MGLOBAL(undefined1, DAT_005e7a7c, 0x005e7a7c);
MGLOBAL(undefined1, DAT_00636190, 0x00636190);
undefined4 FUN_005aa060(undefined4 p0, undefined4 * p1, undefined4 * p2);
int FUN_005ac7e0(int param_1,int param_2);
uint FUN_005ac7b0(uint param_1,undefined4 param_2,undefined4 param_3);
void FUN_005aca40(int param_1,undefined4 param_2,undefined4 param_3);
void FUN_005c92c0(void);
void FUN_005c92f0(void);
void FUN_005c9310(void);
void FUN_005aae60(void);
// MATCH: 0x005c9230 FUN_005c9230
int FUN_005c9230(void)
{
  int iVar1;

  iVar1 = FUN_005ac7e0(FUN_005aa060(0,(undefined4 *)&DAT_005e7a5c,0),(int)&DAT_007de2b8);
  if (iVar1 == 0) {
    return iVar1;
  }
  FUN_005ac7b0(iVar1,(undefined4)&DAT_005e7a7c,0);
  *(void (**)(void))(iVar1 + 0xc) = FUN_005c92c0;
  *(void (**)(void))(iVar1 + 0x10) = FUN_005c92f0;
  *(void (**)(void))(iVar1 + 0x14) = FUN_005c9310;
  *(void (**)(void))(iVar1 + 0x18) = FUN_005aae60;
  FUN_005aca40(iVar1,(undefined4)&DAT_00636190,2);
  return iVar1;
}
