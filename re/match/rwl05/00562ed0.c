// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00562ed0: hand-matched (h1). 28.7% (regs 77.0%, ops 89.7%), 82/87 ins. Record fill from a source struct (3 V3 copies + fields). Original keeps param_5/param_2/param_3 in ebx/ebp/edi (4 callee-saved regs) and copies per-field through scratch regs; struct-copy and raw Ghidra forms both allocate differently. Class (b). */
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
typedef struct V3 { unsigned int x, y, z; } V3;
typedef struct Rec {
  V3 a; unsigned int pad0;
  V3 b; unsigned int pad1;
  V3 c; unsigned int pad2;
  V3 d; unsigned int pad3;
  int f40; unsigned int f44; unsigned int f48;
} Rec;
int FUN_00561ea0(int p0);
// MATCH: 0x00562ed0 FUN_00562ed0
unsigned int * FUN_00562ed0(int param_1,int param_2,unsigned int *param_3,int param_4,int *param_5)
{
  Rec *puVar1;
  unsigned int uVar2;
  int iVar5;

  uVar2 = param_5[1];
  if (uVar2 >= (unsigned int)param_5[2]) return 0;
  puVar1 = (Rec *)(*param_5 + uVar2 * 0x4c);
  param_5[1] = uVar2 + 1;
  if (param_2 != 0) {
    iVar5 = FUN_00561ea0(param_2);
    param_5[4] = iVar5;
  }
  puVar1->a = *(V3 *)param_3[7];
  puVar1->b = *(V3 *)param_3[8];
  puVar1->c = *(V3 *)param_3[9];
  puVar1->d.x = param_3[0];
  puVar1->d.y = param_3[1];
  puVar1->f40 = param_2;
  puVar1->d.z = param_3[2];
  uVar2 = param_3[6];
  puVar1->f44 = uVar2;
  if ((int *)param_5[4] != 0) {
    puVar1->f48 = *(unsigned int *)(*(int *)param_5[4] + (uVar2 >> 3) * 4) >> ((uVar2 & 7) << 2) & 0xf;
    return param_3;
  }
  puVar1->f48 = 0xf;
  return param_3;
}
