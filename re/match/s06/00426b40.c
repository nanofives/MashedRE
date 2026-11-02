// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00426b40: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs06_20261101/results.tsv.
   C++ check fixed (h12, 2026-11-01): FUN_0041e900 has 1 arg (tail-jump via ecx), FUN_00478cf0 args void*. Prototypes only, codegen unchanged (100%, relocheck OK). */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
MGLOBAL(undefined4, DAT_0066d704, 0x0066d704);
MGLOBAL(undefined4, DAT_00644158, 0x00644158);
MGLOBAL(undefined1, DAT_00646e58, 0x00646e58);
void FUN_004074a0(void);
void FUN_0041e900(void *p0);
bool FUN_0041e9b0(int p0);
undefined4 FUN_0041ea20(void);
void FUN_00425ea0(void);
void FUN_0045e040(void);
void FUN_00478cf0(void *p0, void *p1);
void FUN_0047f450(void);
void FUN_0047f480(void);
void FUN_00491530(void);
// MATCH: 0x00426b40 FUN_00426b40
void FUN_00426b40(undefined4 param_1)

{
  int iVar1;
  
  FUN_0041e9b0(param_1);
  FUN_0047f450();
  iVar1 = FUN_0041ea20();
  if (iVar1 != 0) {
    FUN_0041e900(&DAT_00646e58);
  }
  FUN_00491530();
  FUN_00478cf0(&DAT_00646e58,&DAT_00644158);
  FUN_00425ea0();
  FUN_004074a0();
  FUN_0045e040();
  FUN_0047f480();
  DAT_0066d704 = 0;
  return;
}
