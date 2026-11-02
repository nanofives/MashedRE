// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x0047a950: 100% (regs 100%), whole obj symbol BYTE-IDENTICAL over 93 B (relocations masked) by hand (h6).
   0x0047a950 has no row in re/console/pc_funcs.csv, so match.py refuses it ("no Ghidra size"); scored with match.py's
   compare() and an explicit size of 93 B (odis.py shows the ret at +92).
   FUN_004b7090 returns an x87 value that the original converts with __ftol (Ghidra showed the __ftol call FUN_004a2c48 as a separate no-arg call, so the raw form popped st(0) and called it again): declare FUN_004b7090 as returning double and write (int)FUN_004b7090(...). Residual: none. */
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
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned __int64 ulonglong;
typedef long double float10;
MGLOBAL(int, DAT_006bf1cc, 0x006bf1cc);
int FUN_004b6fc0(undefined4 p0);
double FUN_004b7090(undefined4 p0, int p1);
// MATCH: 0x0047a950 FUN_0047a950

undefined4 FUN_0047a950(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  undefined1 uVar3;
  int iVar4;
  
  iVar4 = FUN_004b6fc0(param_1);
  iVar2 = *(int *)(DAT_006bf1cc + 0x6c);
  uVar3 = (int)FUN_004b7090(param_1,iVar4 + -1);
  iVar1 = DAT_006bf1cc;
  *(undefined1 *)(DAT_006bf1cc + 0x2c + iVar2 * 4) = uVar3;
  iVar1 = iVar1 + iVar2 * 4;
  uVar3 = (int)FUN_004b7090(param_1,iVar4);
  *(undefined1 *)(iVar1 + 0x2d) = uVar3;
  iVar2 = DAT_006bf1cc;
  *(undefined2 *)(iVar1 + 0x2e) = 0xffff;
  *(int *)(iVar2 + 0x6c) = *(int *)(iVar2 + 0x6c) + 1;
  return 0;
}
