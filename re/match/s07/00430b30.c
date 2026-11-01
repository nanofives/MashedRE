// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x00430b30 (h9): 100% (regs 100%). Residual: none. Raw g2s did not compile (__thiscall + in_EAX). The original takes its first two
   arguments in EAX and ECX (callers 0x004344f7 / 0x00435363: xor eax,eax / mov ecx,1 or the reverse) plus three
   stack pointers, plain ret (caller cleans). Pilot finding 1: a static function, defined before its caller,
   address not taken, /Ob1. Here the FIRST parameter (b, row) lands in ECX and the second (a, column) in EAX;
   (a, b) order gave lea eax,[ecx+eax*2] (61.5%). Separate DAT[b][a].fN reads (no record pointer) give the shl/disp form. The file therefore holds a stand-in caller (not a MATCH,
   not the real TU: the real callers sit at 0x004344f7..0x0043571b). Record table 0x008989e0: 12-byte records
   indexed a + b*2. [UNCERTAIN] extent: 2x2 records declared, the next referenced global is 0x00898a20. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
#ifdef __cplusplus
#define MATCH_STATIC
#else
#define MATCH_STATIC static
#endif
typedef struct { float f0, f4, f8; } Rec_008989e0;
typedef Rec_008989e0 Row_008989e0[2];
MGLOBALA(Row_008989e0, DAT_008989e0, 2, 0x008989e0);
// MATCH: 0x00430b30 FUN_00430b30
MATCH_STATIC void FUN_00430b30(int b, int a, float *p0, float *p4, float *p8)
{
  *p0 = DAT_008989e0[b][a].f0;
  *p8 = DAT_008989e0[b][a].f8;
  *p4 = DAT_008989e0[b][a].f4;
}

#ifndef __cplusplus
/* stand-in caller (gives FUN_00430b30 its register convention); not a game function */
void standin_00430b30(float *x, float *y, float *z)
{
  FUN_00430b30(0, 1, x, y, z);
}
#endif
