// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x0057c300: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl06_20261108/results.tsv. */
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
typedef unsigned short ushort;
typedef int code();
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined1, DAT_0062403c, 0x0062403c);
int FUN_0055ded0(int p0, ushort p1, ushort p2);
undefined4 FUN_0057c550(int p0);
// MATCH: 0x0057c300 FUN_0057c300

undefined4 * FUN_0057c300(int param_1)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)(**(code **)(DAT_007d3ff8 + 0x108))(0x34,0x30900);
  *puVar1 = &DAT_0062403c;
  FUN_0055ded0(puVar1,1,0);
  if (puVar1[1] != 0) {
    FUN_0057c550(puVar1[1]);
  }
  puVar1[1] = param_1;
  if (param_1 != 0) {
    *(int *)(param_1 + 0x50) = *(int *)(param_1 + 0x50) + 1;
  }
  *(undefined2 *)(puVar1 + 3) = 1;
  puVar1[2] = 0xb;
  *(undefined4 *)puVar1[4] = *(undefined4 *)(param_1 + 0x4c);
  puVar1[0xc] = 0;
  return puVar1;
}
