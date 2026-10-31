// FLAGS: /O2 /GS /Ob1
/* 0x00411ce0 (127 B): 100% exact, regs 100%. Raw Ghidra 97.6%: the only diff was cmp eax,2 vs sub eax,2, fixed by
   a one-case switch on FUN_0042f6a0(). RwGlobals slot +0x20 called through a real cdecl fn-pointer type. No residual. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_0063bb10, 0x0063bb10);
MGLOBAL(int, DAT_0063bb0c, 0x0063bb0c);
MGLOBAL(int, DAT_0063bb28, 0x0063bb28);
typedef int (__cdecl *RwSetStateFn)(int, int);
void FUN_0041a960(void);
int FUN_0042f6a0(void);
void FUN_00483a70(int p0);
// MATCH: 0x00411ce0 FUN_00411ce0
void FUN_00411ce0(void)
{
  switch (FUN_0042f6a0()) {
  case 2:
    (*(RwSetStateFn *)(DAT_007d3ff8 + 0x20))(10,5);
    (*(RwSetStateFn *)(DAT_007d3ff8 + 0x20))(0xb,6);
    (*(RwSetStateFn *)(DAT_007d3ff8 + 0x20))(8,1);
    (*(RwSetStateFn *)(DAT_007d3ff8 + 0x20))(6,1);
    (*(RwSetStateFn *)(DAT_007d3ff8 + 0x20))(0x14,2);
    if ((DAT_0063bb10 != 0) || (DAT_0063bb0c != 0)) {
      FUN_0041a960();
    }
    if ((DAT_0063bb28 != 0) && (DAT_0063bb10 != 0)) {
      FUN_00483a70(DAT_0063bb10);
    }
    break;
  }
}
