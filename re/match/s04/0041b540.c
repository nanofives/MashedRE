// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041b540: 100% by hand (regs 100%). Raw converter 61.4%. What moved it: the two flag checks are a
   `for (i < 4)` loop over the int arrays at 0x0063c8b4 / 0x0063c88c (the compiler unrolls it into the 8
   compare/clear pairs), flags preset to 1 instead of `!= 0`, and an index loop over the 4 records at 0x0063c8d0.
   0x0041af50 (998 B, not in this slice) takes the record in EAX (pilot finding 1), so the file has a `static`
   stand-in `FUN_0041af50` defined first that forwards to `FUN_0041af50_body` (the real 0x0041af50 body, to be
   supplied by the port). The stand-in is NOT the real callee (no MATCH line). It clobbers only eax/ecx/edx, the
   same as any body would, so the caller's bytes do not depend on it. Residual: none. */
/* MATCH-S04 0x0041b540: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs04_20261031/results.tsv. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef struct Rec0041b540 { char pad0[0x6c]; int m6c; char pad70[0x74 - 0x70]; } Rec0041b540;
MGLOBALA(int, DAT_0063c878, 5, 0x0063c878);
MGLOBALA(int, DAT_0063c88c, 4, 0x0063c88c);
MGLOBAL(int, DAT_0063c8a0, 0x0063c8a0);
MGLOBALA(int, DAT_0063c8a4, 4, 0x0063c8a4);
MGLOBALA(int, DAT_0063c8b4, 4, 0x0063c8b4);
MGLOBAL(int, DAT_0063c8c8, 0x0063c8c8);
MGLOBALA(Rec0041b540, DAT_0063c8d0, 4, 0x0063c8d0);
MGLOBALA(int, DAT_0063caa0, 4, 0x0063caa0);
MGLOBAL(int, DAT_0063cab0, 0x0063cab0);
void FUN_0040b930(int *p0);
void FUN_0040b970(int *p0);
void FUN_0040b9a0(int *p0, int p1);
void FUN_0040ba00(int *p0);
void FUN_0040ba60(int *p0);
void FUN_0041af50_body(Rec0041b540 *r);
static void FUN_0041af50(Rec0041b540 *r)
{
  FUN_0041af50_body(r);
}
// MATCH: 0x0041b540 FUN_0041b540
void FUN_0041b540(void)
{
  int i;
  FUN_0040b930(DAT_0063c8a4);
  FUN_0040b9a0(DAT_0063c8b4, 1);
  FUN_0040ba00(DAT_0063c88c);
  FUN_0040b970(DAT_0063c878);
  FUN_0040ba60(DAT_0063caa0);
  DAT_0063c8c8 = 1;
  DAT_0063c8a0 = 1;
  for (i = 0; i < 4; i++) {
    if (DAT_0063c8b4[i] == 0)
      DAT_0063c8c8 = 0;
    if (DAT_0063c88c[i] == 0)
      DAT_0063c8a0 = 0;
  }
  for (i = 0; i < 4; i++) {
    if (DAT_0063c8d0[i].m6c != 0)
      FUN_0041af50(&DAT_0063c8d0[i]);
  }
  DAT_0063cab0++;
}
