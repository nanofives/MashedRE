// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041ded0 (30 B) and 0x0041de80 (68 B): this file holds both. 0x0041de80 is the only callee of
   0x0041ded0 (call site 0x0041dee4, no other reference in .text) and takes the record 0x0063d5a0 in ESI
   (Ghidra unaff_ESI), so it is written as a `static` callee defined before its caller (pilot finding 1).
   Best: 0x0041de80 65.5% (regs 69.0%), 0x0041ded0 27.3% (regs 36.4%). Raw: 71.4% / 27.3%.
   Residual (b) register convention: cl 9176 makes the record arrive in EAX and copies it to ESI under
   push/pop esi, and then reuses ESI for the last object, so the final call is a plain call and
   0x0041ded0 tail-jumps with eax = 0x0063d5a0. The original takes the record in ESI unsaved (the caller saves
   ESI), never writes ESI, and ends with a tail jump (`mov [esp+4], eax; jmp [eax+0x48]`). The same compiler
   does give an unsaved callee-saved parameter register for 0x0041dc30 (EDI, file 0x0041dca0), which never
   writes that register. Tried with no effect: parameter order (a, r) / (r, a), int* record, `register`,
   const pointer, three dropped leading params, int return on either side, `if (!a) return;`, a local for
   the last object, a typed fn-pointer struct, callee defined after the caller, /O1 /Ox /Ob0 /Ob2 /Oy- /G5
   /G6, the permuter (106 compiles, no move). The record (0x54 B, cleared by 0x0041db90): 16 object
   pointers, +0x48 flag (0x0063d5e8), +0x4c float, +0x50 index; each call is the function pointer at +0x48
   of the object, called with the object. */
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
typedef void (*MatchFn0041de80)(char *);
typedef struct {
  char *m_p[19];     /* +0x00..+0x48 */
  float m_4c;        /* +0x4c */
  int m_50;          /* +0x50 */
} MatchRec0041de80;
MGLOBAL(int, DAT_007f1a1c, 0x007f1a1c);
MGLOBAL(int, DAT_0063d5e8, 0x0063d5e8);
MGLOBAL(MatchRec0041de80, DAT_0063d5a0, 0x0063d5a0);

#define CALL48(p) (*(MatchFn0041de80 *)((p) + 0x48))(p)

// MATCH: 0x0041de80 FUN_0041de80
static void FUN_0041de80(MatchRec0041de80 *r, int a)
{
  CALL48(r->m_p[0xd]);
  CALL48(r->m_p[2]);
  CALL48(r->m_p[0]);
  CALL48(r->m_p[1]);
  CALL48(r->m_p[DAT_007f1a1c + 7]);
  if (a != 0)
    CALL48(r->m_p[r->m_50 + 2]);
}

// MATCH: 0x0041ded0 FUN_0041ded0
void FUN_0041ded0(int a)
{
  if (DAT_0063d5e8 != 0)
    FUN_0041de80(&DAT_0063d5a0, a);
}
