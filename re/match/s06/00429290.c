// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00429290 (40 B) + its static callee 0x00429240 (51 B), held in this file: both 100.0% exact
   (regs 100, ops 100), cl 13.00.9176. Raw g2s was 70.0% / 47.4%.
   - 0x00429240 takes (a in EAX, b in ECX): it is `static`, defined before its only caller (call site 0x004292a4,
     the only one in .text), /Ob1 keeps it out of line. Parameter order (a, b) gives EAX = arg2, ECX = arg1 as the
     caller loads them at 0x0042929c / 0x004292a0.
   - Jump table 0..4 (table at 0x00429274: cases 2 and 4 -> ret 0x00429272) needs explicit `case 2: return;` and
     `case 4: return;`; `break` folds them into the default and gives a sub/dec chain.
   - DAT_0067d960 must be a file-`static` (pilot finding 2): extern, the switch value takes EAX and the arguments
     move to ECX/EDX (63.2%). Static, the switch is in EDX as at 0x00429240. Its 3 .text references (0x00428395,
     0x00428b03, 0x00429242) all sit in 0x00428390..0x00429290. C: static under #ifndef __cplusplus, MGLOBAL in C++.
   - FUN_0045d3f0(1) and FUN_00466b50(1) are cdecl, one arg each (deferred `add esp, 8` at 0x004292b0).
   Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
#ifndef __cplusplus
static int DAT_0067d960;
#else
MGLOBAL(int, DAT_0067d960, 0x0067d960);
#endif
void FUN_0045d3f0(int p0);
void FUN_0045d430(void);
void FUN_00466b50(int p0);
void FUN_004967e0(void);
void FUN_00428a30(int a, int b);
void FUN_00428bf0(int a, int b);
void FUN_00428d30(int a, int b);

// MATCH: 0x00429240 FUN_00429240
static void FUN_00429240(int a, int b)
{
  switch (DAT_0067d960) {
  case 0:
    FUN_00428a30(a, b);
    return;
  case 1:
    FUN_00428bf0(a, b);
    return;
  case 2:
    return;
  case 3:
    FUN_00428d30(a, b);
    return;
  case 4:
    return;
  }
}

// MATCH: 0x00429290 FUN_00429290
void FUN_00429290(int a, int b)
{
  FUN_0045d3f0(1);
  FUN_004967e0();
  FUN_00429240(a, b);
  FUN_00466b50(1);
  FUN_0045d430();
}
