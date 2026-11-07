// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b1500: 100.0% (regs 100.0%). Hand: Obj struct (flags +0x188), byte-wide tests via unsigned char cast, and the first flags read through a volatile uint so the later byte tests are separate memory loads (the original does not CSE the flag load). */
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
typedef unsigned int uint;
typedef void *HANDLE;
typedef int BOOL;
typedef struct Obj {
  char pad0[0x28]; char sub[0x64];
  char s8c[0x7c];
  void (*f108)(int, int); int f10c; char pad110[8];
  int f118; char pad11c[0x28];
  int f144; char pad148[0x20];
  int f168; char pad16c[0x1c];
  uint flags; HANDLE h18c;
} Obj;
__declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE p0);
void FUN_005ab150(void *p0);
void FUN_005aea40(int p0);
void FUN_005b8080(void *p0);
// MATCH: 0x005b1500 FUN_005b1500
void FUN_005b1500(Obj *o)
{
  if ((*(volatile uint *)&o->flags & 0x1000) == 0) {
    if ((*(unsigned char *)&o->flags & 4) != 0) {
      FUN_005ab150(o->sub);
    }
    if (o->f144 != 0 && (*(unsigned char *)&o->flags & 1) == 0) {
      FUN_005aea40(o->f144);
      o->f144 = 0;
    }
    if (o->f168 != 0 && (*(unsigned char *)&o->flags & 2) == 0) {
      FUN_005aea40(o->f168);
      o->f168 = 0;
    }
    if (o->f118 != 0) {
      o->f108(o->f118, o->f10c);
      o->f118 = 0;
    }
    if (o->h18c != (HANDLE)0) {
      CloseHandle(o->h18c);
    }
    FUN_005b8080(o->s8c);
    o->flags = o->flags | 0x1000;
  }
}
