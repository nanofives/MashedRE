// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005ac880: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs25_20261107/results.tsv. */
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
typedef unsigned char byte;
typedef int code(int);
MGLOBAL(undefined1, DAT_007dd65c, 0x007dd65c);
void FUN_004522d0(int p0);
int FUN_005a7a60(int p0, int p1);
undefined4 FUN_005ae030(undefined4 p0);
void FUN_005ae7e0(int p0);
void FUN_005ae920(undefined1 * p0, uint p1);
// MATCH: 0x005ac880 FUN_005ac880
undefined4 FUN_005ac880(int param_1)

{
  int iVar1;
  
  iVar1 = FUN_005a7a60(param_1,1);
  if (iVar1 != 0) {
    return 0;
  }
  if (*(code **)(param_1 + 0x20) != (code *)0x0) {
    (**(code **)(param_1 + 0x20))(param_1);
  }
  if (*(int *)(param_1 + 0x3c) != 0) {
    FUN_005ae7e0(*(int *)(param_1 + 0x3c));
  }
  if ((*(byte *)(param_1 + 0x48) & 1) != 0) {
    FUN_004522d0(*(undefined4 *)(param_1 + 0x28));
  }
  **(undefined4 **)(param_1 + 0x44) = *(undefined4 *)(param_1 + 0x40);
  *(undefined4 *)(*(int *)(param_1 + 0x40) + 4) = *(undefined4 *)(param_1 + 0x44);
  FUN_005ae030(param_1);
  if ((*(byte *)(param_1 + 0x48) & 2) != 0) {
    FUN_005ae920(&DAT_007dd65c,param_1);
  }
  return 1;
}
