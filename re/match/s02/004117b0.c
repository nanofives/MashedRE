// FLAGS: /O2 /GS /Ob1
/* MATCH-S02 0x004117b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs02_20261031/results.tsv. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
typedef int code();
MGLOBAL(uint, DAT_00616038, 0x00616038);
MGLOBAL(int, DAT_005f29c8, 0x005f29c8);
MGLOBAL(int, DAT_0063bb10, 0x0063bb10);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_008a94a8, 0x008a94a8);
void FUN_004099a0(void);
undefined4 FUN_00426c00(void);
undefined4 FUN_00430820(void);
undefined4 FUN_00483ca0(int * p0, undefined4 * p1);
void FUN_004987b0(undefined4 p0);
// MATCH: 0x004117b0 FUN_004117b0
/* [C2 2026-05-18] Body (0x004117b0..0x00411865). Has security cookie wrapping. */



void FUN_004117b0(void)



{

  int iVar1;

  undefined4 uVar2;

  undefined4 *puVar3;

  uint unaff_retaddr;

  undefined1 local_44 [64];

  uint local_4;

  

  local_4 = DAT_00616038 ^ unaff_retaddr;

  if (DAT_005f29c8 == 0) {

    if (DAT_0063bb10 != 0) {

      iVar1 = FUN_00430820();

      if (iVar1 == 0) {

        puVar3 = (undefined4 *)(DAT_007d3ff8 + 0xc4);

        uVar2 = FUN_00426c00();

        (*(code *)*puVar3)(local_44,"%sReplay%d.rep","c:\\toast\\",uVar2);

        FUN_004987b0("saving replay %s\n",local_44);

        FUN_00483ca0(DAT_0063bb10,&DAT_008a94a8);

        (**(code **)(DAT_007d3ff8 + 0xc4))(local_44,"%sAll%d.rep","c:\\toast\\",0x10b);

        FUN_004099a0();

      }

    }

    DAT_005f29c8 = 1;

  }

  return;

}
