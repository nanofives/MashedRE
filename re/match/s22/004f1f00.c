// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f1f00 (h2): 100% (regs 100%), relocheck OK, cxx ok. Hand pass from raw 70%: one result variable
   initialised to 0 and a single return at the end (an early return let the compiler fold the 0 into xor eax,eax),
   vtable slot 0x10 is a __stdcall (obj, buf[160], &count). Residual: none. */
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
typedef void (__stdcall *VtFn10)(int *self, void *buf, int *count);
int FUN_004e55d0(int p0);
// MATCH: 0x004f1f00 FUN_004f1f00
int FUN_004f1f00(int param_1)
{
  int size;
  int obj;
  int *p;
  int i;
  int count;
  unsigned char buf[160];

  size = 0;
  if (*(int *)(param_1 + 0x34) != 0) {
    if ((*(uint *)(FUN_004e55d0(param_1) + 8) & 0x1000000) != 0) {
      obj = *(int *)(param_1 + 0x34);
      size = *(int *)(obj + 8) + 0x14;
      ((VtFn10)(**(int ***)(obj + 0x4c))[4])(*(int **)(obj + 0x4c), buf, &count);
      size = size + 4 + (*(int *)(obj + 0x50) + count * 4) * 2;
      p = (int *)(obj + 0x30);
      for (i = 2; i != 0; i--) {
        size += 0x10;
        if (p[-2] != 0) {
          size += *(int *)(obj + 0x54) * *p;
        }
        p += 4;
      }
    }
  }
  return size;
}
