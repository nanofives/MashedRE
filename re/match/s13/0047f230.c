// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047f230: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs13_20261102/results.tsv. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef unsigned int uint;
MGLOBAL(undefined4, DAT_006c9a78, 0x006c9a78);
undefined4 FUN_0057c210(int p0);
// MATCH: 0x0047f230 FUN_0047f230
undefined4 FUN_0047f230(undefined4 *param_1,uint param_2)

{
  undefined4 *puVar1;
  int *piVar2;
  int iVar3;
  
  if (param_2 < 0x10) {
    iVar3 = FUN_0057c210((&DAT_006c9a78)[param_2]);
    piVar2 = *(int **)(*(int *)(iVar3 + 0x10) + 4);
    if (piVar2 != (int *)0x0) {
      puVar1 = (undefined4 *)(*(int *)(*(int *)(*piVar2 + 0x10) + 8) + 0x10 + piVar2[1] * 0x20);
      *param_1 = *puVar1;
      param_1[1] = puVar1[1];
      param_1[2] = puVar1[2];
      return 1;
    }
  }
  return 0;
}
