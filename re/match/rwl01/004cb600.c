// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 78.6% (regs 81.0%), class b. Struct form right (Rec6c 0x6c array, Blk68 copy gives rep movsd, realloc/malloc arms, COM call 0xcc __stdcall, setge result). Residual: original lays out grow code, then the FUN_004cb740 check block, then the shared copy tail with the early 'return 1' epilogue shared at the very end; ours emits the check block and its own epilogue after the tail. Tried: if/else flip, malloc/realloc arm order, permuter (84.5% variant makes the counter unsigned which changes behaviour: rejected). */
/* MATCH-RWL01 0x004cb600: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
#ifndef __cplusplus
#define true 1
#endif
#ifdef __cplusplus
typedef int code(...);
#else
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#endif
typedef struct Blk68 { int d[26]; } Blk68;
typedef struct Rec6c { Blk68 b; int e; } Rec6c;
MGLOBAL(int, DAT_007d456c, 0x007d456c);
MGLOBAL(Rec6c *, DAT_007d4570, 0x007d4570);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
undefined4 FUN_004cb740(int * p0, Rec6c * p1);
// MATCH: 0x004cb600 FUN_004cb600
bool FUN_004cb600(int param_1,int *param_2)

{
  int iVar2;
  
  iVar2 = DAT_007d456c;
  if (param_1 >= DAT_007d456c) {
    DAT_007d456c = param_1 + 1;
    if (DAT_007d4570 != 0) {
      DAT_007d4570 = (Rec6c *)(**(code **)(DAT_007d3ff8 + 0x110))(DAT_007d4570,DAT_007d456c * 0x6c,0x1030411);
    }
    else {
      DAT_007d4570 = (Rec6c *)(**(code **)(DAT_007d3ff8 + 0x108))(DAT_007d456c * 0x6c,0x1030411);
    }
    while (iVar2 < DAT_007d456c) {
      DAT_007d4570[iVar2].e = 0;
      iVar2++;
    }
  }
  else {
    if (FUN_004cb740(param_2,&DAT_007d4570[param_1]) != 0) {
      return true;
    }
  }
  DAT_007d4570[param_1].b = *(Blk68 *)param_2;
  iVar2 = (*(int (__stdcall *)(int *,int,int *))*(int *)(*DAT_007d4110 + 0xcc))(DAT_007d4110,param_1,param_2);
  return iVar2 >= 0;
}
