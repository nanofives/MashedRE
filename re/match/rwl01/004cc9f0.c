// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004cc9f0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
typedef unsigned char byte;
typedef int code();
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d45fc, 0x007d45fc);
// MATCH: 0x004cc9f0 FUN_004cc9f0
undefined4 FUN_004cc9f0(int param_1)

{
  int *piVar1;
  int *piVar2;
  
  piVar1 = (int *)(param_1 + 0x10);
  **(undefined4 **)(param_1 + 0x20) = *(undefined4 *)(param_1 + 0x1c);
  *(undefined4 *)(*(int *)(param_1 + 0x1c) + 4) = *(undefined4 *)(param_1 + 0x20);
  piVar2 = (int *)*piVar1;
  while (piVar2 != piVar1) {
    *(int *)piVar2[1] = *piVar2;
    *(int *)(*piVar2 + 4) = piVar2[1];
    (**(code **)(DAT_007d3ff8 + 0x10c))(piVar2);
    piVar2 = (int *)*piVar1;
  }
  if ((*(byte *)(param_1 + 0x18) & 1) == 0) {
    if ((DAT_007d45fc != param_1) && (DAT_007d45fc != 0)) {
      (**(code **)(DAT_007d3ff8 + 0x11c))(DAT_007d45fc,param_1);
      return 1;
    }
    (**(code **)(DAT_007d3ff8 + 0x10c))(param_1);
  }
  return 1;
}
