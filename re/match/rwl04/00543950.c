// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00543950: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
MGLOBAL(int, DAT_007dc5dc, 0x007dc5dc);
MGLOBAL(int, DAT_007dc628, 0x007dc628);
typedef void (__cdecl *PFN_c)(int,int,int);
MGLOBAL(PFN_c, DAT_007dc62c, 0x007dc62c);
void FUN_004f8640(int p0, uint p1);
uint FUN_004f8660(int p0);
// MATCH: 0x00543950 FUN_00543950

void FUN_00543950(int param_1,int param_2,int param_3)
{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  int *piVar5;
  if (param_3 == 0) {
    iVar4 = *(int *)(param_2 + 4);
    piVar5 = (int *)(param_2 + 0x48);
    do {
      if (*(int *)(DAT_007dc5dc + *piVar5) != 0) {
        iVar1 = *(int *)(*(int *)(DAT_007dc5dc + *piVar5) + 0x30);
        switch (iVar1) {
        case 1:
        case 3:
          if (DAT_007dc628 != 0) {
            uVar2 = *(undefined4 *)(param_1 + 0x18);
            uVar3 = FUN_004f8660(uVar2);
            FUN_004f8640(uVar2,uVar3 | 0x10000);
          }
        }
      }
      piVar5 = piVar5 + 9;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
  }
  (*DAT_007dc62c)(param_1,param_2,param_3);
}
