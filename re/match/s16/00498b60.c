// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00498b60: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(int, DAT_00773410, 0x00773410);
MGLOBAL(void *, DAT_00773408, 0x00773408);
MGLOBAL(void *, DAT_007731f8, 0x007731f8);
MGLOBAL(void *, DAT_007731fc, 0x007731fc);
void _free(void * p0);
// MATCH: 0x00498b60 FUN_00498b60
void FUN_00498b60(void)

{
  int iVar1;
  
  iVar1 = 0;
  if (0 < DAT_00773410) {
    do {
      _free(*(void **)((int)DAT_00773408 + iVar1 * 4));
      iVar1 = iVar1 + 1;
    } while (iVar1 < DAT_00773410);
  }
  _free(DAT_00773408);
  _free(DAT_007731f8);
  _free(DAT_007731fc);
  return;
}
