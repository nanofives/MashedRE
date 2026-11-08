// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00555f20 (h4): 96.1% (regs 96.1%). Builds a short lookup map over the positive entries of the
   int table at 0x0091325c (count at param_1+0x130): min/max scan, RwEngine malloc (slot +0x108, hint 0x30190),
   fill with -1, store index per value, write n/min/map at +0x128/+0x124/+0x12c. The first-loop count must be a
   local (96.1% vs 60.8%). Residual (a): n = max - min + 1 comes out as `inc ecx; mov ebx, ecx` with the RwEngine
   load after the sub; the original has `lea ebx, [ecx + 1]` with the load first. Tried: 5 spellings of n,
   separate range variable, n + n and sizeof(short) * n sizes, unsigned n (94.8), alloc before n (60.5),
   RwGlobals struct member and fn-pointer local (82.1), flags /G3 /G4 (same), /O2 /Ob1, /GS (90.9), /O1 (27.6),
   permuter 151 compiles no gain. */
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
typedef void *(*MallocFn)(unsigned int size, unsigned int hint);
MGLOBAL(int *, DAT_0091325c, 0x0091325c);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x00555f20 FUN_00555f20
int FUN_00555f20(int param_1)
{
  int maxv = 0;
  int found = 0;
  int i;
  int minv = 0xffff;
  int n;
  short *map;
  int count = *(int *)(param_1 + 0x130);

  for (i = 0; i < count; i++) {
    int v = DAT_0091325c[i];
    if (v > 0) {
      if (v < minv) minv = v;
      if (v > maxv) maxv = v;
      found = 1;
    }
  }
  if (found > 0) {
    n = maxv - minv + 1;
    map = (short *)(*(MallocFn *)(DAT_007d3ff8 + 0x108))(n * 2, 0x30190);
    if (map == 0) return 0;
    for (i = 0; i < n; i++) map[i] = -1;
    for (i = 0; i < *(int *)(param_1 + 0x130); i++) {
      int v = DAT_0091325c[i];
      if (v > 0) map[v - minv] = (short)i;
    }
    *(int *)(param_1 + 0x128) = n;
    *(int *)(param_1 + 0x124) = minv;
    *(short **)(param_1 + 0x12c) = map;
  }
  return param_1;
}
