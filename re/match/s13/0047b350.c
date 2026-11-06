// FLAGS: /O2 /GS /Ob1
/* 0x0047b350 (Lua C function, registers a name in the 0x2510-byte record table at 0x0086ece0): 100% (regs 100%),
   relocheck OK. From raw 85.7%: gettop result held across the global load (n first, then i = DAT_00691480),
   the copy is the strcpy intrinsic. KtcRec layout is only what this file and 0x0047b420 touch. Residual: none. */
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
typedef struct lua_State lua_State;
typedef struct KtcRec {
  char name[0x20];
  int ids[8];
  int count;
  char pad[0x2510 - 0x44];
} KtcRec;
MGLOBAL(int, DAT_00691480, 0x00691480);
MGLOBALA(KtcRec, DAT_0086ece0, 16, 0x0086ece0);
int FUN_004b6fc0(lua_State *L);
const char *FUN_004b70d0(lua_State *L, int idx);
void FUN_004b7140(lua_State *L, double n);
char *strcpy(char *d, const char *s);
#pragma intrinsic(strcpy)
// MATCH: 0x0047b350 FUN_0047b350
int FUN_0047b350(lua_State *L)
{
  int n;
  int i;
  n = FUN_004b6fc0(L);
  i = DAT_00691480;
  strcpy(DAT_0086ece0[i].name, FUN_004b70d0(L, n));
  DAT_00691480++;
  FUN_004b7140(L, (double)i);
  return 1;
}
