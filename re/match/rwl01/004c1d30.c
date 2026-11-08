// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 100% (regs 100%), class -. FLAGS was /O1, now /O2 /Ob1 /G5. Callee 0x004c1a70 needs a same-TU definition so the compiler knows it keeps EDX (zero reg) and takes a stack arg: file holds a NON-real stand-in body for 0x004c1a70 under #ifndef __cplusplus (its real body is not in this file; it clobbers eax/ecx only). Function-pointer stores use real prototypes. */
/* MATCH-RWL01 0x004c1d30: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d3ec0, 0x007d3ec0);
MGLOBAL(undefined4, DAT_00617f98, 0x00617f98);
void FUN_004c1de0(void);
void FUN_004c2b80(void);
void FUN_004c2bb0(void);
int FUN_004d8000(int p0, undefined4 p1);
/* stand-in for 0x004c1a70 (not the real body): defined in this TU so the compiler knows it leaves EDX alone */
#ifdef __cplusplus
void FUN_004c1a70(int p0);
#else
void FUN_004c1a70(int p0) { int n = *(int *)(p0 + 0x14) - 2; if (n != 0) { *(int *)(p0 + 0x8c) = n; } else { *(int *)(p0 + 0x90) = *(int *)(p0 + 0x84); } }
#endif
// MATCH: 0x004c1d30 FUN_004c1d30
undefined1 * FUN_004c1d30(void)
{
  undefined1 *puVar1;

  puVar1 = (undefined1 *)(**(int (**)(int, int))(DAT_007d3ff8 + 0x118))(*(int *)(DAT_007d3ec0 + DAT_007d3ff8), 0x30005);
  if (puVar1 == (undefined1 *)0x0) {
    return (undefined1 *)0x0;
  }
  *puVar1 = 4;
  puVar1[1] = 0;
  puVar1[2] = 0;
  puVar1[3] = 0;
  *(undefined4 *)(puVar1 + 4) = 0;
  *(void **)(puVar1 + 0x10) = (void *)FUN_004c1de0;
  *(void **)(puVar1 + 0x18) = (void *)FUN_004c2bb0;
  *(void **)(puVar1 + 0x1c) = (void *)FUN_004c2b80;
  *(undefined4 *)(puVar1 + 0x6c) = 0x3f800000;
  *(undefined4 *)(puVar1 + 0x68) = 0x3f800000;
  *(undefined4 *)(puVar1 + 0x74) = 0x3f800000;
  *(undefined4 *)(puVar1 + 0x70) = 0x3f800000;
  *(undefined4 *)(puVar1 + 0x7c) = 0;
  *(undefined4 *)(puVar1 + 0x78) = 0;
  *(undefined4 *)(puVar1 + 0x80) = 0x3d4ccccd;
  *(undefined4 *)(puVar1 + 0x84) = 0x41200000;
  *(undefined4 *)(puVar1 + 0x88) = 0x40a00000;
  *(undefined4 *)(puVar1 + 0x60) = 0;
  *(undefined4 *)(puVar1 + 100) = 0;
  *(undefined4 *)(puVar1 + 0x14) = 1;
  FUN_004c1a70((int)puVar1);
  *(undefined4 *)(puVar1 + 0x2c) = 0;
  FUN_004d8000((int)&DAT_00617f98, (undefined4)puVar1);
  return puVar1;
}
