// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004c30b0: best 61.7% (regs ~79%), class (b). RwGlobals as a 300-byte struct (G300, rep movsd copies), bool temporaries for the three guards (sete/setne in the original), FUN_004d7ff0 declared variadic (called with 1 or 2 args), alloc result stored to the global first. The original keeps 1 in edi and the state bool in esi; ours swaps them. Permuter variants moved a call before its input (semantically wrong), discarded. */
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
typedef int code();
typedef struct G300 { int a[0x4b]; } G300;
MGLOBAL(G300 *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(G300, DAT_007d3ec8, 0x007d3ec8);
MGLOBAL(undefined4, DAT_00617fe0, 0x00617fe0);
MGLOBAL(int, DAT_007d3ff4, 0x007d3ff4);
typedef undefined4 * (__cdecl *PFN_ALLOC)(undefined4, uint);
typedef void (__cdecl *PFN_FREE)(undefined4 *);
MGLOBAL(PFN_FREE, DAT_007d3fd4, 0x007d3fd4);
int FUN_004c2c90(int p0, undefined4 p1, int * p2, undefined4 p3, int p4);
int FUN_004cae90(void);
undefined4 FUN_004d7ff0(undefined4 p0, ...);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004c30b0 FUN_004c30b0
int FUN_004c30b0(int param_1)
{
  int iVar1;
  G300 *puVar2;
  int bState;
  int bParam;
  int bStream;
  undefined4 loc[2];
  if (DAT_007d3ff8 == (G300 *)0) {
    DAT_007d3ff8 = &DAT_007d3ec8;
  }
  bState = (DAT_007d3ff8->a[0x49] == 1);
  if (bState) {
    bParam = (param_1 != 0);
    if (bParam) {
      iVar1 = FUN_004cae90();
      bStream = (iVar1 != 0);
      if (bStream) {
        DAT_007d3ff8 = (G300 *)(*(PFN_ALLOC)DAT_007d3ff8->a[0x42])(DAT_00617fe0,0x40000);
        puVar2 = DAT_007d3ff8;
        if (DAT_007d3ff8 == (G300 *)0) {
          DAT_007d3ff8 = &DAT_007d3ec8;
          loc[0] = 1;
          loc[1] = FUN_004d7ff0(0x80000013,DAT_00617fe0);
          FUN_004d8480(loc);
          return 0;
        }
        *puVar2 = DAT_007d3ec8;
        FUN_004c2c90(iVar1,4,DAT_007d3ff8->a + 4,(undefined4)(DAT_007d3ff8->a + 0x42),0);
        if (FUN_004c2c90(iVar1,0,0,param_1,0) != 0) {
          FUN_004c2c90(iVar1,0xb,DAT_007d3ff8->a + 0x12,0,0x1d);
          DAT_007d3ff4 = DAT_007d3ff4 + 1;
          DAT_007d3ff8->a[0x49] = 2;
          return 1;
        }
        DAT_007d3ff8 = &DAT_007d3ec8;
        DAT_007d3ec8 = *puVar2;
        (*DAT_007d3fd4)((undefined4 *)puVar2);
        return 0;
      }
      return bStream;
    }
    loc[0] = 1;
    loc[1] = FUN_004d7ff0(0x80000016);
    FUN_004d8480(loc);
    return bParam;
  }
  loc[0] = 1;
  loc[1] = FUN_004d7ff0(0x80000001);
  FUN_004d8480(loc);
  return bState;
}
