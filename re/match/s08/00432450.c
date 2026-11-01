// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00432450: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs08_20261101/results.tsv. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0067ecf0, 0x0067ecf0);
MGLOBAL(int, DAT_0067eabc, 0x0067eabc);
MGLOBAL(int, DAT_0067ecf4, 0x0067ecf4);
MGLOBAL(int, DAT_0067eac0, 0x0067eac0);
MGLOBAL(int, DAT_0067ece8, 0x0067ece8);
MGLOBAL(int, DAT_0067eab4, 0x0067eab4);
MGLOBAL(undefined4, DAT_0067eab0, 0x0067eab0);
MGLOBAL(undefined4, DAT_0067ece4, 0x0067ece4);
void FUN_0042bfb0();
void FUN_0042c1a0(void);
// MATCH: 0x00432450 FUN_00432450
void FUN_00432450(void)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  if (((DAT_0067ecf0 == DAT_0067eabc) && (DAT_0067ecf4 == DAT_0067eac0)) &&
     (DAT_0067ece8 == DAT_0067eab4)) {
    return;
  }
  puVar2 = &DAT_0067eab0;
  puVar3 = &DAT_0067ece4;
  for (iVar1 = 0xc; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
  }
  FUN_0042c1a0();
  FUN_0042bfb0();
  return;
}
