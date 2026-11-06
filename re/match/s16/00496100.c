// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00496100 (h2): 100% (regs 100%). Hand-written: the 8-way body Ghidra shows is the compiler unrolling one plain 256-iteration loop; keys[i] & 0x80 (jns), DirectInput GetDeviceState (+0x24) / Acquire (+0x1c) retry as nested early returns. */
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
typedef long (__stdcall *AcquireFn)(int *self);
typedef long (__stdcall *GetDeviceStateFn)(int *self, unsigned long n, void *data);
MGLOBAL(int *, DAT_00772fb8, 0x00772fb8);
void FUN_004b6480(unsigned char *p0, unsigned int p1);
void FUN_004b6520(char *p0, unsigned int p1);
// MATCH: 0x00496100 FUN_00496100
int FUN_00496100(unsigned char *param_1)
{
  int i;
  char keys[256];

  FUN_004b6480(param_1, 0x100);
  FUN_004b6520(keys, 0x100);
  if ((*(GetDeviceStateFn *)(*DAT_00772fb8 + 0x24))(DAT_00772fb8, 0x100, keys) < 0) {
    if ((*(AcquireFn *)(*DAT_00772fb8 + 0x1c))(DAT_00772fb8) < 0) {
      return 1;
    }
    if ((*(GetDeviceStateFn *)(*DAT_00772fb8 + 0x24))(DAT_00772fb8, 0x100, keys) < 0) {
      return 1;
    }
  }
  for (i = 0; i < 0x100; i++) {
    if (keys[i] & 0x80) {
      param_1[i >> 3] |= (unsigned char)(1 << (i & 7));
    }
  }
  return 1;
}
