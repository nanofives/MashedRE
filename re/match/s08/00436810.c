// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00436810: 100% (regs 100%) by hand, first pass. The two counting loops are a 12-entry loop over the
   0x30-byte records at 0x007f0a40 (fields +4 and +0xc), which this compiler unrolls 6x; Ghidra showed the unrolled
   form. relocheck OK. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct { int f0; int f4; int f8; int fc; int pad[8]; } Match007f0a40;
MGLOBALA(Match007f0a40, DAT_007f0a40, 12, 0x007f0a40);
int FUN_00430760(void);
// MATCH: 0x00436810 FUN_00436810
int FUN_00436810(int param_1)
{
  int n;
  int i;

  if (param_1 == 0xc) {
    return 0;
  }
  if (FUN_00430760() != 0) {
    n = 0;
    for (i = 0; i < 12; i++) {
      if (DAT_007f0a40[i].f4 == 2) {
        n++;
      }
    }
    if (DAT_007f0a40[param_1].f4 == 0) {
      return 0;
    }
    return n >= param_1;
  }
  n = 0;
  for (i = 0; i < 12; i++) {
    if (DAT_007f0a40[i].fc != 0) {
      n++;
    }
  }
  return n - 1 >= param_1;
}
