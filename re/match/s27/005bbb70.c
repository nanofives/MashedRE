// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005bbb70: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs27_20261107/results.tsv. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(undefined *, PTR_LAB_006351b8, 0x006351b8);
MGLOBAL(void *, PTR_LAB_006351e4, 0x006351e4);
MGLOBAL(undefined *, PTR_LAB_006351fc, 0x006351fc);
undefined4 FUN_005ad080(int p0, uint p1);
// MATCH: 0x005bbb70 FUN_005bbb70
undefined4 FUN_005bbb70(void)

{
  int iVar1;
  
  iVar1 = FUN_005ad080((int)&PTR_LAB_006351fc,4);
  if (iVar1 == 1) {
    iVar1 = FUN_005ad080((int)&PTR_LAB_006351b8,9);
    if (iVar1 == 1) {
      iVar1 = FUN_005ad080((int)&PTR_LAB_006351e4,2);
      if (iVar1 == 1) {
        return 1;
      }
    }
  }
  return 0;
}
