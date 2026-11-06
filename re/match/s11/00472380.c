// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00472380 (h7): 100%. RW plugin registration; int return (setge into ecx). RpAtomicRegisterPlugin called
   as FUN_004e7d40. Residual: none. */
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
MGLOBAL(int, DAT_0086ecd4, 0x0086ecd4);
void *FUN_004d7ff0(void *p0, int p1, int p2);
void *FUN_00472330(void *p0, int p1, int p2);
void *FUN_00472360(void *p0, void *p1, int p2, int p3);
int FUN_004c2d90(int p0, int p1, void *p2, void *p3);
int FUN_004e7d40(int p0, int p1, void *p2, void *p3, void *p4);
// MATCH: 0x00472380 FUN_00472380
int FUN_00472380(void)
{
  if (FUN_004c2d90(0,4,(void *)FUN_004d7ff0,(void *)FUN_004d7ff0) < 0) {
    return 0;
  }
  DAT_0086ecd4 = FUN_004e7d40(0x10,0x26990004,(void *)FUN_00472330,(void *)FUN_004d7ff0,(void *)FUN_00472360);
  return DAT_0086ecd4 >= 0;
}
