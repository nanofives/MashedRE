// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x005336d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(int, DAT_00912468, 0x00912468);
void FUN_00533740(void);
void FUN_005337f0(void);
void FUN_00533820(void);
void FUN_00533840(void);
void FUN_00533900(void);
void FUN_005339a0(void);
void FUN_00533ad0(void);
void FUN_00533cc0(void);
int FUN_004c1710(int p0, int p1, void *p2, void *p3, void *p4);
int FUN_004c2d90(int p0, int p1, void *p2, void *p3);
int FUN_005c4780(int p0, void *p2, void *p3, void *p4);
// MATCH: 0x005336d0 FUN_005336d0
int FUN_005336d0(void)
{
  int iVar1;

  iVar1 = FUN_004c2d90(0,0x11e,(void *)FUN_00533740,(void *)FUN_005337f0);
  if (iVar1 >= 0) {
    DAT_00912468 = FUN_004c1710(8,0x11e,(void *)FUN_00533820,(void *)FUN_00533840,(void *)FUN_00533900);
    iVar1 = FUN_005c4780(0x11e,(void *)FUN_00533ad0,(void *)FUN_005339a0,(void *)FUN_00533cc0);
    if ((iVar1 >= 0) && (DAT_00912468 >= 0)) {
      return 1;
    }
  }
  return 0;
}
