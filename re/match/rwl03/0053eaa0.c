// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0053eaa0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef int (*FNP)(void);
MGLOBAL(int, DAT_007dc5dc, 0x007dc5dc);
MGLOBAL(int, DAT_007dc5ec, 0x007dc5ec);
MGLOBAL(int, DAT_007dc5f0, 0x007dc5f0);
int FUN_0053ebb0(void);
int FUN_0053ebf0(void);
int FUN_0053ec40(void);
int FUN_0053ec60(void);
int FUN_0053ed20(void);
int FUN_0053f1a0(void);
int FUN_0053f3c0(void);
int FUN_0053f920(void);
int FUN_0053f9b0(void);
int FUN_0053f9d0(void);
int FUN_0053f9f0(void);
int FUN_0053fa20(void);
int FUN_0053fa70(void);
int FUN_0053fa90(void);
int FUN_0053fab0(void);
int FUN_0053fad0(void);
int FUN_0053fb00(void);
int FUN_0053fb50(void);
int FUN_004c2d90(int p0, int p1, FNP p2, FNP p3);
int FUN_004e7da0(int p0, FNP p1, FNP p2, FNP p3);
int FUN_004e80c0(int p0, int p1, FNP p2, FNP p3, FNP p4);
int FUN_004e80f0(int p0, FNP p1, FNP p2, FNP p3);
int FUN_004f0910(int p0, int p1, FNP p2, FNP p3, FNP p4);
int FUN_004f0940(int p0, FNP p1, FNP p2, FNP p3);
int RpAtomicRegisterPlugin(int p0, int p1, FNP p2, FNP p3, FNP p4);
// MATCH: 0x0053eaa0 FUN_0053eaa0

int FUN_0053eaa0(void)

{
  int iVar1;
  
  iVar1 = FUN_004c2d90(0,0x120,FUN_0053ebf0,FUN_0053ebb0);
  if (iVar1 < 0) {
    return 0;
  }
  DAT_007dc5dc = FUN_004e80c0(4,0x120,FUN_0053ec40,FUN_0053ec60,FUN_0053ed20);
  if (DAT_007dc5dc < 0) {
    return 0;
  }
  iVar1 = FUN_004e80f0(0x120,FUN_0053f3c0,FUN_0053f1a0,FUN_0053f920);
  if (iVar1 < 0) {
    return 0;
  }
  DAT_007dc5ec = RpAtomicRegisterPlugin(4,0x120,FUN_0053f9b0,FUN_0053f9b0,FUN_0053f9d0);
  if (DAT_007dc5ec < 0) {
    return 0;
  }
  iVar1 = FUN_004e7da0(0x120,FUN_0053fa20,FUN_0053f9f0,FUN_0053fa70);
  if (iVar1 < 0) {
    return 0;
  }
  DAT_007dc5f0 = FUN_004f0910(4,0x120,FUN_0053fa90,FUN_0053fa90,FUN_0053fab0);
  if (DAT_007dc5f0 < 0) {
    return 0;
  }
  iVar1 = FUN_004f0940(0x120,FUN_0053fb00,FUN_0053fad0,FUN_0053fb50);
  return iVar1 >= 0;
}
