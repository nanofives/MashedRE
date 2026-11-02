// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044c8b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
MGLOBAL(undefined4, DAT_005f9724, 0x005f9724);
MGLOBAL(undefined1, DAT_00896120, 0x00896120);
void FUN_0044c4f0(undefined4 * p0, undefined4 p1, undefined4 p2, undefined4 p3, int p4);
undefined4 FUN_004e4800(int p0);
int FUN_004e6ab0(int p0);
int RwFrameAddChild(int p0, int p1);
// MATCH: 0x0044c8b0 FUN_0044c8b0
void FUN_0044c8b0(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  undefined *puVar3;
  int iVar4;
  
  iVar4 = 0;
  puVar3 = &DAT_00896120;
  do {
    uVar1 = FUN_004e6ab0(*(undefined4 *)(param_1 + 0x10118));
    iVar2 = FUN_004e4800(uVar1);
    if (iVar2 == 0) {
      RwFrameAddChild(*(undefined4 *)(param_1 + 0x105d4),uVar1);
    }
    FUN_0044c4f0(puVar3,*(undefined4 *)(param_1 + 0x100b8),uVar1,(&DAT_005f9724)[iVar4],iVar4);
    iVar4 = iVar4 + 1;
    puVar3 = puVar3 + 0x58;
  } while (iVar4 < 3);
  return;
}
