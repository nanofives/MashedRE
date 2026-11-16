// FLAGS: /O2 /Ob1 /G5
/* PORT-TOP-AUDIO-5A8F30-STOPS 0x005b85e0: code pointer 0x00635198 (table 0x00635194, walked by 0x005bbb20). Ghidra had no
   function; written from the bytes of MASHED.exe.unpatched 0x005b85e0 (149 B), same shape as re/match/s25/005a8ea0.c:
   FUN_005a9ab0 create, constructor 0x005b8680, FUN_005aa200 table load, FUN_005a9d00 on each failure. match.py: 79.2% (regs 79.2%, ops 81.1%)  45/53 ins  flags=/O2 /Ob1 /G5 tc=ddk */
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
MGLOBAL(undefined1, DAT_00634ae8, 0x00634ae8);
MGLOBAL(undefined1, DAT_00634d88, 0x00634d88);
MGLOBAL(undefined1, DAT_007dde50, 0x007dde50);
MGLOBAL(undefined1, DAT_007dde68, 0x007dde68);
MGLOBAL(undefined1, DAT_007dde70, 0x007dde70);
MGLOBAL(undefined4, DAT_007ddecc, 0x007ddecc);
int FUN_005a9ab0(int param_1,byte param_2,int param_3,int param_4,int param_5,undefined4 param_6);
void FUN_005b8680(int param_1);
int FUN_005aa200(int param_1,int param_2,ushort param_3,int param_4,ushort param_5);
undefined4 FUN_005a9d00(undefined *param_1);
int FUN_005a9de0(int param_1,undefined4 param_2);
undefined4 FUN_005aa060(undefined4 p0, undefined4 * p1, undefined4 * p2);
undefined * FUN_005bbbb0(void);
// MATCH: 0x005b85e0 FUN_005b85e0
int FUN_005b85e0(void)
{
  int iVar1;
  int iVar2;

  iVar1 = FUN_005a9ab0(FUN_005aa060(0,(undefined4 *)&DAT_005e6e50,0),0xc,(int)&DAT_007dde70,(int)&DAT_007dde50,(int)&DAT_007dde68,0x3080d);
  if (iVar1 == 0) {
    return iVar1;
  }
  FUN_005b8680(iVar1);
  iVar2 = FUN_005aa200(iVar1,(int)&DAT_00634ae8,0x1c,(int)&DAT_00634d88,0x1d);
  if (iVar2 == 0) {
    FUN_005a9d00((undefined *)iVar1);
    return 0;
  }
  iVar2 = (int)FUN_005bbbb0();
  if (iVar2 == 0) {
    FUN_005a9d00((undefined *)iVar1);
    return 0;
  }
  FUN_005a9de0(iVar1,iVar2);
  DAT_007ddecc = 1;
  return iVar1;
}
