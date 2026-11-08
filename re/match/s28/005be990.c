// FLAGS: /O2 /Ob1 /G5
// best 48.5 (regs 72.7). Original keeps the flag in the param slot [esp+0xc] (written with eax=1, reused as flag) and only esi/edi; ours takes ebx and cmp-imm. Permuter best (62.5) changed semantics, rejected. Class (b).
/* MATCH-S28 0x005be990: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs28_20261107/results.tsv. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;

typedef long LONG;
MGLOBAL(undefined1, DAT_007de094, 0x007de094);
void FUN_005ae920(undefined1 * p0, int p1);
void FUN_005be930(int p0);
LONG __stdcall FUN_005bf690(int p0);
// MATCH: 0x005be990 FUN_005be990
undefined4 __stdcall FUN_005be990(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = param_1;
  if (*(int *)(param_1 + 4) == 1) {
    if (*(int *)(param_1 + 8) < 0) {
      *(undefined4 *)(param_1 + 8) = 0;
      param_1 = 1;
    }
    (**(void (__stdcall **)(int *))(**(int **)(iVar1 + 0xc) + 8))(*(int **)(iVar1 + 0xc));
    FUN_005be930(iVar1);
  }
  uVar2 = FUN_005bf690(iVar1);
  if (param_1 != 0) {
    FUN_005ae920(&DAT_007de094,iVar1);
  }
  return uVar2;
}
