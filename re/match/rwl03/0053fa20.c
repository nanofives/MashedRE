// FLAGS: /O2 /Ob1 /G5
// h3: 65.6% regs 65.6% (flags /O1 /G5 won). Residue class (b): original loads edi first then lea; prologue/schedule of the arg reload not moved by 4 source forms and the permuter.
/* MATCH-RWL03 0x0053fa20: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
int FUN_004cc790(int p0, int *p1, int p2);
int FUN_00543b10(int p0);
MGLOBAL(int, DAT_007dc5ec, 0x007dc5ec);
// MATCH: 0x0053fa20 FUN_0053fa20
int FUN_0053fa20(int param_1,int param_2,int param_3)
{
  int *puVar1;
  int iVar2;
  int flag;

  iVar2 = param_1;
  if (FUN_004cc790(iVar2, &flag, 4) == 0) {
    return 0;
  }
  if (flag != 0) {
    puVar1 = (int *)(DAT_007dc5ec + param_3);
    if (*(int *)(DAT_007dc5ec + param_3) == 0) {
      if (FUN_00543b10(param_3) != 0) {
        *puVar1 = 1;
        return iVar2;
      }
    }
  }
  return iVar2;
}
