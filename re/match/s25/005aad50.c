// FLAGS: /O2 /Ob1 /G5
/* PORT-TOP-AUDIO-5AA3D0-STOPS 0x005aad50: code pointer 0x00632c38 (walked by 0x005ad080 from 0x005a6110, count 1).
   From the headless decompile (decomp_pc.py --create, Ghidra had no function) and the bytes of
   MASHED.exe.unpatched 0x005aad50..0x005aad97 (72 B, ret at 0x005aad97): BYTE-IDENTICAL under the DDK, relocheck OK. */
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

MGLOBAL(undefined1, DAT_007dcd28, 0x007dcd28);
MGLOBAL(undefined1, DAT_005e6194, 0x005e6194);
int FUN_005ac7e0(int param_1,int param_2);
uint FUN_005ac7b0(uint param_1,undefined4 param_2,undefined4 param_3);
int FUN_005aadc0(int param_1);
void FUN_005aae10(void);
void FUN_005aae30(void);
void FUN_005aae60(void);
// MATCH: 0x005aad50 FUN_005aad50
int FUN_005aad50(void)
{
  int iVar1;

  iVar1 = FUN_005ac7e0(0,(int)&DAT_007dcd28);
  if (iVar1 == 0) {
    return 0;
  }
  FUN_005ac7b0(iVar1,(undefined4)&DAT_005e6194,0);
  *(undefined4 *)(iVar1 + 0xc) = (undefined4)FUN_005aadc0;
  *(undefined4 *)(iVar1 + 0x10) = (undefined4)FUN_005aae10;
  *(undefined4 *)(iVar1 + 0x14) = (undefined4)FUN_005aae30;
  *(undefined4 *)(iVar1 + 0x18) = (undefined4)FUN_005aae60;
  return iVar1;
}
