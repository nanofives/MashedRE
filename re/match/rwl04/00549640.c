// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x00549640 (h2): 100.0% (regs 100). Raw: callbacks as (void*)FUN_x of declared void fns instead of undefined1 globals, 4e7d40 as the plugin register callee, int return; flags /O2 /Ob1 /G5 (not /GS). */
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
MGLOBAL(undefined4, DAT_007dc720, 0x007dc720);
MGLOBAL(undefined4, DAT_007dc724, 0x007dc724);
MGLOBAL(undefined4, DAT_007dc728, 0x007dc728);
void FUN_00549390(void);
void FUN_00549710(void);
void FUN_005497f0(void);
void FUN_00549fe0(void);
void FUN_0054a000(void);
void FUN_0054a030(void);
void FUN_0054a070(void);
void FUN_0054a090(void);
void FUN_0054a0d0(void);
void FUN_0054a0f0(void);
void FUN_005c9d00(void);
void FUN_004d7ff0(void);
void FUN_005492b0(void);
void FUN_005492d0(void);
void FUN_00549310(void);
void FUN_00549350(void);
int FUN_004c2d90(int p0, int p1, void *p2, void *p3);
int FUN_004e7d40(int p0, int p1, void *p2, void *p3, void *p4);
void FUN_004e7da0(int p0, void *p1, void *p2, void *p3);
void FUN_004e7dd0(int p0, void *p1);
void FUN_004e7df0(int p0, void *p1);
int FUN_004e8f50(int p0, int p1, void *p2, void *p3, void *p4);
void FUN_004e8f80(int p0, void *p1, void *p2, void *p3);
void FUN_004e8fb0(int p0, void *p1);
// MATCH: 0x00549640 FUN_00549640
int FUN_00549640(void)
{
  DAT_007dc720 = FUN_004c2d90(0x18,0x123,(void *)FUN_00549710,(void *)FUN_005497f0);
  DAT_007dc724 = FUN_004e7d40(0xc,0x123,(void *)FUN_005492b0,(void *)FUN_005492d0,(void *)FUN_00549310);
  FUN_004e7da0(0x123,(void *)FUN_004d7ff0,(void *)FUN_004d7ff0,(void *)FUN_005c9d00);
  FUN_004e7dd0(0x123,(void *)FUN_00549350);
  FUN_004e7df0(0x123,(void *)FUN_00549390);
  DAT_007dc728 = FUN_004e8f50(8,0x123,(void *)FUN_00549fe0,(void *)FUN_0054a000,(void *)FUN_0054a030);
  FUN_004e8f80(0x123,(void *)FUN_0054a090,(void *)FUN_0054a0d0,(void *)FUN_0054a070);
  FUN_004e8fb0(0x123,(void *)FUN_0054a0f0);
  return 1;
}
