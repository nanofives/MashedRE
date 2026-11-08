// FLAGS: /O2 /GS /Ob1
/* MATCH-TOPM01 0x004841d0: BYTE-IDENTICAL (match.py --bytes: 165 B whole symbol, 0 differing unmasked bytes);
   insn score 94.6% (56/53) only because Ghidra's size (159 B, pc_funcs.csv) stops before the
   `mov eax,1 / pop ebx / ret` tail at 0x0048426e..0x00484273. Static register-arg callee 0x004840f0: 100.0%
   (21/21), BYTE-IDENTICAL (59 B). Started from re/match/s14/004841d0.c (75.0%, callee 66.7%).
   What closed it:
   1. Callee: `DAT_006ce840[idx] = 0;` written BEFORE `DAT_006cec40[idx] = p;` and the loop increments
      `DAT_006ce840[idx]++` (no explicit count pointer). cl then hoists `lea ecx,[edx*4+0x6ce840]` to the entry
      (0x004840f0) and takes idx in EDX, as in the original. Every explicit `int *cnt` form folds back to
      [ecx*4+disp] with idx in ECX.
   2. Callee parameter order (int idx, char *p): right-to-left argument evaluation then gives
      `mov eax,esi / mov edx,edi` at 0x0048425c (with (char *p, int idx) it is `mov edx,edi / mov eax,esi`).
      Registers are unchanged: EAX = p, EDX = idx.
   3. Letters loop as an unsigned 0..25 counter with the glyph row indexed by it: cl strength-reduces to
      EDI = letter (0x41+u), ESI = row pointer, EBX = down-counter 0x1a (0x00484243..0x0048426a). The s14 form
      (two calls written with `DAT_00613b28[u]` and (p, idx) order) gave the swapped ESI/EDI roles.
   Tried and worse: letter-indexed loops (signed: pointer compare `cmp esi,0x680; jl`, no EBX counter;
      unsigned: ESI/EDI swapped), pointer walks (Glyph * / char *, 74.1%), explicit down-counter loops,
      `c | 0x20`, `__inline` pair helpers, declaration order, LANG c++ (preamble references do not compile),
      flag sets (/O2, /Ox, /O1, /G5, /G6, /Ob2, /Og*: none better than /O2 /GS /Ob1). */
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
MGLOBALA(int, DAT_006ce840, 0x100, 0x006ce840);
MGLOBALA(char *, DAT_006cec40, 0x100, 0x006cec40);
typedef char Glyph[0x40];
MGLOBALA(Glyph, DAT_00613328, 15, 0x00613328);
MGLOBALA(Glyph, DAT_006136e8, 7, 0x006136e8);
MGLOBALA(Glyph, DAT_006138a8, 10, 0x006138a8);
MGLOBALA(Glyph, DAT_00613b28, 26, 0x00613b28);

// MATCH: 0x004840f0 FUN_004840f0
static void FUN_004840f0(int idx, char *p)
{
  DAT_006ce840[idx] = 0;
  DAT_006cec40[idx] = p;
  while ((p[3] | p[2] | p[1] | p[0]) != 0) {
    DAT_006ce840[idx]++;
    p += 4;
  }
}

// MATCH: 0x004841d0 FUN_004841d0
int FUN_004841d0(void)
{
  int i;
  unsigned int u;
  for (i = 0; i < 0x100; i++) {
    DAT_006ce840[i] = 0;
  }
  for (u = 0; u < 15; u++) {
    FUN_004840f0(u + 0x21, DAT_00613328[u]);
  }
  for (u = 0; u < 10; u++) {
    FUN_004840f0(u + 0x30, DAT_006138a8[u]);
  }
  for (u = 0; u < 7; u++) {
    FUN_004840f0(u + 0x3a, DAT_006136e8[u]);
  }
  for (u = 0; u < 26; u++) {
    FUN_004840f0(u + 0x61, DAT_00613b28[u]);
    FUN_004840f0(u + 0x41, DAT_00613b28[u]);
  }
  return 1;
}
