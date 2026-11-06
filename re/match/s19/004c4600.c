// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c4600 (h5): 100% (regs 100%). RwMatrixMultiply: identity-flag shortcuts (struct copy = rep movsd),
   else the RwGlobals-module multiply function pointer at +8; result flags = flag1 & flag2.
   FLAGS: /G5 added (h5 finding, 2026-11-06): the RW core in this slice is a Pentium (/G3-/G5) build. Under the
   game flags /O2 /GS /Ob1 alone (= /G6 scheduling and no byte-register ALU ops) this file scores  89.8%: prologue load scheduling and the
   return `mov eax,esi` after `pop edi` (forms + permuter did not move them). */
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
typedef struct RwV3d { float x; float y; float z; } RwV3d;
typedef struct RwMatrix { RwV3d right; unsigned int flags; RwV3d up; unsigned int pad1; RwV3d at; unsigned int pad2; RwV3d pos; unsigned int pad3; } RwMatrix;
typedef void (*fp_mtxmul)(RwMatrix *, const RwMatrix *, const RwMatrix *);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4028, 0x007d4028);

// MATCH: 0x004c4600 FUN_004c4600
RwMatrix *FUN_004c4600(RwMatrix *matrixOut, const RwMatrix *matrixIn1, const RwMatrix *matrixIn2)
{
  unsigned int flag1 = matrixIn1->flags;
  unsigned int flag2 = matrixIn2->flags;
  unsigned int ident = *(unsigned int *)(DAT_007d3ff8 + DAT_007d4028 + 4) & 0x20000;
  if (flag1 & ident) {
    *matrixOut = *matrixIn2;
    return matrixOut;
  }
  if (flag2 & ident) {
    *matrixOut = *matrixIn1;
    return matrixOut;
  }
  (*(fp_mtxmul *)(DAT_007d3ff8 + DAT_007d4028 + 8))(matrixOut, matrixIn1, matrixIn2);
  matrixOut->flags = flag1 & flag2;
  return matrixOut;
}
