// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004ce480 (h3): 89.7% (regs 89.7%, ops 93.7%). Image path search: when the name is relative (FUN_004e1a90 == 0) and the image path at module+4 (module = RwGlobals + DAT_007d4628) is non-empty, for each ';'-separated entry (RwGlobals+0xe0 strchr, called twice as in the original; +0xf4 strlen) allocate strlen(name) + len + extra with FUN_004ce5d0, memcpy the directory, strcpy (+0xcc) the name after it, and call cb(buf, data) until it returns 0 or the list ends; otherwise one buffer with the bare name. Returns the name, or NULL when the buffer allocation fails. From 54.4%: module struct, memcpy intrinsic, the while (path) { if (!*path) break; } loop (84.9%), the simple branch written first (permuter: 89.7%). Residual: placement of the two NULL-return epilogues (the original shares one after the simple branch and puts the loop's early return before it). Tried: both branch orders, buf + len / &buf[len], a pre-NUL store (byte count off), permuter 154 compiles. Class (b). The raw converter's file did not compile as C++; this one does. */
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
typedef struct Mod { int pad; char *imagePath; } Mod;
typedef char *(*fpStrchr)(const char *s, int c);
typedef unsigned int (*fpStrlen)(const char *s);
typedef char *(*fpStrcpy)(char *d, const char *s);
typedef int (*FileCB)(char *path, void *data);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4628, 0x007d4628);
int FUN_004e1a90(const char *path);
char *FUN_004ce5d0(unsigned int size);
#define MOD ((Mod *)(DAT_007d3ff8 + DAT_007d4628))

// MATCH: 0x004ce480 FUN_004ce480
const char *FUN_004ce480(const char *param_1, int param_2, FileCB param_3, void *param_4)
{
  const char *path;
  char *next;
  unsigned int len;
  char *buf;

  path = MOD->imagePath;
  if (FUN_004e1a90(param_1) || !path || !*path) {
    buf = FUN_004ce5d0((*(fpStrlen *)(DAT_007d3ff8 + 0xf4))(param_1) + param_2);
    if (!buf) {
      return 0;
    }
    (*(fpStrcpy *)(DAT_007d3ff8 + 0xcc))(buf, param_1);
    param_3(buf, param_4);
  } else {
    while (path) {
      if (!*path) {
        break;
      }
      (*(fpStrchr *)(DAT_007d3ff8 + 0xe0))(path, ';');
      next = (*(fpStrchr *)(DAT_007d3ff8 + 0xe0))(path, ';');
      if (next) {
        len = next - path;
        next++;
      } else {
        len = (*(fpStrlen *)(DAT_007d3ff8 + 0xf4))(path);
      }
      buf = FUN_004ce5d0((*(fpStrlen *)(DAT_007d3ff8 + 0xf4))(param_1) + len + param_2);
      if (!buf) {
        return 0;
      }
      memcpy(buf, path, len);
      (*(fpStrcpy *)(DAT_007d3ff8 + 0xcc))(buf + len, param_1);
      if (!param_3(buf, param_4)) {
        return param_1;
      }
      path = next;
    }
  }
  return param_1;
}
