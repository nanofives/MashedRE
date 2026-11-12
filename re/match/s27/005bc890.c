// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005bc890: 72.0% (regs 78%). Struct copy + intrinsic memcpy give the rep movs. Residual: original pushes ebx/ebp/esi/edi before the first call (no shrink-wrap), ours pushes them after the null check; layout of the free path otherwise equal. Flags /O2 /GS /O1 /Oy- tried. Class: a.
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
typedef void *LPVOID;
typedef struct S48 { int a[16]; uint size; void *data; } S48;
__declspec(dllimport) LPVOID __stdcall CoTaskMemAlloc(unsigned int p0);
__declspec(dllimport) void __stdcall CoTaskMemFree(LPVOID p0);
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
// MATCH: 0x005bc890 FUN_005bc890
S48 * FUN_005bc890(S48 *src)
{
  S48 *pv;
  pv = (S48 *)CoTaskMemAlloc(0x48);
  if (pv == 0) {
    return 0;
  }
  *pv = *src;
  if (src->size != 0) {
    pv->data = CoTaskMemAlloc(src->size);
    if (src->data != 0) {
      memcpy(pv->data, src->data, src->size);
    } else {
      CoTaskMemFree(pv);
      return 0;
    }
  }
  return pv;
}
