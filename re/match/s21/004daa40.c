// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004daa40 (h4): 100% (regs 100%). Rewritten from odis: unsigned loops with plain int shifts (the raw '& 0x1f' masks gave and ecx), ternary (i & 1<<j) ? 1 << (bits-1-j)*4 : 0, typed pointers. Table DAT_007d6c98 holds 1 << DAT_00618430 entries. The array length 16 in MGLOBALA is a placeholder: the code does not bound it [UNCERTAIN]. Residual: none. */
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
typedef void *(*AllocFn)(void *freeList, uint hint);
MGLOBAL(uint, DAT_00618430, 0x00618430);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(uint, DAT_007d6c98, 16, 0x007d6c98);
void *FUN_004cc7f0(int p0, int p1, int p2, int p3);
// MATCH: 0x004daa40 FUN_004daa40
int FUN_004daa40(char *param_1)
{
  uint bits = DAT_00618430;
  uint n = 1 << bits;
  uint i;
  void *fl;
  char *root;
  for (i = 0; i < n; i++) {
    uint v = 0;
    uint j;
    for (j = 0; j < bits; j++) {
      v |= (i & (1 << j)) ? (1 << ((bits - 1 - j) * 4)) : 0;
    }
    DAT_007d6c98[i] = v;
  }
  fl = FUN_004cc7f0(0x5c, 0x400, 4, 0x30411);
  *(void **)(param_1 + 0x4004) = fl;
  root = (char *)(*(AllocFn *)(DAT_007d3ff8 + 0x118))(fl, 0x30411);
  *(char **)(param_1 + 0x4000) = root;
  {
    int k;
    for (k = 0; k < 16; k++) ((int *)(root + 0x1c))[k] = 0;
  }
  return 1;
}
