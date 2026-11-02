// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00475a00: 93.1% (regs 100%, ops 100%), relocheck OK. Raw g2s body (results.tsv: 93.1% too).
   Residual: the two stores use [edx+edi] / [eax+edi] where the original has [edi+edx] / [edi+eax] (SIB base and
   index swapped; edi = count*4 held across the call). Tried: every extern order, file-static subsets of all four
   globals, char-ptr, int-ptr and int typed globals with offset-first and global-first sums, array indexing (drops the shl,
   89.7%), a struct over 0x691604..0x691614 (72.4%), unsigned offset, permuter (113 compiles, no gain).
   Class (b). */
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
MGLOBAL(int, DAT_0069160c, 0x0069160c);
MGLOBAL(int, DAT_00691608, 0x00691608);
MGLOBAL(int, DAT_00691604, 0x00691604);
MGLOBAL(int, DAT_00691610, 0x00691610);
undefined4 FUN_00474db0(int p0, int p1);
void FUN_004770c0(int p0, uint p1, uint p2, undefined4 p3);
// MATCH: 0x00475a00 FUN_00475a00
void FUN_00475a00(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  
  FUN_004770c0(DAT_0069160c * 0x40 + DAT_00691608,0x80c,param_2,param_1);
  iVar2 = DAT_0069160c * 4;
  uVar1 = FUN_00474db0(param_2,0x50);
  *(undefined4 *)(iVar2 + DAT_00691604) = uVar1;
  *(undefined4 *)(iVar2 + DAT_00691610) = param_2;
  DAT_0069160c = DAT_0069160c + 1;
  return;
}
