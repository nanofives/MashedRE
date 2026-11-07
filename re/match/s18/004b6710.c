// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 0x004b6710 (h4): 100%, regs 100%. By hand (g2s did not compile). The file name arrives in EAX (Ghidra in_EAX): pilot finding 1, FUN_004b6710 is a static function defined before a small stand-in caller (MatchStandIn_004b6710, not a matched function, the real caller is 0x004b6940). fopen is FUN_004a4541(name, "rb"), CreateFileA dwShareMode 0. */
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
MGLOBAL(unsigned char, DAT_007d3e50, 0x007d3e50);
MGLOBAL(void *, DAT_007d3e48, 0x007d3e48);
MGLOBALA(char, s_rb_005cf010, 3, 0x005cf010);
void *FUN_004a4541(const char *p0, const char *p1);
__declspec(dllimport) void *__stdcall CreateFileA(const char *, unsigned long, unsigned long, void *, unsigned long, unsigned long, void *);
__declspec(dllimport) unsigned long __stdcall GetLastError(void);

// MATCH: 0x004b6710 FUN_004b6710
static void *FUN_004b6710(const char *param_1)
{
  if (DAT_007d3e50 & 1) {
    DAT_007d3e48 = FUN_004a4541(param_1, s_rb_005cf010);
    return DAT_007d3e48;
  }
  DAT_007d3e48 = CreateFileA(param_1, 0x80000000, 0, 0, 3, 0x60000000, 0);
  if (GetLastError() == 0 && DAT_007d3e48 != (void *)-1) {
    return (void *)1;
  }
  return 0;
}

/* stand-in caller (not a matched function): makes 9176 pass param_1 in EAX */
int MatchStandIn_004b6710(const char *name)
{
  return FUN_004b6710(name) != 0;
}
