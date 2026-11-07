// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b36f0: 100.0% (regs 100.0%). Hand: table PTR_PTR_00634550 as int*[0x66] (2 ptrs per entry), param_2!=0 branch first; globals typed array (MGLOBALA). */
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
typedef unsigned int uint;
MGLOBALA(int *, PTR_PTR_00634550, 0x66, 0x00634550);
int FUN_005ac9e0(int p0, int p1);
// MATCH: 0x005b36f0 FUN_005b36f0
int FUN_005b36f0(int param_1, int param_2)
{
  int iVar1;
  uint i;

  i = 0;
  do {
    if (param_2 != 0) {
      iVar1 = FUN_005ac9e0(*PTR_PTR_00634550[i * 2], param_1);
      if (iVar1 != 0) {
        return *PTR_PTR_00634550[i * 2];
      }
    }
    else {
      iVar1 = FUN_005ac9e0(*(int *)(PTR_PTR_00634550[i * 2] + 1), param_1);
      if (iVar1 != 0) {
        return *(int *)(PTR_PTR_00634550[i * 2] + 1);
      }
    }
    i = i + 1;
  } while (i < 0x33);
  return 0;
}
