// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00476390 (h2): 100% by bytes. Instruction score 93.6% only because Ghidra's size (146 B) stops
   before pop edi/pop esi/ret; match.py --bytes: 149 B whole symbol BYTE-IDENTICAL. Fix vs raw (91.5%): typed
   0x40-byte record arrays with signed (int) pointer compares, so the end addresses are relocated, not literals.
   Residual: none. */
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
typedef struct { unsigned char b[0x40]; } Rec40;
MGLOBALA(Rec40, DAT_00691618, 2, 0x00691618);
MGLOBALA(Rec40, DAT_00692418, 2, 0x00692418);
MGLOBAL(Rec40, DAT_00692498, 0x00692498);
undefined4 FUN_0040bb30(char *p0);
void FUN_00475ef0(void);
void FUN_00476cb0(Rec40 *p0, int p1, int p2);
void FUN_004770c0(Rec40 *p0, uint p1, uint p2, undefined4 p3);
// MATCH: 0x00476390 FUN_00476390
void FUN_00476390(void)
{
  undefined4 uVar1;
  Rec40 *p;

  FUN_00475ef0();
  uVar1 = FUN_0040bb30("flames3");
  for (p = DAT_00691618; (int)p < (int)(DAT_00691618 + 2); p++) {
    FUN_004770c0(p, 0x817, 0x40, uVar1);
    FUN_00476cb0(p, 5, 2);
  }
  uVar1 = FUN_0040bb30("smoke");
  for (p = DAT_00692418; (int)p < (int)(DAT_00692418 + 2); p++) {
    FUN_004770c0(p, 0x817, 0x40, uVar1);
  }
  FUN_004770c0(&DAT_00692498, 0x807, 0x40, 0);
}
