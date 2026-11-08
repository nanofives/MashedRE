// FLAGS: /O2 /Ob1 /G5
/* C++-check edits: casts/prototypes only, DDK bytes unchanged */
/* MATCH-S28 0x005c96e0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(undefined1, DAT_005e7abc, 0x005e7abc);
MGLOBAL(void *, PTR_DAT_006363b8, 0x006363b8);
MGLOBAL(void *, PTR_DAT_00636430, 0x00636430);
MGLOBAL(undefined1, DAT_007de374, 0x007de374);
int FUN_005ad420(int p0, int p1, uint p2, int p3, uint p4);
undefined4 FUN_005ad540(undefined4 p0, undefined4 p1, undefined4 p2);
int FUN_005ad570(int p0);
// MATCH: 0x005c96e0 FUN_005c96e0
undefined * FUN_005c96e0(void)

{
  int iVar1;
  
  iVar1 = FUN_005ad570((int)&DAT_007de374);
  if (iVar1 == 0) {
    return (undefined *)0x0;
  }
  FUN_005ad540((undefined4)&DAT_007de374,(undefined4)&DAT_005e7abc,0);
  FUN_005ad420((int)&DAT_007de374,(int)&PTR_DAT_006363b8,6,(int)&PTR_DAT_00636430,5);
  return &DAT_007de374;
}
