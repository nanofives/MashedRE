// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00425ef0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(int, DAT_008992a4, 0x008992a4);
MGLOBAL(int, DAT_00899260, 0x00899260);
MGLOBAL(int, DAT_008992f0, 0x008992f0);
MGLOBAL(int, DAT_008992ac, 0x008992ac);
MGLOBAL(int, DAT_0089933c, 0x0089933c);
MGLOBAL(int, DAT_008992f8, 0x008992f8);
MGLOBAL(int, DAT_00899388, 0x00899388);
MGLOBAL(int, DAT_00899344, 0x00899344);
MGLOBAL(int, DAT_008993d4, 0x008993d4);
MGLOBAL(int, DAT_00899390, 0x00899390);
MGLOBAL(int, DAT_00899420, 0x00899420);
MGLOBAL(int, DAT_008993dc, 0x008993dc);
MGLOBAL(int, DAT_0089946c, 0x0089946c);
MGLOBAL(int, DAT_00899428, 0x00899428);
MGLOBAL(int, DAT_008994b8, 0x008994b8);
MGLOBAL(int, DAT_00899474, 0x00899474);
// MATCH: 0x00425ef0 FUN_00425ef0
int FUN_00425ef0(void)

{
  int iVar1;
  
  iVar1 = 0;
  if ((DAT_008992a4 != 0) && (DAT_00899260 != 0)) {
    iVar1 = 1;
  }
  if ((DAT_008992f0 != 0) && (DAT_008992ac != 0)) {
    iVar1 = iVar1 + 1;
  }
  if ((DAT_0089933c != 0) && (DAT_008992f8 != 0)) {
    iVar1 = iVar1 + 1;
  }
  if ((DAT_00899388 != 0) && (DAT_00899344 != 0)) {
    iVar1 = iVar1 + 1;
  }
  if ((DAT_008993d4 != 0) && (DAT_00899390 != 0)) {
    iVar1 = iVar1 + 1;
  }
  if ((DAT_00899420 != 0) && (DAT_008993dc != 0)) {
    iVar1 = iVar1 + 1;
  }
  if ((DAT_0089946c != 0) && (DAT_00899428 != 0)) {
    iVar1 = iVar1 + 1;
  }
  if ((DAT_008994b8 != 0) && (DAT_00899474 != 0)) {
    iVar1 = iVar1 + 1;
  }
  return iVar1;
}
