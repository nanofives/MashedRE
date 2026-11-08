// FLAGS: /O2 /GS /Ob1
/* re/match/top07/0040d270.c -- PORT-TOP-07: 0x0040d270 (hooks.csv Course::Finish), 441 B.
   match.py: 100.0% (regs 100.0%, ops 100.0%) 135/135 ins, whole symbol BYTE-IDENTICAL (relocations masked).
   Caller: 0x0040d440 only.
   Argument facts read from the disassembly (re/tools/match/odis.py 0x0040d270):
     0x00426340, 0x004264d0, 0x00411d90 each take one pushed argument, **(param_1 + param_2 * 4)
     (0x0040d305/0x0040d307, 0x0040d2cc/0x0040d2ce, 0x0040d3df/0x0040d3e1); Ghidra's decomp dropped two of them.
     0x00426070 takes none here (no push before 0x0040d3c8).
     0x44c30000 (= 1560.0f) is stored to DAT_00803324 at 0x0040d3d2.
     The elapsed value is converted unsigned (fild + fadd 0x005cc94c at 0x0040d406 when negative), format "%u (%.3f)"
     at 0x005ccd48, factor read from the float at 0x005ccd68.
   What set the block layout: the three-part test at 0x0040d289..0x0040d2a8 written as a static __inline helper
   (Cond_0040d270, expanded under /Ob1) and the branch written `if (!Cond) {A} else {B}`, so the A path
   (0x0040d2ae..0x0040d343) is laid out first and B (0x0040d345..0x0040d3c0) falls into the common tail 0x0040d3c3.
   The same test written inline in either if/else order, with gotos, a switch, or nested ifs, lays B first and
   merges A's duplicated 0x0040d110 + 0x004ccde0 pair (55.6%). Cond_0040d270 is a mechanical name, not a source name.
   Residual: none. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0063ba8c, 0x0063ba8c);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(float, DAT_00803324, 0x00803324);
MGLOBAL(int, DAT_0080332c, 0x0080332c);
MGLOBAL(float, DAT_005ccd68, 0x005ccd68);
int FUN_00426ba0(void);
uint FUN_004950b0(void);
void FUN_00462510(void);
void FUN_00462500(void);
int FUN_00426c00(void);
int FUN_0042f790(void);
void FUN_0042f7a0(void);
int FUN_0042fe30(void);
int FUN_0040d040(int param_1, int param_2);
void FUN_0040cf80(void);
void FUN_0040cfd0(void);
void FUN_0040d020(int param_1, int param_2);
void FUN_0040d110(int param_1, int param_2);
void FUN_00458c70(void);
void FUN_00426340(int p0);
int FUN_00426020(void);
void FUN_00407800(int p0);
void FUN_004074a0(void);
void FUN_004264d0(int p0);
void FUN_004ccde0(void);
int FUN_00426070(void);
void FUN_00411d90(int p0);
void FUN_004987b0(const char *fmt, ...);
static __inline int Cond_0040d270(int param_1, int param_2, int a)
{
  if (a != 0 && **(int **)(param_1 + param_2 * 4) == FUN_00426c00() && FUN_0042f790() == 0) {
    return 1;
  }
  return 0;
}
// MATCH: 0x0040d270 FUN_0040d270
void FUN_0040d270(int param_1, int param_2)
{
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar6;

  iVar2 = FUN_00426ba0();
  uVar3 = FUN_004950b0();
  FUN_00462510();
  if (!Cond_0040d270(param_1, param_2, iVar2)) {
    FUN_004ccde0();
    FUN_00462500();
    if (DAT_007f0fd0 != 5) {
      iVar4 = FUN_0042fe30();
      FUN_004264d0(**(int **)(param_1 + param_2 * 4));
      *(int *)(param_1 + 0x44) = iVar4;
    }
    FUN_0040cf80();
    if (iVar2 != 0) {
      FUN_0040cfd0();
    }
    FUN_004ccde0();
    FUN_0040d020(param_1, param_2);
    if (DAT_007f0fd0 == 5) {
      FUN_00426340(**(int **)(param_1 + param_2 * 4));
      FUN_00407800(FUN_00426020());
    }
    else {
      FUN_004074a0();
    }
    FUN_0040d110(param_1, param_2);
    FUN_004ccde0();
  }
  else {
    iVar4 = FUN_0042fe30();
    if (FUN_0040d040(param_1, param_2) != 0) {
      FUN_00462500();
      FUN_0040cf80();
      FUN_0040d110(param_1, param_2);
    }
    if (DAT_007f0fd0 == 5) {
      FUN_00462500();
      FUN_00458c70();
      FUN_00426340(**(int **)(param_1 + param_2 * 4));
      FUN_00407800(FUN_00426020());
    }
    else {
      FUN_004074a0();
      if (*(int *)(param_1 + 0x44) != iVar4 || **(int **)(param_1 + param_2 * 4) != FUN_00426c00()) {
        FUN_004264d0(**(int **)(param_1 + param_2 * 4));
        *(int *)(param_1 + 0x44) = iVar4;
      }
    }
  }
  FUN_0042f7a0();
  DAT_0080332c = FUN_00426070();
  DAT_00803324 = 1560.0f;
  FUN_00411d90(**(int **)(param_1 + param_2 * 4));
  DAT_0063ba8c = 1;
  uVar6 = FUN_004950b0() - uVar3;
  FUN_004987b0("course LoadTime is %u (%.3f)\n", uVar6, (double)((float)uVar6 * DAT_005ccd68));
}
