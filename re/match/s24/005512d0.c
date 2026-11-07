// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 h3 0x005512d0: no Ghidra size; nosize.py: acc 100, regs 100, bytes_bad=[], 90 B, ends 0x0055132a on nop padding before 0x00551330. Ghidra's callee arity was wrong: FUN_00551410 takes 1 argument and the 0x5c is the 5th argument of FUN_00551460 (the add esp,4 after the first call shows it). RtFSHandler method stored at handler+0x4c by 0x00551190. */
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
__declspec(dllimport) unsigned long __stdcall GetFileAttributesA(const char *lpFileName);
char *__cdecl FUN_00551410(char *path);
int __cdecl FUN_00551460(char *dst, int size, char *dir, char *name, int sep);
// MATCH: 0x005512d0 FUN_005512d0
int FUN_005512d0(int param_1, char *param_2)
{
  char buf[256];
  if (!FUN_00551460(buf, 0x100, *(char **)(param_1 + 0xc), FUN_00551410(param_2), 0x5c)) return 0;
  return GetFileAttributesA(buf) != 0xffffffff;
}
