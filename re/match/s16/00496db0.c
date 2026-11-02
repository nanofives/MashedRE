// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00496db0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs16_20261102/results.tsv. */
/* ---- MATCH-S16 preamble (same text in every re/match/s16 source; not part of any function body) ----
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
/* C++-compat (MATCH-S16 swap): every callee typed int-in/int-out, FUN_004d4f90 with its 4 cdecl args */
int FUN_004d4170(void);
void FUN_004d41e0(int p0);
int FUN_004d4380(int p0);
int FUN_004d4dd0(int p0);
int FUN_004d4f90(int p0, int p1, int p2, int p3);
int FUN_004eb9d0(void);
// MATCH: 0x00496db0 FUN_00496db0
int FUN_00496db0(void)

{
  int iVar1;
  int iVar2;
  int uVar3;
  
  iVar1 = FUN_004d4170();
  if (iVar1 != 0) {
    iVar2 = FUN_004d4dd0(iVar1);
    if (iVar2 != 0) {
      uVar3 = FUN_004eb9d0();
      uVar3 = FUN_004d4f90(iVar2,0,uVar3,0);
      iVar2 = FUN_004d4380(uVar3);
      if (iVar2 != 0) {
        return iVar1;
      }
    }
    FUN_004d41e0(iVar1);
  }
  return 0;
}
