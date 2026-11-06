// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x004624c0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
MGLOBAL(undefined4, DAT_0068f4cc, 0x0068f4cc);
int FID_conflict__wprintf(char * p0, ...);
undefined4 FUN_00550430(undefined4 * p0);
int FUN_00551190(int p0, undefined4 p1, undefined4 p2);
// MATCH: 0x004624c0 FUN_004624c0
void FUN_004624c0(void)

{
  int iVar1;
  
  DAT_0068f4cc = FUN_00551190(5,0x5cc4de,0x5cc4de);
  iVar1 = FUN_00550430(DAT_0068f4cc);
  if (iVar1 != 0) {
    FID_conflict__wprintf("RtFSManagerRegister successful\n");
    return;
  }
  FID_conflict__wprintf("RtFSManagerRegister failed\n");
  return;
}
