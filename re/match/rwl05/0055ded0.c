// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR04 2026-11-08: 98.4% (regs 100%), was 70.3%. Separate loop counter k fixes the ebx/edi swap. Remaining: one store keeps [edx+eax-0xc] where the original has [eax+edx-0xc] (8 spellings, pointer temp, permuter 711 compiles). Class (b). */
/* MATCH-RWL05 0x0055ded0 (h2): hand, 70.3%, class (b): only the register swap ebx<->edi (original ebx=count, edi=zero) and one store operand order remain; permuter 200 s no gain. */
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
typedef int (*AL)(unsigned int,int);
// MATCH: 0x0055ded0 FUN_0055ded0
int FUN_0055ded0(int param_1,unsigned short param_2,unsigned short param_3)
{
  unsigned int n;
  int off;
  int *p;

  n = param_2;
  *(unsigned short *)(param_1 + 0x20) = 0xffff;
  *(int *)(param_1 + 4) = 0;
  *(int *)(param_1 + 0x24) = 0;
  *(int *)(param_1 + 8) = 0;
  *(int *)(param_1 + 0x2c) = 0;
  *(unsigned short *)(param_1 + 0xe) = param_2;
  *(int *)(param_1 + 0x10) = (*(AL *)(DAT_007d3ff8 + 0x108))(n * 0xc,0x30900);
  *(unsigned short *)(param_1 + 0xc) = 0;
  *(int *)(param_1 + 0x1c) = 0;
  if (0 < param_2) {
    unsigned int k = n;
    off = 0;
    do {
      off += 0xc;
      k--;
      *(int *)(*(int *)(param_1 + 0x10) + off - 8) = 0;
      *(int *)(*(int *)(param_1 + 0x10) + off - 4) = 0;
      *(int *)(off + *(int *)(param_1 + 0x10) - 0xc) = 0;
    } while (k != 0);
  }
  *(unsigned short *)(param_1 + 0x14) = 0;
  *(unsigned short *)(param_1 + 0x16) = param_3;
  *(int *)(param_1 + 0x28) = 0;
  if (param_3 != 0) {
    *(int *)(param_1 + 0x18) = (*(AL *)(DAT_007d3ff8 + 0x108))((unsigned int)param_3 << 2,0x30900);
    return param_1;
  }
  *(int *)(param_1 + 0x18) = 0;
  return param_1;
}
