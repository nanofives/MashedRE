// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045be90: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs10_20261102/results.tsv. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
typedef int code();
MGLOBAL(int, DAT_005f9bd8, 0x005f9bd8);
MGLOBAL(void *, PTR_FUN_005f99c0, 0x005f99c0);
MGLOBAL(undefined1, DAT_0088fbe0, 0x0088fbe0);
void FUN_0045b350(void);
void FUN_0045ba10(void);
void thunk_FUN_004548a0(void);
// MATCH: 0x0045be90 FUN_0045be90
void FUN_0045be90(void)

{
  int iVar1;
  undefined *puVar2;
  undefined **ppuVar3;
  
  iVar1 = 0;
  if (0 < DAT_005f9bd8) {
    ppuVar3 = &PTR_FUN_005f99c0;
    do {
      (*(code *)*ppuVar3)();
      iVar1 = iVar1 + 1;
      ppuVar3 = ppuVar3 + 0x10;
    } while (iVar1 < DAT_005f9bd8);
  }
  puVar2 = &DAT_0088fbe0;
  do {
    FUN_0045ba10();
    puVar2 = puVar2 + 0xb4;
  } while ((int)puVar2 < 0x88feb0);
  return;
}
