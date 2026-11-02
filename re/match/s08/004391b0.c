// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x004391b0: 100% (regs 100%) by hand, first pass. The original takes its one argument in EAX: that is
   the convention this compiler gives a `static` function defined before its caller (pilot finding 1), so the file
   holds a stand-in caller (caller_004391b0, not matched). The real caller is 0x00439210 (its file holds this
   function too). The raw form had in_EAX and scored 58.3%. Strings are the .rdata "Lock"/"tick"/"Star". relocheck OK. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0067e9fc, 0x0067e9fc);
MGLOBALA(char, s_tick_005cda3c, 5, 0x005cda3c);
MGLOBALA(char, s_Lock_005cda44, 5, 0x005cda44);
MGLOBALA(char, s_Star_005cd970, 5, 0x005cd970);
int FUN_0040bb90(const char *name);
int FUN_00430760(void);
// MATCH: 0x004391b0 FUN_004391b0
static int FUN_004391b0(int kind)
{
  if (kind == 0) {
    return FUN_0040bb90(s_Lock_005cda44);
  }
  if (kind == 1) {
    return FUN_0040bb90(s_Star_005cd970);
  }
  if (kind == 2) {
    if (FUN_00430760() != 0 && DAT_0067e9fc != 10 && DAT_0067e9fc != 2) {
      return FUN_0040bb90(s_tick_005cda3c);
    }
    return 0;
  }
  if (kind == 3) {
    return FUN_0040bb90(s_Star_005cd970);
  }
  return 0;
}

/* stand-in caller (not matched): makes the compiler pass kind in EAX */
int caller_004391b0(int k)
{
  return FUN_004391b0(k) + FUN_004391b0(k + 1);
}
