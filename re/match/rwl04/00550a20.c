// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00550a20 h1: best 82.4% (regs 82.4%, ops 94.1%). Residue: edi pushed at entry in orig (no shrink-wrap), ops (vt+0x28) computed before the x18 test in orig. memcpy intrinsic = rep movsd/movsb. Class (a). */
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
typedef int (*rdfn)(int, char *, int);
typedef int (*skfn)(int, int, int);
typedef struct { int pad0; int pad1; rdfn rd; int pad3; skfn sk; } Ops;
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
// MATCH: 0x00550a20 FUN_00550a20
char * FUN_00550a20(char *param_1,int param_2,int param_3)
{
  int iVar1;
  int iVar4;
  Ops *ops;
  
  iVar4 = 0;
  ops = (Ops *)(*(int *)(param_3 + 0x38) + 0x28);
  iVar1 = ops->rd(param_3,param_1,param_2 + -1);
  if (iVar1 == 0) {
    return 0;
  }
  if (0 < iVar1) {
    param_2 = iVar1 + -1;
    do {
      if (param_1[iVar4] == '\n') {
        iVar4 = iVar4 + 1;
        param_1[iVar4] = 0;
        iVar4 = iVar4 - iVar1;
        ops = (Ops *)(*(int *)(param_3 + 0x38) + 0x28);
        if (*(int *)(param_3 + 0x18) != 0) {
          *(int *)(param_3 + 0x28) = 4;
          *(int *)(param_3 + 0x20) = 3;
        }
        ops->sk(param_3,iVar4,2);
        return param_1;
      }
      if (((param_1[iVar4] == '\r') && (iVar4 < param_2)) &&
         (param_1[iVar4 + 1] == '\n')) {
        memcpy(param_1 + iVar4, param_1 + iVar4 + 1, (iVar1 - iVar4) - 1);
        iVar1 = iVar1 + -1;
        param_2 = param_2 + -1;
      }
      else {
        iVar4 = iVar4 + 1;
      }
    } while (iVar4 < iVar1);
  }
  param_1[iVar1] = 0;
  return param_1;
}
