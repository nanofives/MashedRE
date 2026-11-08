// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a6ea0 (h2): best 38.9% (regs 44.4%), class c. original truncates inline (fnstcw/or ah,0xc/fldcw/fistp) and uses fidiv dword on a zero-extended temp; DDK calls _ftol and fild qword. relocheck BAD only because the code differs from the original. */
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
typedef unsigned int uint;
typedef unsigned long DWORD;
MGLOBAL(uint, DAT_007dca54, 0x007dca54);
MGLOBAL(float, DAT_005cc9fc, 0x005cc9fc);
MGLOBAL(int, DAT_007dca50, 0x007dca50);
DWORD FUN_005ab040(void);
uint FUN_005ab070(void);
// MATCH: 0x005a6ea0 FUN_005a6ea0
void FUN_005a6ea0(void)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  float fVar4;

  uVar1 = FUN_005ab040();
  uVar2 = FUN_005ab070();
  uVar3 = uVar1 - DAT_007dca54;
  if (uVar1 < DAT_007dca54) {
    uVar3 = uVar3 - 1;
  }
  fVar4 = ((float)uVar3 / (float)uVar2) * DAT_005cc9fc;
  DAT_007dca54 = uVar1;
  DAT_007dca50 = DAT_007dca50 + (int)fVar4;
}
