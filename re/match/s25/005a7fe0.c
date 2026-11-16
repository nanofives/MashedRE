// FLAGS: /O2 /Ob1 /G5
/* PORT-TOP-AUDIO-5A8F30-STOPS 0x005a7fe0: code pointer 0x006351a4 (table 0x00635194, walked by 0x005bbb20). Ghidra had no
   function; written from the bytes of MASHED.exe.unpatched 0x005a7fe0 (127 B), same shape as re/match/s25/005a8ea0.c:
   FUN_005a9ab0 create, constructor 0x005a8060, FUN_005aa200 table load, FUN_005a9d00 on each failure. match.py: 91.1% (regs 91.1%, ops 93.3%)  43/45 ins  flags=/O2 /Ob1 /G5 tc=ddk */
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
MGLOBAL(undefined1, DAT_005e6e50, 0x005e6e50);
MGLOBAL(undefined1, DAT_00632f10, 0x00632f10);
MGLOBAL(undefined1, DAT_00632fa0, 0x00632fa0);
MGLOBAL(undefined1, DAT_007dcac0, 0x007dcac0);
MGLOBAL(undefined1, DAT_007dcae8, 0x007dcae8);
MGLOBAL(undefined1, DAT_007dcb44, 0x007dcb44);
MGLOBAL(undefined1, DAT_005e662c, 0x005e662c);
int FUN_005a9ab0(int param_1,byte param_2,int param_3,int param_4,int param_5,undefined4 param_6);
void FUN_005a8060(int param_1);
int FUN_005aa200(int param_1,int param_2,ushort param_3,int param_4,ushort param_5);
undefined4 FUN_005a9d00(undefined *param_1);
int FUN_005a9de0(int param_1,undefined4 param_2);
undefined4 FUN_005aa060(undefined4 p0, undefined4 * p1, undefined4 * p2);
int FUN_005ad6a0(undefined1 *p0);
// MATCH: 0x005a7fe0 FUN_005a7fe0
int FUN_005a7fe0(void)
{
  int iVar1;
  int iVar2;

  iVar1 = FUN_005a9ab0(FUN_005aa060(0,(undefined4 *)&DAT_005e6e50,0),0xc,(int)&DAT_007dcae8,(int)&DAT_007dcac0,(int)&DAT_007dcb44,0x3080e);
  if (iVar1 == 0) {
    return iVar1;
  }
  FUN_005a8060(iVar1);
  iVar2 = FUN_005aa200(iVar1,(int)&DAT_00632f10,0x6,(int)&DAT_00632fa0,0x9);
  if (iVar2 == 0) {
    FUN_005a9d00((undefined *)iVar1);
    return 0;
  }
  FUN_005a9de0(iVar1,FUN_005ad6a0(&DAT_005e662c));
  return iVar1;
}
