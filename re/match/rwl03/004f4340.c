// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x004f4340: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911ae0, 0x00911ae0);
undefined4 * FUN_004d4170(void);
void FUN_004d41e0(int p0);
int * FUN_004d4380(int * p0);
int * FUN_004d4dd0(int * p0);
int * FUN_004d4f90(uint * p0, int * p1, int p2);
undefined * * FUN_004eb9d0(int p0);
void FUN_004f07a0(int p0);
// MATCH: 0x004f4340 FUN_004f4340
undefined4 FUN_004f4340(void)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  iVar1 = (int)FUN_004d4170();
  if (iVar1 != 0) {
    *(undefined4 *)(iVar1 + 0x2c) = 2;
    iVar2 = (int)FUN_004d4dd0((int *)iVar1);
    if (iVar2 != 0) {
      uVar3 = (undefined4)FUN_004eb9d0(0);
      uVar3 = (undefined4)FUN_004d4f90((uint *)iVar2,0,(int)uVar3);
      iVar2 = (int)FUN_004d4380((int *)uVar3);
      if (iVar2 != 0) {
        *(int *)(DAT_007d3ff8 + DAT_00911ae0 + 0x58) = iVar1;
        FUN_004f07a0(iVar1);
        return 1;
      }
    }
    FUN_004d41e0(iVar1);
  }
  return 0;
}
