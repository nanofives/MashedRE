// FLAGS: /O2 /Ob1 /G5
// best 2.4. 9-byte CRT fragment: lea edx,[esp+4]; call 0x4a70d5, then falls through into 0x005c2f8d (Ghidra boundary, no ret). Class (c).
/* MATCH-S28 0x005c2f84: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs28_20261107/results.tsv. */
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
typedef unsigned int uint;
MGLOBAL(int, DAT_00773990, 0x00773990);
uint __fastcall __fload_withFB();
uint FUN_004a70bc(void);
uint __math_exit(void);
uint FUN_004a70a5(void);
uint __startOneArgErrorHandling(void);
// MATCH: 0x005c2f84 FUN_005c2f84
uint FUN_005c2f84(uint param_1,uint param_2)

{
  uint uVar1;
  undefined1 in_ZF;
  short in_FPUControlWord;
  
  __fload_withFB();
  if ((bool)in_ZF) {
    if (((param_2 & 0xfffff) != 0) || (param_1 != 0)) {
      uVar1 = FUN_004a70bc();
      goto LAB_005c300b;
    }
    param_1 = param_2 & 0x80000000;
    param_2 = 0;
joined_r0x005c2ffc:
    if (param_1 == 0) {
LAB_005c2fae:
      if (DAT_00773990 != 0) {
        return param_2;
      }
      uVar1 = __math_exit();
      return uVar1;
    }
  }
  else {
    if (in_FPUControlWord != 0x27f) {
      param_2 = FUN_004a70a5();
    }
    if ((param_2 & 0x80000000) == 0) goto LAB_005c2fae;
    if (((param_2 & 0x7ff00000) == 0) && ((param_2 & 0xfffff) == 0)) goto joined_r0x005c2ffc;
  }
  uVar1 = 1;
LAB_005c300b:
  if (DAT_00773990 != 0) {
    return uVar1;
  }
  uVar1 = __startOneArgErrorHandling();
  return uVar1;
}
