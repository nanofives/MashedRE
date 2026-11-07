// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f4470: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs22_20261107/results.tsv. */
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
typedef int code(int, undefined4 *);
MGLOBAL(undefined4 *, DAT_007d7298, 0x007d7298);
MGLOBAL(int, DAT_007d729c, 0x007d729c);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ae0, 0x00911ae0);
undefined4 FUN_004cc9f0(int p0);
void FUN_004d41e0(int p0);
void FUN_004f07d0(int p0);
void FUN_004f4900(void);
void FUN_004f8730(void);
// MATCH: 0x004f4470 FUN_004f4470
void FUN_004f4470(void)

{
  int iVar1;
  undefined4 *puVar2;
  
  puVar2 = DAT_007d7298;
  if (DAT_007d7298 != (undefined4 *)0x0) {
    do {
      *puVar2 = 0;
      (**(code **)(DAT_007d3ff8 + 0x11c))(DAT_007d729c,puVar2);
      puVar2 = (undefined4 *)puVar2[1];
    } while (puVar2 != (undefined4 *)0x0);
    DAT_007d7298 = (undefined4 *)0x0;
  }
  if (DAT_007d729c != 0) {
    FUN_004cc9f0(DAT_007d729c);
    DAT_007d729c = 0;
  }
  FUN_004f8730();
  FUN_004f4900();
  FUN_004f07d0(0);
  iVar1 = *(int *)(DAT_00911ae0 + 0x54 + DAT_007d3ff8);
  if (iVar1 != 0) {
    FUN_004d41e0(iVar1);
    *(undefined4 *)(DAT_00911ae0 + 0x54 + DAT_007d3ff8) = 0;
  }
  return;
}
