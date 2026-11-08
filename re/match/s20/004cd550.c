// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cd550 (h3): 50.0% (regs 60.4%); jump tables at 0x004cd7d4 / 0x004cd7ec past Ghidra's size. FLAGS /G5 (no cookie). Set/get of the six file-function handles in the module (RwGlobals + DAT_00911d00): with a non-NULL arg, type 1..6 stores it at +0x10/+0x14/+4/+0xc/+8/+0x18 and returns it; with NULL, copies the default at +0x2c/+0x30/+0x20/+0x28/+0x24/+0x34 (NULL when the default is NULL) and returns the handle (types 3, 5) or the default (types 1, 2, 4, 6); default RWERROR(0x25, type). Cases are written in the binary's body order 3,5,4,1,2,6. Residual: the original keeps every get-case as if (d) h = d; else h = 0 (two stores, esi = 0 preloaded, pop esi on each return path); 9176 folds that into one copy here, so every get case is shorter and the frame/register choices shift. Tried: if/else, ternary, an int-typed store in the else branch, permuter 220 compiles (51.1% by reordering RwError field stores, not kept). Class (b). */
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
typedef struct RwError { int pluginID; int errorCode; } RwError;
typedef struct Mod {
  void *handle;
  void *h4;
  void *h8;
  void *hc;
  void *h10;
  void *h14;
  void *h18;
  void *h1c;
  void *d20;
  void *d24;
  void *d28;
  void *d2c;
  void *d30;
  void *d34;
} Mod;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911d00, 0x00911d00);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);
#define MOD ((Mod *)(DAT_007d3ff8 + DAT_00911d00))

// MATCH: 0x004cd550 FUN_004cd550
void *FUN_004cd550(void *param_1, int param_2)
{
  if (param_1) {
    switch (param_2) {
    case 3:
      MOD->h4 = param_1;
      return param_1;
    case 5:
      MOD->h8 = param_1;
      return param_1;
    case 4:
      MOD->hc = param_1;
      return param_1;
    case 1:
      MOD->h10 = param_1;
      return param_1;
    case 2:
      MOD->h14 = param_1;
      return param_1;
    case 6:
      MOD->h18 = param_1;
      return param_1;
    default: {
      RwError err;
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(0x25, param_2);
      FUN_004d8480(&err);
      return 0;
    }
    }
  }
  switch (param_2) {
  case 3:
    if (MOD->d20) {
      MOD->h4 = MOD->d20;
    } else {
      MOD->h4 = 0;
    }
    return MOD->h4;
  case 5:
    if (MOD->d24) {
      MOD->h8 = MOD->d24;
    } else {
      MOD->h8 = 0;
    }
    return MOD->h8;
  case 4:
    if (MOD->d28) {
      MOD->hc = MOD->d28;
    } else {
      MOD->hc = 0;
    }
    return MOD->d28;
  case 1:
    if (MOD->d2c) {
      MOD->h10 = MOD->d2c;
    } else {
      MOD->h10 = 0;
    }
    return MOD->d2c;
  case 2:
    if (MOD->d30) {
      MOD->h14 = MOD->d30;
    } else {
      MOD->h14 = 0;
    }
    return MOD->d30;
  case 6:
    if (MOD->d34) {
      MOD->h18 = MOD->d34;
    } else {
      MOD->h18 = 0;
    }
    return MOD->d34;
  default: {
    RwError err;
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x25, param_2);
    FUN_004d8480(&err);
    return 0;
  }
  }
}
