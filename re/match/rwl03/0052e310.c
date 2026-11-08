// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0052e310: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(int, DAT_007dc500, 0x007dc500);
MGLOBAL(undefined4, DAT_007dc504, 0x007dc504);
MGLOBAL(undefined4, DAT_007dc508, 0x007dc508);
void FUN_0052e3d0(void);
void FUN_0052e490(void);
void FUN_0052e4e0(void);
void FUN_0052e650(void);
void FUN_0052e500(void);
void FUN_0052e560(void);
void FUN_0052e580(void);
void FUN_0052e5a0(void);
void FUN_0052e5c0(void);
void FUN_0052e6c0(void);
void FUN_005c9d00(void);
void FUN_0052e710(void);
void FUN_0052e800(void);
void FUN_0052ec50(void);
void FUN_004d7ff0(void);
int FUN_004c2d90(int p0, int p1, void *p2, void *p3);
void FUN_004e7da0(int p0, void *p1, void *p2, void *p3);
void FUN_004e7dd0(int p0, void *p1);
int FUN_004e7df0(int p0, void *p1);
int FUN_004e8f50(int p0, int p1, void *p2, void *p3, void *p4);
void FUN_004e8f80(int p0, void *p1, void *p2, void *p3);
int RpAtomicRegisterPlugin(int p0, int p1, void *p2, void *p3, void *p4);
#define V(f) ((void *)(f))
// MATCH: 0x0052e310 FUN_0052e310
int FUN_0052e310(void)
{
  DAT_007dc500 = FUN_004c2d90(0,0x116,V(FUN_0052e3d0),V(FUN_0052e490));
  DAT_007dc504 = RpAtomicRegisterPlugin(4,0x116,V(FUN_0052e560),V(FUN_0052e580),V(FUN_0052e5a0));
  FUN_004e7da0(0x116,V(FUN_0052ec50),V(FUN_004d7ff0),V(FUN_005c9d00));
  FUN_004e7dd0(0x116,V(FUN_0052e5c0));
  FUN_004e7df0(0x116,V(FUN_0052e650));
  DAT_007dc508 = FUN_004e8f50(4,0x116,V(FUN_0052e4e0),V(FUN_0052e500),V(FUN_004d7ff0));
  FUN_004e8f80(0x116,V(FUN_0052e800),V(FUN_0052e710),V(FUN_0052e6c0));
  return 1;
}
