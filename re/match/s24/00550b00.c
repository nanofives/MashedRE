// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 h3 0x00550b00: 100% (regs 100%). The path-to-filesystem lookup (':' scan, list search by name, default FS at DAT_007dc76c, error callback DAT_007dc768(6)) is a static __forceinline helper GetFS (file-local, no address of its own, plain __inline was not inlined under /Ob1), and the caller is if (!fs) return 0; return fs->fn4c(fs, path). One flat function body peaked at 69.1%, the if (fs) return call form at 86.2%. */
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
typedef struct FS FS;
struct FS {
  FS *next;            /* +0x00 */
  int pad04[2];
  char *name;          /* +0x0c */
  int pad10[15];
  int (__cdecl *fn4c)(FS *, char *); /* +0x4c */
};
typedef unsigned int (__cdecl *fnf4_t)(const char *);
typedef int (__cdecl *fnf0_t)(const char *, const char *);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(FS *, DAT_007dc754, 0x007dc754);
MGLOBAL(int, DAT_007dc75c, 0x007dc75c);
typedef void (__cdecl *errfn_t)(int);
MGLOBAL(errfn_t, DAT_007dc768, 0x007dc768);
MGLOBAL(FS *, DAT_007dc76c, 0x007dc76c);
char *__cdecl _strncpy(char *, const char *, unsigned int);
// MATCH: 0x00550b00 FUN_00550b00
static __forceinline FS *GetFS(char *path)
{
  unsigned int len, i;
  FS *fs;
  char name[100];
  if (DAT_007dc75c) {
    len = (*(fnf4_t *)(DAT_007d3ff8 + 0xf4))(path);
    for (i = 0; i < len; i++) {
      if (path[i] == ':') {
        _strncpy(name, path, i + 1);
        name[i + 1] = 0;
        for (fs = DAT_007dc754; fs; fs = fs->next)
          if ((*(fnf0_t *)(DAT_007d3ff8 + 0xf0))(name, fs->name) == 0) return fs;
      }
    }
    if (DAT_007dc76c) return DAT_007dc76c;
  }
  if (DAT_007dc768) DAT_007dc768(6);
  return 0;
}
int FUN_00550b00(char *path)
{
  FS *fs = GetFS(path);
  if (!fs) return 0;
  return fs->fn4c(fs, path);
}
