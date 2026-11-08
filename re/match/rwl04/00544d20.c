// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00544d20 (h2): 86.7% (regs 86.7). Callee 0x004e7d40 is the registered-plugin call (raw's undefined RpAtomicRegisterPlugin), fn addresses passed as (void*)FUN_x, int return, if(x<0) return 0 chain with final 'return 1' (setge folded). Residue: original keeps a separate 'xor eax,eax; ret' after the 2nd and 3rd tests, ours merges them into one shared exit. class (a). */
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
typedef unsigned int undefined4;
MGLOBAL(int, DAT_007dc66c, 0x007dc66c);
MGLOBAL(int, DAT_007dc670, 0x007dc670);
void FUN_00544dc0(void);
void FUN_00544dd0(void);
void FUN_00544de0(void);
void FUN_00544e00(void);
void FUN_00544f10(void);
void FUN_00544f50(void);
void FUN_00544fc0(void);
void FUN_00545080(void);
void FUN_005451d0(void);
void FUN_005451f0(void);
int FUN_004c2d90(int p0, int p1, void *p2, void *p3);
int FUN_004e7d40(int p0, int p1, void *p2, void *p3, void *p4);
int FUN_004e8f50(int p0, int p1, void *p2, void *p3, void *p4);
int FUN_004e8f80(int p0, void *p1, void *p2, void *p3);
// MATCH: 0x00544d20 FUN_00544d20
int FUN_00544d20(void)
{
  int iVar1;
  iVar1 = FUN_004c2d90(0,0x105,(void *)FUN_00544dc0,(void *)FUN_00544dd0);
  if (iVar1 < 0) return 0;
  DAT_007dc66c = FUN_004e7d40(0xc,0x105,(void *)FUN_00544de0,(void *)FUN_00544de0,(void *)FUN_00544e00);
  if (DAT_007dc66c < 0) return 0;
  DAT_007dc670 = FUN_004e8f50(0xc,0x105,(void *)FUN_005451f0,(void *)FUN_00544f10,(void *)FUN_00544f50);
  if (DAT_007dc670 < 0) return 0;
  iVar1 = FUN_004e8f80(0x105,(void *)FUN_00545080,(void *)FUN_00544fc0,(void *)FUN_005451d0);
  if (iVar1 < 0) return 0;
  return 1;
}
