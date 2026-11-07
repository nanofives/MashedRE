// FLAGS: /O1
/* MATCH-S28 0x005c22bd: 100% (regs 100%). _sscanf: stack FILE image {ptr,cnt,base,flag=0x49,...} + strlen + FUN_005c35d6(&file, fmt, va=&fmt+1); /O1 (CRT). Class: none. */
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
typedef struct { char *_ptr; int _cnt; char *_base; int _flag; int _file; int _charbuf; int _bufsiz; char *_tmpfname; } FILE_s;
unsigned int FUN_004a9410(const char *p0);
int FUN_005c35d6(FILE_s *p0, const char *p1, void *p2);
// MATCH: 0x005c22bd FUN_005c22bd
int __cdecl FUN_005c22bd(const char *_Src, const char *_Format, ...)
{
  FILE_s f;

  f._flag = 0x49;
  f._base = (char *)_Src;
  f._ptr = (char *)_Src;
  f._cnt = (int)FUN_004a9410(_Src);
  return FUN_005c35d6(&f, _Format, (void *)(&_Format + 1));
}
