// FLAGS: /O2 /Ob1 /G5
/* PORT-TOP-AUDIO-5AA3D0-STOPS 0x005a8ea0: code pointer 0x00632c28 (walked by 0x005ad080 from 0x005a6110, count 1).
   From the headless decompile (decomp_pc.py --create, Ghidra had no function) and the bytes of
   MASHED.exe.unpatched 0x005a8ea0..0x005a8f28 (137 B). Best 77.6% (41/49 ins): the DDK cross-jumps the two failure tails
   (push esi / call 0x005a9d00 / xor eax,eax / ret) that the original keeps apart (0x005a8eee and 0x005a8f04), and the
   iVar1 == 0 return into them; nested-if and return-variable spellings did not change it. equiv.py
   EQUIV-IF-CALLERS-IGNORE(EAX) [LAYOUT]; the caller 0x005ad080 tests the return (indirect, table 0x00632c28). */
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

MGLOBAL(undefined4, DAT_009146fc, 0x009146fc);
MGLOBAL(undefined4, DAT_007dcbec, 0x007dcbec);
MGLOBAL(undefined1, DAT_007dcb70, 0x007dcb70);
MGLOBAL(undefined1, DAT_007dcb78, 0x007dcb78);
MGLOBAL(undefined1, DAT_007dcbd4, 0x007dcbd4);
MGLOBAL(undefined1, PTR_DAT_00633080, 0x00633080);
MGLOBAL(undefined1, PTR_DAT_00633140, 0x00633140);
int FUN_005a9ab0(int param_1,byte param_2,int param_3,int param_4,int param_5,undefined4 param_6);
void FUN_005a8f30(int param_1);
int FUN_005aa200(int param_1,int param_2,ushort param_3,int param_4,ushort param_5);
undefined4 FUN_005a9d00(undefined *param_1);
undefined * FUN_005b34f0(void);
int FUN_005a9de0(int param_1,undefined4 param_2);
// MATCH: 0x005a8ea0 FUN_005a8ea0
int FUN_005a8ea0(void)
{
  int iVar1;
  int iVar2;

  iVar1 = FUN_005a9ab0(DAT_009146fc,0xc,(int)&DAT_007dcb78,(int)&DAT_007dcbd4,(int)&DAT_007dcb70,0x3080b);
  if (iVar1 == 0) {
    return iVar1;
  }
  FUN_005a8f30(iVar1);
  iVar2 = FUN_005aa200(iVar1,(int)&PTR_DAT_00633080,8,(int)&PTR_DAT_00633140,9);
  if (iVar2 == 0) {
    FUN_005a9d00((undefined *)iVar1);
    return 0;
  }
  iVar2 = (int)FUN_005b34f0();
  if (iVar2 == 0) {
    FUN_005a9d00((undefined *)iVar1);
    return 0;
  }
  FUN_005a9de0(iVar1,iVar2);
  DAT_007dcbec = 1;
  return iVar1;
}
