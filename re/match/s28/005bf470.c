// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005bf470: 100% (regs 100%). Ghidra's stack-pointer form rewritten as QueryInterface + vtbl+0x20 call + Release on a 0x10c-byte frame struct (obj, out, 260-byte pad). Class: none. */
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
typedef int (__stdcall *qi_fn)(int *, void *, void *);
typedef int (__stdcall *m_fn)(int *, void *);
typedef int (__stdcall *rel_fn)(int *);
typedef struct { int *obj; int out; char pad[0x104]; } frame_t;
MGLOBAL(undefined4, DAT_005d0bac, 0x005d0bac);
// MATCH: 0x005bf470 FUN_005bf470
int FUN_005bf470(int *param_1, int *param_2)
{
  frame_t f;
  int hr;

  hr = ((qi_fn)*(int *)*param_1)(param_1, &DAT_005d0bac, &f.obj);
  if (hr >= 0) {
    hr = ((m_fn)*(int *)(*f.obj + 0x20))(f.obj, &f.out);
    ((rel_fn)*(int *)(*f.obj + 8))(f.obj);
    if (hr < 0) {
      return hr;
    }
    *param_2 = f.out;
    hr = 0;
  }
  return hr;
}
