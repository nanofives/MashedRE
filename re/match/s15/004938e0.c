// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x004938e0 (h8): best 80.0% (regs 90%), the raw converter's while-condition form (re-read *piVar2).
   Tried: do{c=a;a=*c}while(c!=a) in pointer and int types (60-70%, regs 100%: loop value and result land in the
   swapped eax/ecx pair), __inline static root-walk helpers, while/for forms (14 ins), permuter (40%).
   Residual (b): register assignment of the two loop temporaries (original keeps next in ecx, current in eax). */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
// MATCH: 0x004938e0 FUN_004938e0
void FUN_004938e0(undefined4 *param_1,undefined4 param_2)

{
  int *piVar1;
  int *piVar2;
  
  piVar1 = (int *)*param_1;
  do {
    piVar2 = piVar1;
    piVar1 = (int *)*piVar2;
  } while (piVar2 != (int *)*piVar2);
  *(undefined4 *)(DAT_00911ae4 + (int)piVar2) = param_2;
  return;
}
