// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00475a60: 100% (regs 100%), relocheck OK. Raw g2s body was 95.8%: the loads of 0x00691614 and
   0x0069160c after the call came out swapped (relocheck BAD). Declaration order of the externs does not move it
   (all 24 orders tried). Making DAT_0069160c file-static (pilot finding 2) gives 100%: under C it is
   `static int DAT_0069160c;`, under C++ the MGLOBAL. Residual: none. */
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
typedef unsigned int undefined4;
#ifndef __cplusplus
static int DAT_0069160c;
#else
MGLOBAL(int, DAT_0069160c, 0x0069160c);
#endif
MGLOBAL(int, DAT_00691610, 0x00691610);
MGLOBAL(int, DAT_00691604, 0x00691604);
MGLOBAL(int, DAT_00691614, 0x00691614);
void FUN_004b6520(undefined4 p0, undefined4 p1);
// MATCH: 0x00475a60 FUN_00475a60
void FUN_00475a60(void)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < DAT_0069160c) {
    do {
      FUN_004b6520(*(undefined4 *)(DAT_00691604 + iVar2 * 4),
                   *(int *)(DAT_00691610 + iVar2 * 4) * 0x50);
      iVar1 = DAT_0069160c;
      *(undefined4 *)(DAT_00691614 + iVar2 * 4) = 0;
      iVar2 = iVar2 + 1;
    } while (iVar2 < iVar1);
  }
  return;
}
