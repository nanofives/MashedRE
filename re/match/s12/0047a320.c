// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x0047a320: best 95.2% (regs 95.2%) by hand (h6), raw converter 93.0%.
   FUN_004b7090 returns an x87 value that the original converts with __ftol (Ghidra showed the __ftol call FUN_004a2c48 as a separate no-arg call, so the raw form popped st(0) and called it again): declare FUN_004b7090 as returning double and write (int)FUN_004b7090(...).
   Tried: intrinsic strcpy forms (57-73%), pointer-to-count local before the copy (73.8%), struct G218 (66.7%),
   permuter 155 compiles (no gain). Residual (b): the add esi, 0x218 (pointer to the count) is hoisted above the
   copy loop in ours and sits after the push args of FUN_004b7090 in the original, plus the loop alignment pad that
   moves with it. */
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
typedef unsigned __int64 ulonglong;
typedef long double float10;
MGLOBAL(int, DAT_006bf1cc, 0x006bf1cc);
MGLOBAL(int, DAT_006bf1c8, 0x006bf1c8);
int FUN_004b6fc0(undefined4 p0);
double FUN_004b7090(undefined4 p0, int p1);
int FUN_004b70d0(undefined4 p0, undefined4 p1);
// MATCH: 0x0047a320 FUN_0047a320
undefined4 FUN_0047a320(undefined4 param_1)

{
  char cVar1;
  int iVar2;
  int iVar3;
  char *pcVar4;
  undefined4 uVar5;
  char *pcVar6;
  int *piVar7;
  
  iVar3 = FUN_004b6fc0(param_1);
  pcVar4 = (char *)FUN_004b70d0(param_1,iVar3 + -1);
  iVar2 = DAT_006bf1cc;
  pcVar6 = (char *)((*(int *)(DAT_006bf1cc + 0x218) + 0xa3) * 0x40 + DAT_006bf1c8);
  do {
    cVar1 = *pcVar4;
    pcVar4 = pcVar4 + 1;
    *pcVar6 = cVar1;
    pcVar6 = pcVar6 + 1;
  } while (cVar1 != '\0');
  piVar7 = (int *)(iVar2 + 0x218);
  uVar5 = (int)FUN_004b7090(param_1,iVar3);
  *(undefined4 *)(DAT_006bf1c8 + 0x2ac0 + *piVar7 * 4) = uVar5;
  *piVar7 = *piVar7 + 1;
  return 0;
}
