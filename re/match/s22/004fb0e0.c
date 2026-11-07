// FLAGS: /O2 /Ob1 /G5 /Oy-
/* MATCH-S22 0x004fb0e0 (h7): 100.0% (regs 100.0%) under /O2 /Ob1 /G5 /Oy- (the original keeps an ebp frame; plain
   /G5 scores 69.2%). Callees 0x004ed6ba/0x004ed903/0x004ed983 are d3dx9-psgp dispatch thunks (jmp [ptr]); they pop
   their own args (no add esp), so they are declared __stdcall with the arity of the pushes. */
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
MGLOBAL(void *, DAT_007d4150, 0x007d4150);
MGLOBAL(int, DAT_007d9900, 0x007d9900);
MGLOBAL(undefined1, DAT_006181e0, 0x006181e0);
MGLOBAL(undefined1, DAT_006199b0, 0x006199b0);
MGLOBAL(undefined1, DAT_007d98c0, 0x007d98c0);
MGLOBAL(undefined1, DAT_00911d20, 0x00911d20);
void * __stdcall FUN_004ed6ba(void *a, void *b, void *c);
void * __stdcall FUN_004ed983(void *a, void *b, void *c);
void * __stdcall FUN_004ed903(void *a, void *b);
// MATCH: 0x004fb0e0 FUN_004fb0e0
void FUN_004fb0e0(void *param_1)
{
  if (DAT_007d4150 == 0) {
    FUN_004ed6ba(&DAT_007d98c0, &DAT_00911d20, &DAT_006181e0);
    DAT_007d4150 = &DAT_007d98c0;
  }
  if (DAT_007d9900 != 0) {
    FUN_004ed983(param_1, &DAT_006199b0, DAT_007d4150);
    return;
  }
  FUN_004ed903(param_1, DAT_007d4150);
}
