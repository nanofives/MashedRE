// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00555280: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs24_20261107/results.tsv. */
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
typedef int code();
MGLOBAL(undefined4 *, DAT_00912a18, 0x00912a18);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_00912a14, 0x00912a14);
MGLOBAL(int, DAT_00912a1c, 0x00912a1c);
MGLOBAL(undefined4, DAT_00912a20, 0x00912a20);
MGLOBAL(undefined4, DAT_00912a08, 0x00912a08);
MGLOBAL(int, DAT_0091325c, 0x0091325c);
MGLOBAL(int, DAT_00912a0c, 0x00912a0c);
MGLOBAL(undefined4, DAT_00912a10, 0x00912a10);
undefined4 FUN_004c5930(int p0);
undefined4 FUN_004cc9f0(int p0);
undefined4 FUN_00555830(int p0);
// MATCH: 0x00555280 FUN_00555280
void FUN_00555280(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  
  puVar2 = DAT_00912a18;
  while (puVar2 != (undefined4 *)0x0) {
    (**(code **)(DAT_007d3ff8 + 0x10c))(*puVar2);
    FUN_00555830(puVar2[1]);
    puVar1 = (undefined4 *)puVar2[2];
    (**(code **)(DAT_007d3ff8 + 0x11c))(DAT_00912a14,puVar2);
    puVar2 = puVar1;
  }
  DAT_00912a18 = (undefined4 *)0x0;
  if (DAT_00912a1c != 0) {
    FUN_004c5930(DAT_00912a1c);
  }
  DAT_00912a1c = 0;
  DAT_00912a20 = 0;
  FUN_004cc9f0(DAT_00912a14);
  DAT_00912a14 = 0;
  FUN_004cc9f0(DAT_00912a08);
  DAT_00912a08 = 0;
  if (DAT_0091325c != 0) {
    (**(code **)(DAT_007d3ff8 + 0x10c))(DAT_0091325c);
  }
  DAT_0091325c = 0;
  if (DAT_00912a0c != 0) {
    (**(code **)(DAT_007d3ff8 + 0x10c))(DAT_00912a0c);
  }
  DAT_00912a0c = 0;
  DAT_00912a10 = 0;
  return;
}
