// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00518080: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs23_20261107/results.tsv. */
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
void FUN_00516de0(int p0, int p1, uint p2, int p3);
void FUN_00518140(undefined4 * p0, int p1, int p2);
void FUN_00520870(void * p0, undefined * p1, undefined4 p2);
// MATCH: 0x00518080 FUN_00518080

void FUN_00518080(int *param_1,int *param_2,int *param_3)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  undefined4 local_4;
  
  iVar3 = 0;
  iVar4 = 0;
  local_4 = 0;
  if (param_1 != (int *)0x0) {
    iVar3 = *param_1;
  }
  if (param_2 != (int *)0x0) {
    iVar4 = *param_2;
  }
  if (param_3 != (int *)0x0) {
    local_4 = *param_3;
  }
  uVar1 = *(undefined4 *)(iVar3 + 0x1f0);
  uVar2 = *(undefined4 *)(iVar3 + 0x1e8);
  FUN_00518140(iVar3,iVar4,local_4);
  if (iVar4 != 0) {
    FUN_00516de0(iVar3,iVar4,0x4000,0xffffffff);
    FUN_00520870(iVar4,uVar1,uVar2);
    *param_2 = 0;
  }
  if (local_4 != 0) {
    FUN_00516de0(iVar3,local_4,0x4000,0xffffffff);
    FUN_00520870(local_4,uVar1,uVar2);
    *param_3 = 0;
  }
  FUN_00520870(iVar3,uVar1,uVar2);
  *param_1 = 0;
  return;
}
