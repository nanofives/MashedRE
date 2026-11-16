// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005b35a0: was 60.0% with the *param_1 increment before the head stores (a permuter move that changed the store order, equiv.py ret-memory differs). PORT-TOP-AUDIOSTOP-5A61B0 (2026-11-16): increment last, the original order (load/inc/store at 0x005b35cc/0x005b35cf/0x005b35d0, after the head stores at 0x005b35c7/0x005b35ca; capstone on MASHED.exe.unpatched); 55.0% (regs 100%, ops 100%), residual is the eax/edx role swap of param_1 and param_1+4 plus the increment temp (scratch rotation), 36 source forms tried. equiv.py: EQUIV-IF-CALLERS-IGNORE(EAX,EDX,ECX), caller 0x005ad893 EDX unknown (0x005ad770 returns at 0x005ad8a2 with edx/ecx untouched after the call, so they reach 0x005ad770's own callers).
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
// MATCH: 0x005b35a0 FUN_005b35a0
void FUN_005b35a0(int *param_1,int param_2)

{
  int *piVar1;
  int *piVar2;
  
  piVar1 = param_1 + 1;
  piVar2 = (int *)(param_2 + 0x20);
  *(undefined4 *)(param_2 + 0x24) = 0;
  *piVar2 = 0;
  *piVar2 = *piVar1;
  *(int **)(param_2 + 0x24) = piVar1;
  *(int **)(*piVar1 + 4) = piVar2;
  *piVar1 = (int)piVar2;
  *param_1 = *param_1 + 1;
  return;
}
