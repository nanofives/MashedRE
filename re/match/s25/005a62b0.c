// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a62b0: hand-matched (h1). */
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

/* best 77.6% (regs 77.6%, ops 79.6%): residual = cross-jump. Original keeps the three failure returns as separate inline
   blocks (jne cont; pop esi; ret) and duplicates the "push esi; call FUN_005a9d00; xor eax,eax; ret" tail; ours merges them into one
   shared tail. Tried return iVar1 on the first failure, /O1 /GS /Ox /Og /Ob2 /Oy-: no change. Class (a) (cross-jump merge). */
MGLOBAL(undefined4, DAT_009146fc, 0x009146fc);
MGLOBAL(undefined4, DAT_007dc9e4, 0x007dc9e4);
MGLOBAL(void *, PTR_DAT_00632c50, 0x00632c50);
MGLOBAL(void *, PTR_DAT_00632da0, 0x00632da0);
MGLOBAL(undefined, DAT_007dc968, 0x007dc968);
MGLOBAL(undefined, DAT_007dc980, 0x007dc980);
MGLOBAL(undefined, DAT_007dc9dc, 0x007dc9dc);
void FUN_005a6340(int p0);
int FUN_005a9ab0(undefined4 p0, int p1, undefined *p2, undefined *p3, undefined *p4, undefined4 p5);
undefined4 FUN_005a9d00(int p0);
int FUN_005a9de0(int p0, int p1);
int FUN_005aa200(int p0, int p1, int p2, int p3, int p4);
int FUN_005aeb30(void);
// MATCH: 0x005a62b0 FUN_005a62b0
int FUN_005a62b0(void)
{
  int iVar1;
  int iVar2;
  iVar1 = FUN_005a9ab0(DAT_009146fc,0xc,&DAT_007dc980,&DAT_007dc968,&DAT_007dc9dc,0x30810);
  if (iVar1 == 0) {
    return iVar1;
  }
  FUN_005a6340(iVar1);
  iVar2 = FUN_005aa200(iVar1,(int)&PTR_DAT_00632c50,0xe,(int)&PTR_DAT_00632da0,0xf);
  if (iVar2 == 0) {
    FUN_005a9d00(iVar1);
    return 0;
  }
  iVar2 = FUN_005aeb30();
  if (iVar2 == 0) {
    FUN_005a9d00(iVar1);
    return 0;
  }
  FUN_005a9de0(iVar1,iVar2);
  DAT_007dc9e4 = 1;
  return iVar1;
}
