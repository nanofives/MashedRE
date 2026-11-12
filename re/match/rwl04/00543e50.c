// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00543e50 h1: best 75.9% (regs 75.9%, ops 76.4%). Residue: orig keeps 14 unmerged 'xor eax,eax; ret' failure exits after each jge, ours cross-jumps them to one tail (148 vs 174 ins). Flags /Ot /Oy- /GF /Zi /Ow /Od do not change it. Callbacks as real functions, callees with real arity. Class (a). */
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
MGLOBAL(int, DAT_007dc634, 0x007dc634);
MGLOBAL(int, DAT_007dc638, 0x007dc638);
MGLOBAL(int, DAT_007dc63c, 0x007dc63c);
MGLOBAL(int, DAT_007dc640, 0x007dc640);
MGLOBAL(int, DAT_007dc644, 0x007dc644);
MGLOBAL(int, DAT_007dc648, 0x007dc648);
MGLOBAL(int, DAT_007dc64c, 0x007dc64c);
MGLOBAL(int, DAT_007dc650, 0x007dc650);
MGLOBAL(int, DAT_007dc654, 0x007dc654);
MGLOBAL(int, DAT_007dc658, 0x007dc658);
MGLOBAL(int, DAT_007dc65c, 0x007dc65c);
MGLOBAL(int, DAT_007dc660, 0x007dc660);
MGLOBAL(int, DAT_007dc664, 0x007dc664);
MGLOBAL(int, DAT_007dc668, 0x007dc668);
typedef int (*cbA)(void);
int FUN_005440c0(void);
int FUN_005440d0(void);
int FUN_005440e0(void);
int FUN_00544100(void);
int FUN_005441f0(void);
int FUN_005444a0(void);
int FUN_005446f0(void);
int FUN_005448f0(void);
int FUN_004c1710(int a, int b, int (*c)(void), int (*d)(void), int (*e)(void));
int FUN_004c1cc0(int a, int b, int (*c)(void), int (*d)(void), int (*e)(void));
int FUN_004c2d90(int a, int b, int (*c)(void), int (*d)(void));
int FUN_004c5da0(int a, int b, int (*c)(void), int (*d)(void), int (*e)(void));
int FUN_004cf430(int a, int (*b)(void), int (*c)(void), int (*d)(void));
int FUN_004e4bd0(int a, int b, int (*c)(void), int (*d)(void), int (*e)(void));
int FUN_004e4c00(int a, int (*b)(void), int (*c)(void), int (*d)(void));
int FUN_004e80c0(int a, int b, int (*c)(void), int (*d)(void), int (*e)(void));
int FUN_004e80f0(int a, int (*b)(void), int (*c)(void), int (*d)(void));
int FUN_004e8f50(int a, int b, int (*c)(void), int (*d)(void), int (*e)(void));
int FUN_004e8f80(int a, int (*b)(void), int (*c)(void), int (*d)(void));
int FUN_004f0910(int a, int b, int (*c)(void), int (*d)(void), int (*e)(void));
int FUN_004f0940(int a, int (*b)(void), int (*c)(void), int (*d)(void));
int FUN_005c4640(int a, int (*b)(void), int (*c)(void), int (*d)(void));
int FUN_005c4780(int a, int (*b)(void), int (*c)(void), int (*d)(void));
// MATCH: 0x00543e50 FUN_00543e50
int FUN_00543e50(void)

{
  int iVar1;
  
  iVar1 = FUN_004c2d90(0,0x11f,FUN_005440c0,FUN_005440d0);
  if (iVar1 < 0) {
    return 0;
  }
  DAT_007dc634 = FUN_004e8f50(8,0x11f,FUN_005440e0,FUN_00544100,FUN_005441f0);
  if (DAT_007dc634 < 0) {
    return 0;
  }
  DAT_007dc638 = FUN_004e8f80(0x11f,FUN_005444a0,FUN_005446f0,FUN_005448f0);
  if (DAT_007dc638 < 0) {
    return 0;
  }
  DAT_007dc63c = FUN_004f0910(8,0x11f,FUN_005440e0,FUN_00544100,FUN_005441f0);
  if (DAT_007dc634 < 0) {
    return 0;
  }
  DAT_007dc640 = FUN_004f0940(0x11f,FUN_005444a0,FUN_005446f0,FUN_005448f0);
  if (DAT_007dc638 < 0) {
    return 0;
  }
  DAT_007dc644 = FUN_004c1710(8,0x11f,FUN_005440e0,FUN_00544100,FUN_005441f0);
  if (DAT_007dc644 < 0) {
    return 0;
  }
  DAT_007dc648 = FUN_005c4780(0x11f,FUN_005444a0,FUN_005446f0,FUN_005448f0);
  if (DAT_007dc648 < 0) {
    return 0;
  }
  DAT_007dc64c = FUN_004c1cc0(8,0x11f,FUN_005440e0,FUN_00544100,FUN_005441f0);
  if (DAT_007dc64c < 0) {
    return 0;
  }
  DAT_007dc650 = FUN_005c4640(0x11f,FUN_005444a0,FUN_005446f0,FUN_005448f0);
  if (DAT_007dc650 < 0) {
    return 0;
  }
  DAT_007dc654 = FUN_004e4bd0(8,0x11f,FUN_005440e0,FUN_00544100,FUN_005441f0);
  if (DAT_007dc654 < 0) {
    return 0;
  }
  DAT_007dc658 = FUN_004e4c00(0x11f,FUN_005444a0,FUN_005446f0,FUN_005448f0);
  if (DAT_007dc658 < 0) {
    return 0;
  }
  DAT_007dc65c = FUN_004e80c0(8,0x11f,FUN_005440e0,FUN_00544100,FUN_005441f0);
  if (DAT_007dc65c < 0) {
    return 0;
  }
  DAT_007dc660 = FUN_004e80f0(0x11f,FUN_005444a0,FUN_005446f0,FUN_005448f0);
  if (DAT_007dc660 < 0) {
    return 0;
  }
  DAT_007dc664 = FUN_004c5da0(8,0x11f,FUN_005440e0,FUN_00544100,FUN_005441f0);
  if (DAT_007dc664 < 0) {
    return 0;
  }
  DAT_007dc668 = FUN_004cf430(0x11f,FUN_005444a0,FUN_005446f0,FUN_005448f0);
  return 0 <= DAT_007dc668;
}
