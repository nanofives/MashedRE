// FLAGS: /O2 /GS /Ob1
/* re/match/topm01/0047ba00.c -- PORT-TOP-MATCH-01: 0x0047ba00 (hooks.csv FUN_0047ba00), 14 B.
   match.py: 100.0% (regs 100.0%, ops 100.0%) 5/5 ins, whole symbol BYTE-IDENTICAL (relocations masked).
   Was 0% in re/match/s13/0047ba00.c (every form there compiled to the 5-byte tail jmp).
   Facts read from the disassembly (re/tools/match/odis.py):
     Body 0x0047ba00: mov eax,[esp+4]; push eax; call 0x00496940; add esp,4; ret.
     Only caller site 0x00402944 (in 0x00402750): push 0 at 0x00402937, push eax (the return of 0x004671a0)
     at 0x00402943, call, add esp,8 at 0x00402949. So 0x0047ba00 takes two stack arguments and reads the first.
     The caller does not read eax after the call (0x0040294c calls 0x00499ce0 next).
   What closed it: the second parameter plus returning 0x00496940's int result. One parameter gives the tail
   jmp (0%); two parameters with a void body give pop ecx in place of add esp,4 (80%).
   0x00496940 is declared without a prototype: its matched body takes no argument
   (re/match/s16/00496940.c), and the original still pushes one here.
   Residual: none. */
/* ---- MATCH-TOPM01 preamble (same text in every re/match/topm01 source; not part of any function body) ----
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
#ifdef __cplusplus
int FUN_00496940(...);
#else
int FUN_00496940();
#endif
// MATCH: 0x0047ba00 FUN_0047ba00
int FUN_0047ba00(int param_1, int param_2)
{
  return FUN_00496940(param_1);
}
