// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004ce790 (h3): 100% (regs 100%). Image format register: ext = '.' + strncpy(ext, 0x12) lowered (RwGlobals+0xd0, +0xfc); search the format list (module+0x21c, module = RwGlobals + DAT_007d4628) with strcmp (+0xe8) in a while (fmt && !found) loop; when absent allocate from the freelist at module+0x218 (+0x118, hint 0x30406), memcpy the lower ext, build the upper ext (+0xf8), clear read/write, push on the list; then set read/write when non-NULL; returns 1. From 40.0%: format struct, memcpy intrinsic, the found/fmt loop. Relocheck OK. */
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
typedef struct ImgFmt {
  char lcExt[0x14];
  char ucExt[0x14];
  void *readFn;
  void *writeFn;
  struct ImgFmt *next;
} ImgFmt;
typedef struct Mod { char pad[0x218]; void *freeList; ImgFmt *formats; } Mod;
typedef char *(*fpStrncpy)(char *d, const char *s, unsigned int n);
typedef char *(*fpStrcase)(char *s);
typedef int (*fpStrcmp)(const char *a, const char *b);
typedef void *(*fpFreeListAlloc)(void *fl, unsigned int hint);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4628, 0x007d4628);
#define MOD ((Mod *)(DAT_007d3ff8 + DAT_007d4628))

// MATCH: 0x004ce790 FUN_004ce790
int FUN_004ce790(const char *param_1, void *param_2, void *param_3)
{
  char ext[0x14];
  ImgFmt *fmt;
  ImgFmt *found;

  ext[0] = '.';
  (*(fpStrncpy *)(DAT_007d3ff8 + 0xd0))(&ext[1], param_1, 0x12);
  ext[0x13] = 0;
  (*(fpStrcase *)(DAT_007d3ff8 + 0xfc))(ext);
  fmt = MOD->formats;
  found = 0;
  while (fmt && !found) {
    if (!(*(fpStrcmp *)(DAT_007d3ff8 + 0xe8))(fmt->lcExt, ext)) {
      found = fmt;
    } else {
      fmt = fmt->next;
    }
  }
  if (!found) {
    found = (ImgFmt *)(*(fpFreeListAlloc *)(DAT_007d3ff8 + 0x118))(MOD->freeList, 0x30406);
    if (!found) {
      return 0;
    }
    memcpy(found->lcExt, ext, 0x14);
    found->ucExt[0] = '.';
    (*(fpStrncpy *)(DAT_007d3ff8 + 0xd0))(&found->ucExt[1], param_1, 0x12);
    found->ucExt[0x13] = 0;
    (*(fpStrcase *)(DAT_007d3ff8 + 0xf8))(found->ucExt);
    found->readFn = 0;
    found->writeFn = 0;
    found->next = MOD->formats;
    MOD->formats = found;
  }
  if (param_2) {
    found->readFn = param_2;
  }
  if (param_3) {
    found->writeFn = param_3;
  }
  return 1;
}
