// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c3ac0 (h3): best 75.0% (regs 86.1%, ops 91.7%). RwV3dLength: dot then RW table sqrt (RwGlobals+DAT_007d3ffc table) through a
   RwSplitBits union (volatile int members). Residual: x87 pop placement around `test eax,eax` and the scheduling
   of the two RwGlobals loads against the `+= 0x800` (eax/ecx/edx rotate). Tried idx/tab/inline-expression forms,
   split add, permuter (109). Class (b). */
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
typedef struct RwV3d { float x, y, z; } RwV3d;
typedef union RwSplitBits { float nReal; volatile int nInt; volatile unsigned int nUInt; } RwSplitBits;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d3ffc, 0x007d3ffc);

// MATCH: 0x004c3ac0 FUN_004c3ac0
float FUN_004c3ac0(const RwV3d *in)
{
  RwSplitBits s;
  s.nReal = in->z * in->z + (in->x * in->x + in->y * in->y);
  if (s.nUInt) {
    const volatile unsigned int *tab = *(volatile unsigned int **)(DAT_007d3ff8 + DAT_007d3ffc);
    s.nUInt += 0x800;
    s.nUInt = tab[(s.nUInt >> 12) & 0xfff] + ((s.nUInt >> 1) & 0x3fc00000);
  }
  return s.nReal;
}
