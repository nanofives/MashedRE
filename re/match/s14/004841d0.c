// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 (h1) 0x004841d0: 75.0% (regs 92.9%). File also holds the static register-arg callee
   0x004840f0 (66.7%, regs 71.4%): EAX = glyph record, EDX = char code; its only caller is 0x004841d0.
   Residual 0x004840f0, class (b): our cl passes the index in ECX and addresses [ecx*4 + table] twice;
   the original materializes `lea ecx, [edx*4 + 0x6ce840]` (count pointer) and takes the index in EDX.
   Tried: explicit count pointer (before / after the pointer store), direct indexing, for/while/do,
   `||` vs `|` tests, 4-char struct records, `register`, one struct holding both tables, param order
   (EAX always the first used), char/short/uint index, a dummy middle param (goes to the stack), permuter.
   Residual 0x004841d0, class (b): `lea ecx` vs `lea edx` follows the callee's convention above; the
   letters loop keeps letter/pointer in ESI/EDI (original EDI/ESI); `mov eax,1; pop ebx; ret` lies past
   Ghidra's size. Tried: letter-indexed loop, pointer walk, int counter (worse), permuter. */
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
static void FUN_004840f0(char *p, int idx)
{
  int *cnt;
  DAT_006cec40[idx] = p;
  cnt = &DAT_006ce840[idx];
  *cnt = 0;
  while ((p[3] | p[2] | p[1] | p[0]) != 0) {
    (*cnt)++;
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
    FUN_004840f0(DAT_00613328[u], u + 0x21);
  }
  for (u = 0; u < 10; u++) {
    FUN_004840f0(DAT_006138a8[u], u + 0x30);
  }
  for (u = 0; u < 7; u++) {
    FUN_004840f0(DAT_006136e8[u], u + 0x3a);
  }
  for (u = 0; u < 26; u++) {
    FUN_004840f0(DAT_00613b28[u], u + 0x61);
    FUN_004840f0(DAT_00613b28[u], u + 0x41);
  }
  return 1;
}
