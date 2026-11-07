// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00555910 (h1): 100% (regs 100%) by tools/nosize.py: symbol 466 B, ends 0x00555ae2, then `90` padding
   to the next entry 0x00555af0, bytes_bad=[]. No Ghidra size (hooks.csv name is C++-style). Font .met loader:
   malloc size is strlen(name) + strlen(path) + 5 with hint 0x10190 (2-arg RwMalloc slot +0x108). The failure tail
   is `destroy(font); font = 0;` in both the result-mismatch and the malloc-failed branch, then `return font`. */
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
typedef struct Font Font;
typedef unsigned int (*StrLenFn)(const char *s);
typedef void *(*MallocFn)(unsigned int size, unsigned int hint);
typedef void (*FreeFn)(void *p);
typedef char *(*StrCatFn)(char *dst, const char *src);
typedef int (*StrCmpFn)(const char *a, const char *b);
Font *FUN_005551d0(void);
void *FUN_005507b0(const char *name, const char *mode);
int FUN_00550910(void *stream);
char *FUN_00550a20(char *buf, int size, void *stream);
void FUN_00555830(Font *font);
Font *FUN_00555af0(Font *font, char *work, void *stream, char *line);
Font *FUN_00555ff0(Font *font, char *work, void *stream, char *line);
Font *FUN_00556780(Font *font, char *work, void *stream, char *line);
MGLOBAL(char *, DAT_00912a0c, 0x00912a0c);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(char, DAT_00617378, 2, 0x00617378);
MGLOBALA(char, s_METRICS3_00623eac, 10, 0x00623eac);
MGLOBALA(char, s_METRICS2_00623eb8, 10, 0x00623eb8);
MGLOBALA(char, s_METRICS1_00623ec4, 10, 0x00623ec4);
MGLOBALA(char, DAT_00623ed0, 5, 0x00623ed0);

// MATCH: 0x00555910 FUN_00555910
Font *FUN_00555910(const char *param_1)
{
  Font *font;
  Font *result;
  char *path;
  const char *cursor;
  void *stream;
  int len;
  int i;
  char c;
  char line[256];
  char work[128];

  font = FUN_005551d0();
  if (font == 0) {
    return 0;
  }
  len = (*(StrLenFn *)(DAT_007d3ff8 + 0xf4))(DAT_00912a0c);
  path = (char *)(*(MallocFn *)(DAT_007d3ff8 + 0x108))((*(StrLenFn *)(DAT_007d3ff8 + 0xf4))(param_1) + len + 5, 0x10190);
  if (path != 0) {
    cursor = DAT_00912a0c;
    i = 0;
    stream = 0;
    do {
      c = *cursor;
      if (c == ';' || c == '\0') {
        path[i] = '\0';
        (*(StrCatFn *)(DAT_007d3ff8 + 0xd4))(path, param_1);
        (*(StrCatFn *)(DAT_007d3ff8 + 0xd4))(path, DAT_00623ed0);
        stream = FUN_005507b0(path, DAT_00617378);
        i = 0;
      }
      else {
        path[i] = c;
        i++;
      }
      cursor++;
    } while (c != '\0' && stream == 0);
    result = 0;
    if (stream != 0) {
      FUN_00550a20(line, 0x100, stream);
      if ((*(StrCmpFn *)(DAT_007d3ff8 + 0xe8))(line, s_METRICS1_00623ec4) == 0) {
        result = FUN_00555af0(font, work, stream, line);
      }
      else if ((*(StrCmpFn *)(DAT_007d3ff8 + 0xe8))(line, s_METRICS2_00623eb8) == 0) {
        result = FUN_00555ff0(font, work, stream, line);
      }
      else if ((*(StrCmpFn *)(DAT_007d3ff8 + 0xe8))(line, s_METRICS3_00623eac) == 0) {
        result = FUN_00556780(font, work, stream, line);
      }
      FUN_00550910(stream);
    }
    (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(path);
    if (result != font) {
      FUN_00555830(font);
      font = 0;
    }
  }
  else {
    FUN_00555830(font);
    font = 0;
  }
  return font;
}
