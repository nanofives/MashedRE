// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00425c00: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs05_20261031/results.tsv. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(uint, DAT_00616038, 0x00616038);
MGLOBAL(undefined4, DAT_00644150, 0x00644150);
MGLOBAL(undefined1, DAT_005cd29c, 0x005cd29c);
MGLOBAL(undefined4, DAT_00899260, 0x00899260);
void FUN_00425b90(undefined4 * p0);
undefined4 FUN_004781b0(undefined1 * p0, int p1);
undefined4 FUN_004a2b60(undefined1 * p0, const char * p1, ...);
void FUN_004c5c80(undefined4 p0);
// MATCH: 0x00425c00 FUN_00425c00
/* [C1 2026-05-20] Copter (helicopter) asset registration. Scans array starting at &DAT_00899260
   with stride 0x13 ints
 */

undefined4 FUN_00425c00(undefined4 param_1)

{
  int *piVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 *puVar4;
  uint unaff_retaddr;
  undefined1 local_44 [64];
  uint local_4;
  
  local_4 = DAT_00616038 ^ unaff_retaddr;
  puVar4 = (undefined4 *)0x0;
  FUN_004c5c80(DAT_00644150);
  iVar3 = 0;
  piVar1 = (int *)&DAT_00899260;
  do {
    if (*piVar1 == 0) {
      puVar4 = &DAT_00899260 + iVar3 * 0x13;
      break;
    }
    piVar1 = piVar1 + 0x13;
    iVar3 = iVar3 + 1;
  } while ((int)piVar1 < 0x8994c0);
  FUN_004a2b60(local_44,"%s%s%s%s","ToastArt/common/perm/","Copters/",param_1,&DAT_005cd29c);
  uVar2 = FUN_004781b0(local_44,0);
  *puVar4 = uVar2;
  FUN_00425b90(puVar4);
  FUN_004c5c80(0);
  return 1;
}
