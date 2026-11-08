// FLAGS: /O1
/* MATCH-S21 0x004ecb67 (h4): 100% (regs 100%) under /O1, written from odis (the raw converter did not compile). A 6-byte forwarding thunk, jmp dword ptr [0x006187c4]. A void(void) call through a global function pointer gives that tail jump. The original forwards whatever stack arguments its caller pushed. The C form declares none, so the bytes are identical and the port must not read this prototype as the callee's real signature. Residual: none. */
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
typedef void (*FwdFn)(void);
MGLOBAL(FwdFn, DAT_006187c4, 0x006187c4);
// MATCH: 0x004ecb67 FUN_004ecb67
void FUN_004ecb67(void)
{
  DAT_006187c4();
}
