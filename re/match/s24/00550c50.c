// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00550c50 (h4): nosize.py 79.8% (regs 81.8%); 80.0% (regs 82.0%) at the real size. No Ghidra size: the
   original ends at the ret at 0x00550d79 (298 bytes, then 90 padding up to 0x00550d80). File open (RtFSHandler
   open): builds the path with FUN_00551460(path, 0x100, dir, FUN_00551410(name), '\'), maps the mode bits to
   CreateFileA access/share/disposition/attributes, then GetFileSize and the GetLastError slot. Callee arity fixed
   from the disassembly: FUN_00551410 takes 1 argument and FUN_00551460 takes 5 (the 0x5c Ghidra gave
   FUN_00551410 is FUN_00551460's 5th argument) (77.0 -> 85.0 -> 80.0 with both right; the 85.0 form pushed 0x5c
   twice). Residual (a): ebx/esi pushed at entry in the original, shrink-wrapped past the early `return 5` in
   ours (the S19 class), which shifts every stack offset in the first block. /O2 /GS /Ob1 62.7, /O1 27.0. */
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
typedef struct { int f0; int f4; int f8; int fc; unsigned long err; } FsErr;
__declspec(dllimport) void *__stdcall CreateFileA(const char *name, unsigned long access, unsigned long share, void *sa, unsigned long disp, unsigned long flags, void *tmpl);
__declspec(dllimport) unsigned long __stdcall GetFileSize(void *h, unsigned long *hi);
__declspec(dllimport) unsigned long __stdcall GetLastError(void);
FsErr *FUN_00550740(void);
char *FUN_00551410(const char *s);
int FUN_00551460(char *dst, int size, const char *dir, const char *name, char sep);
// MATCH: 0x00550c50 FUN_00550c50
int FUN_00550c50(int param_1, unsigned long *param_2, const char *param_3, unsigned char param_4)
{
  char path[256];
  unsigned long access;
  unsigned long share;
  unsigned long disp;
  unsigned long attr;
  void *h;

  if (!FUN_00551460(path, 0x100, *(const char **)(param_1 + 0xc), FUN_00551410(param_3), 0x5c)) {
    return 5;
  }
  access = 0;
  share = 0;
  attr = 0;
  if (param_4 & 0x10) {
    attr = 0x60000000;
    param_2[6] = 1;
    param_2[8] = 3;
  } else {
    param_2[6] = 0;
  }
  if (param_4 & 2) {
    share = 2;
    access = 0x40000000;
    disp = (param_4 & 8) ? 4 : 2;
    if (param_2[6] == 0) {
      attr = 0x80;
    }
  } else {
    disp = 3;
    attr |= 1;
  }
  if (param_4 & 1) {
    access |= 0x80000000;
    share |= 1;
  }
  h = CreateFileA(path, access, share, 0, disp, attr, 0);
  param_2[0x10] = (unsigned long)h;
  if (h == (void *)-1) {
    return 3;
  }
  if (param_2[6] == 0) {
    param_2[8] = 2;
  }
  param_2[9] = 1;
  param_2[2] = 0;
  param_2[3] = 0;
  param_2[0] = GetFileSize(h, param_2 + 1);
  FUN_00550740()->err = GetLastError();
  return 1;
}
