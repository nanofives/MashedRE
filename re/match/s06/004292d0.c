// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x004292d0: 100% by hand (h4), regs 100%. This file also holds its callee 0x0055deb0 (100%).
   The original loads the argument into edx, pushes it, calls 0x0055deb0 and then uses edx again as the memcpy source
   base (lea esi, [edx + 8]): the compiler knew the callee leaves edx alone, which it only does for a callee defined
   earlier in the same file. An extern callee gave 82.4% (reload from the stack), a 64-bit or 8-byte-struct return
   68-72%, a static callee 82.4% (register-passed arg). The fit is a NON-static callee defined before the caller.
   0x0055deb0 (mov eax,[esp+4]; mov eax,[eax+4]; ret) sits 1.2 MB away, so this is either a shared TU or a folded
   identical getter (linker ICF): bytes alone cannot tell. Port note: FUN_0055deb0 is a global definition here, so no
   other port file may define it. relocheck OK. Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
extern "C" {
#endif
void *memcpy(void *, const void *, unsigned int);
#ifdef __cplusplus
}
#endif
#pragma intrinsic(memcpy)
#ifndef __cplusplus
// MATCH-HELD: 0x0055deb0 (copy defined before its caller for the DDK register use; port body: Collision/RwpIntegrator.cpp)
uint FUN_0055deb0(int p0)
{
  return *(uint *)(p0 + 4);
}
#else
uint FUN_0055deb0(int p0);   /* port: one body, bound by the wrapper's #define */
#endif
// MATCH: 0x004292d0 FUN_004292d0
void FUN_004292d0(int param_1, void *param_2)
{
  memcpy(param_2, (char *)param_1 + 8, FUN_0055deb0(param_1));
}
