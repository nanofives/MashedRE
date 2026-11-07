// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cd140 (h3): 100% (regs 100%). Returns 0 when module+0x3c (request ptr) is NULL, else clears the 0x3c-byte request record at module+0x38 (memset intrinsic, rep stosd x15) and returns 1. Module = RwGlobals + DAT_00911d00. Relocheck OK. */
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
void *memset(void *d, int c, unsigned int n);
#pragma intrinsic(memset)
typedef struct Req { short size; short pad; void *ptr; int type; unsigned int flags; void *data; int rest[10]; } Req;
typedef struct Mod { void *handle; int pad[13]; Req req; } Mod;
MGLOBAL(int, DAT_00911d00, 0x00911d00);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
#define MOD ((Mod *)(DAT_007d3ff8 + DAT_00911d00))

// MATCH: 0x004cd140 FUN_004cd140
int FUN_004cd140(void)
{
  if (!MOD->req.ptr) {
    return 0;
  }
  memset(&MOD->req, 0, sizeof(Req));
  return 1;
}
