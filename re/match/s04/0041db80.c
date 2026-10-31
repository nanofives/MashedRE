// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041db80: 100% (regs 100%) by hand. The original is `mov eax, 0x63d55c; jmp 0x41dab0`, a tail call
   into a function that takes its record pointer in EAX. 0x0041dab0 has no Ghidra function entry, so this file
   holds it as a `static` callee defined before its caller (pilot finding 1: a static, address-not-taken callee
   under /Ob1 gets a register convention, first parameter in EAX). It carries a HIDDEN marker with its size
   (0x0041dab0..0x0041db79, 202 B); checked byte-identical with match.py's score_function/byte_compare at that
   size (100%, 0 differing bytes). The record at 0x0063d55c: six object pointers at +0x00..+0x14, float at +0x2c
   (= DAT_0063d588); each call is the function pointer at +0x48 of the object, called with the object.
   Constants: 0x005cc35c 4.0, 0x005cd238 0.34, 0x005cd234 0.82, 0x005cd230 1.3, 0x005ccdf4 1.86.
   Written first time from the disassembly. Residual: none. */
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
typedef void (*MatchFn0041dab0)(char *);
typedef struct {
  char *m_p[6];      /* +0x00..+0x14 */
  char pad18[0x14];
  float m_f;         /* +0x2c (0x0063d588) */
} MatchRec0041dab0;
MGLOBAL(float, DAT_005cc35c, 0x005cc35c);
MGLOBAL(float, DAT_005cd238, 0x005cd238);
MGLOBAL(float, DAT_005cd234, 0x005cd234);
MGLOBAL(float, DAT_005cd230, 0x005cd230);
MGLOBAL(float, DAT_005ccdf4, 0x005ccdf4);
MGLOBAL(MatchRec0041dab0, DAT_0063d55c, 0x0063d55c);

#define CALL48(p) (*(MatchFn0041dab0 *)((p) + 0x48))(p)

// HIDDEN: 0x0041dab0 _FUN_0041dab0 202
static void FUN_0041dab0(MatchRec0041dab0 *r)
{
  if (r->m_f < DAT_005cc35c) {
    CALL48(r->m_p[0]);
    CALL48(r->m_p[5]);
  }
  if (r->m_f < DAT_005cd238)
    return;
  if (r->m_f < DAT_005cd234) {
    CALL48(r->m_p[1]);
  } else if (r->m_f < DAT_005cd230) {
    CALL48(r->m_p[1]);
    CALL48(r->m_p[2]);
  } else if (r->m_f < DAT_005ccdf4) {
    CALL48(r->m_p[1]);
    CALL48(r->m_p[2]);
    CALL48(r->m_p[3]);
  }
  if (r->m_f >= DAT_005ccdf4 && r->m_f < DAT_005cc35c)
    CALL48(r->m_p[4]);
}

// MATCH: 0x0041db80 FUN_0041db80
void FUN_0041db80(void)
{
  FUN_0041dab0(&DAT_0063d55c);
}
