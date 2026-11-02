// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047f450 (h7): 100% (regs 100%). Raw 68.8%: FUN_004827a0 was a byte global; declared as a function and passed as a function pointer. relocheck OK. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
MGLOBAL(int, DAT_006ce274, 0x006ce274);
void FUN_0047f380(void);
void FUN_004827a0(void);
void FUN_00482820(int p0);
int FUN_0055afc0(int p0, void (*p1)(void), int p2);
int FUN_0055dec0(int p0);
// MATCH: 0x0047f450 FUN_0047f450
void FUN_0047f450(void)
{
  int iVar1;

  if (DAT_006ce274 != 0) {
    iVar1 = FUN_0055dec0(DAT_006ce274);
    FUN_0055afc0(iVar1, FUN_004827a0, iVar1);
    FUN_0047f380();
    FUN_00482820(DAT_006ce274);
  }
}
