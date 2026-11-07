// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00554050 (h2): 100% (regs 100%), byte-identical. FUN_005c4d50 takes one argument (it reads
   [esp+8] after one push; Ghidra listed none), which lets both branches share the push of p[0] and one
   add esp,4. The raw converter had it as (void) and read DAT_00912a28 three times (42.9%). */
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
typedef void (*rwfn1_t)(int);
typedef void (*rwfn2_t)(int, int *);
MGLOBAL(int *, DAT_00912a28, 0x00912a28);
MGLOBAL(int, DAT_00912a24, 0x00912a24);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_004cc9f0(int p0);
int FUN_00553e80(int p0);
int FUN_005c4d50(int p0);
// MATCH: 0x00554050 FUN_00554050
void FUN_00554050(void)
{
  int *p = DAT_00912a28;

  if (p != 0) {
    int next = p[5];
    if (p[0] != 0) {
      if (p[1] == -1) {
        FUN_005c4d50(p[0]);
      }
      else {
        (*(rwfn1_t *)(DAT_007d3ff8 + 0x10c))(p[0]);
      }
    }
    p[0] = 0;
    (*(rwfn2_t *)(DAT_007d3ff8 + 0x11c))(DAT_00912a24, p);
    FUN_00553e80(next);
  }
  FUN_004cc9f0(DAT_00912a24);
  DAT_00912a24 = 0;
}
