// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041e8c0: 100% by hand. Original: mov edx, [0x0063d7e4]; jmp [edx + 0x18] (a tail call
   through the function pointer at +0x18 of the record DAT_0063d7e4 points at, stack args passed through).
   Under this compiler the scratch register is set by the number of stack args: 0/3 -> eax, 1/4 -> ecx, 2/5 -> edx
   (checked for 0..4 args, return type has no effect). 2 args is the smallest count that gives edx; the bytes and
   the behaviour (a jmp leaves the caller's args untouched) are the same for 5. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef void (*MatchFn0041e8c0)(int, int);
MGLOBAL(char *, DAT_0063d7e4, 0x0063d7e4);
// MATCH: 0x0041e8c0 FUN_0041e8c0
void FUN_0041e8c0(int a1, int a2)
{
  (*(MatchFn0041e8c0 *)(DAT_0063d7e4 + 0x18))(a1, a2);
}
