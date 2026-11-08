// FLAGS: /O1
// best 9.2 (raw converter text + C++ prototypes for fscale/ABS). CRT floating-point error-handler tail that runs in ANOTHER function frame (reads/writes [ebp-0x2d0..0x8a] of the caller, no prologue, jumps into the caller shared epilogue). Not expressible as a standalone C function. Class (c).
/* MATCH-S28 0x005c340a: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs28_20261107/results.tsv. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef long double float10;
MGLOBAL(int, DAT_00773990, 0x00773990);
MGLOBAL(undefined1, DAT_005e72b0, 0x005e72b0);
MGLOBAL(undefined1, DAT_005e72a0, 0x005e72a0);
MGLOBAL(undefined1, DAT_005e72c0, 0x005e72c0);
MGLOBAL(undefined1, DAT_005e72b8, 0x005e72b8);
MGLOBAL(undefined1, DAT_005e72a8, 0x005e72a8);
MGLOBAL(undefined1, DAT_005e72c8, 0x005e72c8);
void FUN_004ae17b(int p0, int * p1, ushort * p2);
float10 fscale(float10 a, float10 b);
float10 ABS(float10 a);
MGLOBAL(int, _DAT_005e72b8, 0x005e72b8);
MGLOBAL(int, _DAT_005e72a8, 0x005e72a8);
MGLOBAL(int, _DAT_005e72c8, 0x005e72c8);
MGLOBAL(int, _DAT_005e72b0, 0x005e72b0);
MGLOBAL(int, _DAT_005e72a0, 0x005e72a0);
MGLOBAL(int, _DAT_005e72c0, 0x005e72c0);
// MATCH: 0x005c340a FUN_005c340a
/* [C1 2026-05-18] Same body as 0x005c3403 minus the leading `byte&0xfe` clear at EBP-0x2c8. */

void FUN_005c340a(void)

{
  char cVar1;
  ushort uVar2;
  int unaff_EBP;
  ushort in_FPUStatusWord;
  float10 in_ST0;
  
  if (DAT_00773990 != 0) {
    return;
  }
  *(double *)(unaff_EBP + -0x2d0) = (double)in_ST0;
  cVar1 = *(char *)(unaff_EBP + -0x90);
  if (cVar1 != '\0') {
    if (cVar1 == -1) {
      uVar2 = *(ushort *)(unaff_EBP + -0x2ca) & 0x7ff0;
    }
    else {
      if (cVar1 != -2) {
        if (cVar1 == '\0') {
          return;
        }
        *(int *)(unaff_EBP + -0x8e) = (int)cVar1;
        goto LAB_005c34ef;
      }
      uVar2 = *(ushort *)(unaff_EBP + -0x2ca) & 0x7ff0;
      if (uVar2 == 0) {
        *(undefined4 *)(unaff_EBP + -0x8e) = 4;
        in_ST0 = (float10)fscale(in_ST0,(float10)_DAT_005e72b8);
        if (ABS(in_ST0) < (float10)_DAT_005e72a8) {
          in_ST0 = in_ST0 * (float10)_DAT_005e72c8;
        }
        goto LAB_005c34ef;
      }
    }
    if (uVar2 == 0x7ff0) {
      *(undefined4 *)(unaff_EBP + -0x8e) = 3;
      in_ST0 = (float10)fscale(in_ST0,(float10)_DAT_005e72b0);
      if ((float10)_DAT_005e72a0 < ABS(in_ST0)) {
        in_ST0 = in_ST0 * (float10)_DAT_005e72c0;
      }
      goto LAB_005c34ef;
    }
  }
  if ((*(ushort *)(unaff_EBP + -0xa4) & 0x20) != 0) {
    return;
  }
  if ((in_FPUStatusWord & 0x20) == 0) {
    return;
  }
  *(undefined4 *)(unaff_EBP + -0x8e) = 8;
LAB_005c34ef:
  *(int *)(unaff_EBP + -0x8a) = *(int *)(unaff_EBP + -0x94) + 1;
  if ((*(byte *)(unaff_EBP + -0x2c8) & 1) == 0) {
    *(undefined4 *)(unaff_EBP + -0x86) = *(undefined4 *)(unaff_EBP + 8);
    *(undefined4 *)(unaff_EBP + -0x82) = *(undefined4 *)(unaff_EBP + 0xc);
    if (*(char *)(*(int *)(unaff_EBP + -0x94) + 0xd) != '\x01') {
      *(undefined4 *)(unaff_EBP + -0x7e) = *(undefined4 *)(unaff_EBP + 0x10);
      *(undefined4 *)(unaff_EBP + -0x7a) = *(undefined4 *)(unaff_EBP + 0x14);
    }
  }
  *(double *)(unaff_EBP + -0x76) = (double)in_ST0;
  FUN_004ae17b((int)*(char *)(*(int *)(unaff_EBP + -0x94) + 0xe),(int *)(unaff_EBP + -0x8e),(ushort *)(unaff_EBP + -0xa4)
              );
  return;
}
