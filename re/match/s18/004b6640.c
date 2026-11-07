// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b6640 (h4): 60.5%, regs 88.4% (instruction-aligned; ops 93.0%). By hand from g2s (48%). Residual: the original keeps p in EDX through the stores and copies it to ESI (mov esi,edx after push edx) only for the last call FUN_00550430(p); 9176 puts p in ESI from the start, which shifts every store's base register. Tried: file-static DAT_007d3e54/8cd9f8/8eda70 (81.4% but tail-calls a reload, wrong), a second local q (from p or from each global), __inline helpers for alloc/memset/fn-pointer stores/global stores, register keyword, flagscan (best /O2), permuter 330 compiles (no gain). Class (b): source shape not found. */
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
typedef struct { void *f[0x16]; } S_004b6640;
typedef void *(*RwMallocFn)(int, int);
MGLOBAL(void *, PTR_DAT_006172f0, 0x006172f0);
MGLOBAL(void *, PTR_DAT_006172f4, 0x006172f4);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d3e50, 0x007d3e50);
MGLOBAL(S_004b6640 *, DAT_007d3e54, 0x007d3e54);
MGLOBAL(S_004b6640 *, DAT_008cd9f8, 0x008cd9f8);
MGLOBAL(int, DAT_008cda20, 0x008cda20);
MGLOBAL(S_004b6640 *, DAT_008eda70, 0x008eda70);
MGLOBAL(int, DAT_008eda98, 0x008eda98);
void FUN_005504d0(void *p0);
void FUN_00551330(S_004b6640 *p0, int p1, void *p2, void *p3);
void FUN_00550430(S_004b6640 *p0);
void FUN_004b6630(void);
void FUN_0045b350(void);
void FUN_004b6c00(void);
void FUN_004b6f60(void);
void FUN_004b6cd0(void);
void FUN_004b6f90(void);
void FUN_004b6ed0(void);
void FUN_004b6ba0(void);
void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)

// MATCH: 0x004b6640 FUN_004b6640
void FUN_004b6640(int param_1)
{
  S_004b6640 *p;

  FUN_005504d0(PTR_DAT_006172f0);
  p = (S_004b6640 *)(*(RwMallocFn *)(DAT_007d3ff8 + 0x108))(0x58, 0);
  memset(p, 0, 0x58);
  p->f[5] = (void *)FUN_004b6630;
  p->f[6] = (void *)FUN_0045b350;
  p->f[10] = (void *)FUN_004b6c00;
  p->f[11] = (void *)FUN_004b6f60;
  p->f[12] = (void *)FUN_004b6cd0;
  p->f[17] = (void *)FUN_004b6f90;
  p->f[14] = (void *)FUN_004b6ed0;
  p->f[19] = (void *)FUN_004b6ba0;
  DAT_007d3e50 = param_1;
  DAT_007d3e54 = p;
  DAT_008cd9f8 = p;
  DAT_008cda20 = 0;
  DAT_008eda70 = p;
  DAT_008eda98 = 1;
  FUN_00551330(p, 2, PTR_DAT_006172f0, PTR_DAT_006172f4);
  FUN_00550430(p);
}
