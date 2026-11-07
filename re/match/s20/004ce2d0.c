// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004ce2d0 (h3): 100% (regs 100%). FLAGS /G5 (no cookie; the converter's /GS pick did not hold). len = RwGlobals+0xf4 strlen(s) + 1; grows the module buffer (RwGlobals + DAT_007d4628: +4 buffer, +8 size) with RwGlobals+0x110 realloc(buffer, len, 0x1040406) when len > size, RWERROR(0x80000013, len) and NULL on failure; memcpy intrinsic into the buffer; returns the argument. From 52.1%: module struct, memcpy, /G5. Relocheck OK. */
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
void *memcpy(void *d, const void *s, unsigned int n);
#pragma intrinsic(memcpy)
typedef struct RwError { int pluginID; int errorCode; } RwError;
typedef struct Mod { int pad; char *buffer; int size; } Mod;
typedef int (*fpStrlen)(const char *s);
typedef void *(*fpRealloc)(void *mem, unsigned int size, unsigned int hint);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4628, 0x007d4628);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);
#define MOD ((Mod *)(DAT_007d3ff8 + DAT_007d4628))

// MATCH: 0x004ce2d0 FUN_004ce2d0
char *FUN_004ce2d0(char *param_1)
{
  RwError err;
  int len;
  char *buf;
  len = (*(fpStrlen *)(DAT_007d3ff8 + 0xf4))(param_1) + 1;
  if (len > MOD->size) {
    buf = (char *)(*(fpRealloc *)(DAT_007d3ff8 + 0x110))(MOD->buffer, len, 0x1040406);
    if (!buf) {
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(0x80000013, len);
      FUN_004d8480(&err);
      return 0;
    }
    MOD->buffer = buf;
    MOD->size = len;
  }
  memcpy(MOD->buffer, param_1, len);
  return param_1;
}
