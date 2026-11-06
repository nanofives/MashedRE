// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00462dd0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(undefined4, DAT_0069049c, 0x0069049c);
MGLOBAL(undefined1, DAT_0068fc9c, 0x0068fc9c);
MGLOBAL(undefined1, DAT_0068fca4, 0x0068fca4);
MGLOBAL(undefined1, DAT_0068fca8, 0x0068fca8);
MGLOBAL(undefined1, DAT_0068fca0, 0x0068fca0);
MGLOBAL(undefined1, DAT_0068fcac, 0x0068fcac);
MGLOBAL(undefined1, DAT_0068fcb0, 0x0068fcb0);
MGLOBAL(undefined1, DAT_0068fcb4, 0x0068fcb4);
MGLOBAL(undefined1, DAT_0068fcb8, 0x0068fcb8);
MGLOBAL(undefined1, DAT_0068fc98, 0x0068fc98);
MGLOBAL(undefined1, DAT_006036d0, 0x006036d0);
MGLOBAL(undefined1, DAT_00690158, 0x00690158);
MGLOBAL(undefined4, DAT_0069045c, 0x0069045c);
MGLOBAL(undefined4, DAT_007f1a1c, 0x007f1a1c);
int FID_conflict__wprintf(char * p0, ...);
undefined4 FUN_004627b0(void);
undefined4 FUN_005a7f70(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
void FUN_005a8890(int p0, undefined4 * p1);
MGLOBAL(int, _DAT_0068fc9c, 0x0068fc9c);
MGLOBAL(int, _DAT_0068fca4, 0x0068fca4);
MGLOBAL(int, _DAT_0068fca8, 0x0068fca8);
MGLOBAL(int, _DAT_0068fca0, 0x0068fca0);
MGLOBAL(int, _DAT_0068fcac, 0x0068fcac);
MGLOBAL(int, _DAT_0068fcb0, 0x0068fcb0);
MGLOBAL(int, _DAT_0068fcb4, 0x0068fcb4);
MGLOBAL(int, _DAT_0068fcb8, 0x0068fcb8);
MGLOBAL(int, _DAT_0068fc98, 0x0068fc98);
MGLOBAL(int, _DAT_006036d0, 0x006036d0);
// MATCH: 0x00462dd0 FUN_00462dd0
/* [C2 2026-06-01] No parameters. Body 0x00462dd0..0x00462ebf. Creates the 4 music/dialogue RWS
   streams. Source file string… */

void FUN_00462dd0(void)

{
  int iVar1;
  undefined4 uVar2;
  int *piVar3;
  undefined4 *puVar4;
  
  if (DAT_006904f0 != 1) {
    iVar1 = FUN_004627b0();
    if (iVar1 == 0) {
      FID_conflict__wprintf("\\toast\\Code\\src\\AppCode\\AUDIO\\soundengine.c: CreateStream\n");
      iVar1 = 0;
      do {
        uVar2 = FUN_005a7f70(DAT_0069049c,0,0,0,0);
        (&DAT_0069045c)[iVar1] = uVar2;
        FID_conflict__wprintf("CreateStream %d Done\n",iVar1);
        iVar1 = iVar1 + 1;
      } while (iVar1 < 4);
      FID_conflict__wprintf
                ("\\toast\\Code\\src\\AppCode\\AUDIO\\soundengine.c: CreateStream Done\n");
      _DAT_0068fc9c = 1;
      _DAT_0068fca4 = 0;
      _DAT_0068fca8 = 0;
      _DAT_0068fca0 = 0;
      _DAT_0068fcac = 0;
      _DAT_0068fcb0 = 4;
      _DAT_0068fcb4 = 2;
      _DAT_0068fcb8 = 1;
      puVar4 = &DAT_0069045c;
      piVar3 = &DAT_007f1a1c;
      do {
        _DAT_0068fc98 = &DAT_00690158 + *piVar3 * 0x80;
        FUN_005a8890(*puVar4,&DAT_0068fc98);
        piVar3 = piVar3 + 4;
        puVar4 = puVar4 + 1;
      } while ((int)piVar3 < 0x7f1a5c);
      _DAT_006036d0 = 0;
      DAT_006904f0 = 1;
    }
  }
  return;
}
