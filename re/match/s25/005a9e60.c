// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a9e60: hand-matched (h1). */
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

/* best 75.4% (regs 78.5%, ops 86.2%): residual = flag-or codegen. Original does "mov eax,[esi+0xc]; or al,0x40; mov [esi+0xc],eax" and
   reloads [esi+0x48] for the neg/sbb; ours uses ecx and "or ecx,0x40". Tried byte/uint shift forms, ternary result. Class (b). */
void *FUN_005ae250(undefined4 p0, int p1, int p2, int p3, int p4);
uint *FUN_005ae650(undefined4 p0, uint p1, uint p2, int p3, uint *p4, undefined4 p5);
// MATCH: 0x005a9e60 FUN_005a9e60
uint FUN_005a9e60(uint param_1,undefined4 param_2,int param_3)
{
  ushort uVar1;
  undefined4 uVar3;
  if (*(int *)(param_1 + 0x48) != 0) {
    return 0;
  }
  uVar1 = *(ushort *)(param_1 + 0x46);
  if (1 < uVar1) {
    if (param_3 != 0) {
      uVar3 = (undefined4)FUN_005ae650(param_2,uVar1,1 << (*(uint *)(param_1 + 0x40) >> 0x1c),1,0,0x30809);
      *(undefined4 *)(param_1 + 0x48) = uVar3;
      *(uint *)(param_1 + 0xc) = *(uint *)(param_1 + 0xc) | 0x40;
      return (*(int *)(param_1 + 0x48) != 0) ? param_1 : 0;
    }
    uVar3 = (undefined4)FUN_005ae250(param_2,uVar1,1 << (*(uint *)(param_1 + 0x40) >> 0x1c),1,0);
    *(undefined4 *)(param_1 + 0x48) = uVar3;
    *(uint *)(param_1 + 0xc) = *(uint *)(param_1 + 0xc) | 0x80;
    return (*(int *)(param_1 + 0x48) != 0) ? param_1 : 0;
  }
  *(undefined4 *)(param_1 + 0x48) = 1;
  return (*(int *)(param_1 + 0x48) != 0) ? param_1 : 0;
}
