// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045cfe0 (h2): 100% (regs 100%). Raw 70.6%. The increment is a float literal (0.0166f, the value
   stored at 0x005ce650): with the constant read through an MGLOBAL the compiler emits fld const / fadd [field]
   (90.2%, any operand order); a literal lets it fold to fld [field] / fadd const (relocheck OK, the
   __real constant is not counted). 0x0045cbe0 is declared with 3 args: the original pushes 3, the
   callee reads 2. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
typedef struct { int f0; int f4; int f8; float fc; float f10; int pad[5]; } Rec28;
MGLOBAL(int *, DAT_0088fbc4, 0x0088fbc4);
MGLOBAL(Rec28 *, DAT_0088fbc8, 0x0088fbc8);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
void FUN_0045c640(int param_1);
void FUN_0045cbe0(int param_1, int param_2, int param_3);
// MATCH: 0x0045cfe0 FUN_0045cfe0
void FUN_0045cfe0(void)
{
  int p;
  int i;

  for (p = 0; p < 4; p++) {
    FUN_0045c640(p);
    for (i = 0; i < *DAT_0088fbc4; i++) {
      if (DAT_0088fbc8[i].fc > DAT_005d757c) {
        DAT_0088fbc8[i].f10 = DAT_0088fbc8[i].f10 + 0.0166f; /* 0.0166f = the float at 0x005ce650 */
        if (DAT_0088fbc8[i].f10 > DAT_0088fbc8[i].fc)
          FUN_0045cbe0(DAT_0088fbc8[i].f0, DAT_0088fbc8[i].f4, DAT_0088fbc8[i].f8);
      }
    }
  }
}
