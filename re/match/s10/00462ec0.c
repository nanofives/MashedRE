// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00462ec0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
MGLOBAL(int, DAT_006904f0, 0x006904f0);
MGLOBAL(undefined1, DAT_006036d0, 0x006036d0);
MGLOBAL(undefined4, DAT_0069045c, 0x0069045c);
MGLOBAL(undefined4, DAT_006904c4, 0x006904c4);
MGLOBAL(undefined4, DAT_0088f680, 0x0088f680);
MGLOBAL(undefined4, DAT_008aa2e0, 0x008aa2e0);
int FID_conflict__wprintf(char * p0, ...);
undefined4 FUN_004627b0(void);
void FUN_00550be0(void);
void FUN_005aaa00(int * p0, undefined4 p1, undefined4 p2);
MGLOBAL(int, _DAT_006036d0, 0x006036d0);
// MATCH: 0x00462ec0 FUN_00462ec0
/* [C2 2026-06-01] No parameters. Body 0x00462ec0..0x00462f43. Tears down the 4 RWS streams created
   by FUN_00462dd0. Source… */

void FUN_00462ec0(void)

{
  int iVar1;
  undefined4 *puVar2;
  
  iVar1 = FUN_004627b0();
  if ((iVar1 == 0) && (DAT_006904f0 == 1)) {
    FID_conflict__wprintf("\\toast\\Code\\src\\AppCode\\AUDIO\\soundengine.c: DestroyStreams\n");
    FUN_00550be0();
    puVar2 = &DAT_0069045c;
    do {
      FUN_005aaa00(*puVar2,0,0);
      puVar2 = puVar2 + 1;
    } while ((int)puVar2 < 0x69046c);
    iVar1 = 0;
    do {
      if (*(int *)((int)&DAT_0088f680 + iVar1) != 0) {
        *(undefined4 *)(*(int *)((int)&DAT_0088f680 + iVar1) + 0xc) = 0;
        *(undefined4 *)((int)&DAT_008aa2e0 + iVar1) = 0;
      }
      *(undefined4 *)((int)&DAT_006904c4 + iVar1) = 0;
      iVar1 = iVar1 + 4;
    } while (iVar1 < 0x18);
    DAT_006904f0 = 0;
    _DAT_006036d0 = 1;
  }
  return;
}
