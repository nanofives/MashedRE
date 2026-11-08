// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004ce390 (h3): 100% (regs 100%). Image read by extension: strip the path with RwGlobals+0xdc strrchr on ':', '/', 0x5c, then find '.'; walk the format list at module+0x21c (module = RwGlobals + DAT_007d4628; entry: lower ext +0, upper ext +0x14, read fn +0x28, next +0x30) comparing with RwGlobals+0xe8; on a hit with a read fn calls FUN_004ce480(path, 5, FUN_004ce690, &{readFn, result=0}) and returns result. From 77.6%: structs, the read-fn NULL test on fmt->readFn before filling the context (moved the result store after the pushes). Relocheck OK. */
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
typedef struct ImgFmt {
  char lcExt[0x14];
  char ucExt[0x14];
  void *readFn;
  void *writeFn;
  struct ImgFmt *next;
} ImgFmt;
typedef struct Mod { char pad[0x21c]; ImgFmt *formats; } Mod;
typedef struct ReadCtx { void *readFn; void *result; } ReadCtx;
typedef char *(*fpStrrchr)(const char *s, int c);
typedef int (*fpStrcmp)(const char *a, const char *b);
typedef int (*FileCB)(void *a, void *b);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4628, 0x007d4628);
int FUN_004ce690(void *a, void *b);
int FUN_004ce480(const char *path, int mode, FileCB cb, void *data);
#define MOD ((Mod *)(DAT_007d3ff8 + DAT_007d4628))

// MATCH: 0x004ce390 FUN_004ce390
void *FUN_004ce390(const char *param_1)
{
  ReadCtx ctx;
  const char *name;
  const char *p;
  ImgFmt *fmt;

  name = param_1;
  p = (*(fpStrrchr *)(DAT_007d3ff8 + 0xdc))(name, ':');
  if (p) name = p;
  p = (*(fpStrrchr *)(DAT_007d3ff8 + 0xdc))(name, '/');
  if (p) name = p;
  p = (*(fpStrrchr *)(DAT_007d3ff8 + 0xdc))(name, 0x5c);
  if (p) name = p;
  p = (*(fpStrrchr *)(DAT_007d3ff8 + 0xdc))(name, '.');
  if (p) {
    for (fmt = MOD->formats; fmt; fmt = fmt->next) {
      if (!(*(fpStrcmp *)(DAT_007d3ff8 + 0xe8))(fmt->lcExt, p) ||
          !(*(fpStrcmp *)(DAT_007d3ff8 + 0xe8))(fmt->ucExt, p)) {
        if (!fmt->readFn) {
          return 0;
        }
        ctx.readFn = fmt->readFn;
        ctx.result = 0;
        FUN_004ce480(param_1, 5, FUN_004ce690, &ctx);
        return ctx.result;
      }
    }
  }
  return 0;
}
