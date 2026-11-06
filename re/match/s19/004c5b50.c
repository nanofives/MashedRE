// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c5b50 (h4): 100% (regs 100%), relocheck OK. RwTextureSet*Name shape: strncpy slot +0xd0 (32), strlen slot +0xf4, >= 32 -> RWERROR {pluginID 1, _rwerror(0x8000001e,...)} + RwErrorSet, terminate byte 31. RwError local struct and >= 0x20 test fixed the frame and the jb. */
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
typedef struct { int pluginID; int errorCode; } RwError;
typedef void (*fpcpy)(char *, const char *, unsigned int);
typedef unsigned int (*fplen)(const char *);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_004d7ff0(int code, ...);
RwError * FUN_004d8480(RwError * p0);
// MATCH: 0x004c5b50 FUN_004c5b50
char * FUN_004c5b50(char *texture,const char *name)
{
  (*(fpcpy *)(DAT_007d3ff8 + 0xd0))(texture + 0x30,name,0x20);
  if ((*(fplen *)(DAT_007d3ff8 + 0xf4))(name) >= 0x20) {
    RwError err;
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x8000001e,name,0x20,0x1f,(int)name[0x1f]);
    FUN_004d8480(&err);
    texture[0x4f] = 0;
  }
  return texture;
}
