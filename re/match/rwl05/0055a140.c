// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR04 2026-11-08: 100% (regs 100%), relocheck OK, C++ check OK, was 12.3%. Returns param 1 (Ghidra typed it void). Supersedes the header below. */
/* MATCH-RWL05 0x0055a140: hand-matched (h1). 12.3% (regs 95.4%, ops 98.5%), 65/65 ins. Same instruction stream; register assignment differs only (original: param_1 in eax, param_2 in esi, ours param_2 in edx and param_1 in esi). ushort == 0xffff compares, ushort loaded into unsigned local, while-loop form, permuter (26%) did not move it. Class (b). */
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
// MATCH: 0x0055a140 FUN_0055a140
int FUN_0055a140(int param_1,int param_2)
{
  int *piVar1;
  unsigned int *puVar2;
  int *piVar3;
  int iVar4;
  unsigned int u;

  if (*(unsigned short *)(param_2 + 0x54) != 0xffff) {
    u = *(unsigned short *)(*(int *)(param_2 + 0x50) + 0x20);
    piVar1 = (int *)(*(int *)(param_1 + 0x20) + u * 4);
    piVar3 = (int *)*piVar1;
    while (piVar3 != 0 && *piVar3 != param_2) {
      piVar1 = piVar3 + 1;
      piVar3 = (int *)*piVar1;
    }
    iVar4 = *piVar1;
    *piVar1 = *(int *)(iVar4 + 4);
    *(int *)(iVar4 + 4) = *(int *)(param_1 + 0x1c);
    *(int *)(param_1 + 0x1c) = iVar4;
  }
  if (*(unsigned short *)(param_2 + 0x5c) != 0xffff) {
    u = *(unsigned short *)(*(int *)(param_2 + 0x58) + 0x20);
    piVar1 = (int *)(*(int *)(param_1 + 0x20) + u * 4);
    piVar3 = (int *)*piVar1;
    while (piVar3 != 0 && *piVar3 != param_2) {
      piVar1 = piVar3 + 1;
      piVar3 = (int *)*piVar1;
    }
    iVar4 = *piVar1;
    *piVar1 = *(int *)(iVar4 + 4);
    *(int *)(iVar4 + 4) = *(int *)(param_1 + 0x1c);
    *(int *)(param_1 + 0x1c) = iVar4;
  }
  u = *(unsigned short *)(param_2 + 4);
  puVar2 = (unsigned int *)(*(int *)(param_1 + 0x6c) + (u >> 5) * 4);
  *puVar2 = *puVar2 & ~(1 << (u & 0x1f));
  *(unsigned short *)(param_2 + 4) = 0xffff;
  return param_1;
}
