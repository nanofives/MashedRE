// FLAGS: /O2 /Ob1 /G5
// best 89.1 (ops 100, regs 98.2). Hand-written: switch(state), QueryInterface out-param = &param_1 (address-taken), info[12] stack struct. Residual: switch value in eax/base in ecx vs original base eax/state ecx. Class (b); permuter 87 compiles no gain.
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
MGLOBAL(undefined1, DAT_005d0b3c, 0x005d0b3c);
typedef int (__stdcall *QIFn)(int *, undefined1 *, int *);
typedef int (__stdcall *InfoFn)(int *, int, uint *);
// MATCH: 0x005bdff0 FUN_005bdff0
void FUN_005bdff0(int param_1, int param_2)
{
  uint info[12];
  int iVar2;
  int *pObj;
  uint *puVar1;

  switch (*(int *)(param_1 + 0x10)) {
  case 1:
    pObj = *(int **)(param_1 + 0xc);
    iVar2 = (*(QIFn *)*pObj)(pObj, &DAT_005d0b3c, &param_1);
    if (iVar2 >= 0) {
      iVar2 = (*(InfoFn *)(*(int *)param_1 + 0x4c))((int *)param_1, 0x30, info);
      if (iVar2 >= 0) {
        if (param_2 != 0) {
          info[2] = info[2] | 0x200;
        }
        else {
          info[2] = info[2] & 0xfffffdff;
        }
        (*(InfoFn *)(*(int *)param_1 + 0x50))((int *)param_1, 0x30, info);
      }
    }
    break;
  case 2:
    puVar1 = *(uint **)(param_1 + 0xc);
    if (param_2 != 0) {
      *puVar1 = *puVar1 | 8;
      return;
    }
    *puVar1 = *puVar1 & 0xfffffff7;
    return;
  }
}