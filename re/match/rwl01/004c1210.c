// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004c1210: best 98.4% (regs 100%, ops 100%), class (a)/(b). The file holds the static callee 0x004c0ad0 (100%): defining it first lets the compiler keep ecx/edx live across the call (extraout_ECX/EDX in the Ghidra text). Only the base register of one [+0x9c] load in the list-search loop differs (2 loop spellings tried).
   MATCH-RACEPATH-X87A 2026-11-09: CLOSED, 232 B BYTE-IDENTICAL. The search walks one pointer with an empty-body
   for, `for (; *(int *)(iVar2 + 0x9c) != param_1; iVar2 = *(int *)(iVar2 + 0x9c)) ;`, so the [+0x9c] load uses the
   copy (edx) as at 0x004c123c. The same walk as a while loop is 77.4%, the two-variable for (cursor + next) 98.4%. */
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
typedef unsigned char byte;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x004c0ad0 _FUN_004c0ad0
static void FUN_004c0ad0(int p0, int p1)
{
  int iVar1;
  iVar1 = *(int *)(p0 + 0x98);
  *(int *)(p0 + 0xa0) = p1;
  for (; iVar1 != 0; iVar1 = *(int *)(iVar1 + 0x9c)) {
    FUN_004c0ad0(iVar1,p1);
  }
}

// MATCH: 0x004c1210 FUN_004c1210
int FUN_004c1210(int param_1)

{
  uint bVar1;
  int iVar2;
  
  iVar2 = *(int *)(*(int *)(param_1 + 4) + 0x98);
  if (iVar2 == param_1) {
    *(undefined4 *)(*(int *)(param_1 + 4) + 0x98) = *(undefined4 *)(param_1 + 0x9c);
  }
  else {
    for (; *(int *)(iVar2 + 0x9c) != param_1; iVar2 = *(int *)(iVar2 + 0x9c)) ;
    *(undefined4 *)(iVar2 + 0x9c) = *(undefined4 *)(param_1 + 0x9c);
  }
  iVar2 = *(int *)(param_1 + 0x98);
  *(int *)(param_1 + 0xa0) = param_1;
  *(undefined4 *)(param_1 + 4) = 0;
  *(undefined4 *)(param_1 + 0x9c) = 0;
  for (; iVar2 != 0; iVar2 = *(int *)(iVar2 + 0x9c)) {
    FUN_004c0ad0(iVar2,param_1);
  }
  bVar1 = (uint)*(byte *)(*(int *)(param_1 + 0xa0) + 3);
  if ((bVar1 & 3) == 0) {
    *(undefined4 *)(*(int *)(param_1 + 0xa0) + 8) = *(undefined4 *)(DAT_007d3ff8 + 0xbc);
    *(int *)(*(int *)(param_1 + 0xa0) + 0xc) = DAT_007d3ff8 + 0xbc;
    *(int *)(*(int *)(DAT_007d3ff8 + 0xbc) + 4) = *(int *)(param_1 + 0xa0) + 8;
    *(int *)(DAT_007d3ff8 + 0xbc) = *(int *)(param_1 + 0xa0) + 8;
  }
  *(byte *)(*(int *)(param_1 + 0xa0) + 3) = (byte)(bVar1 | 3);
  *(byte *)(param_1 + 3) = *(byte *)(param_1 + 3) | 0xc;
  return param_1;
}
