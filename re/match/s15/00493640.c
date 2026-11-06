// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00493640 (h8): best 67.9% (regs 67.9%, ops 94.6%). Instruction stream identical except block
   placement: the original puts the single `xor eax,eax; ret` right after the first test (jne over it, every later
   je jumps back to 0x00493649); ours puts it at the end. Int return (bool gave xor al,al: raw 32%). Tried: one
   || condition, nested ifs (Ghidra form), separate if/return per call, goto fail, && expression with comma,
   result variable, first call split out, permuter (no gain). Residual (b): source form that places the fail
   block first not found. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
int FUN_004e5d30(void);
int FUN_00543e50(void);
int FUN_0053eaa0(void);
int FUN_0053d0b0(void);
int FUN_00538600(void);
int FUN_00534a80(void);
int FUN_00534920(void);
int FUN_00546530(void);
int FUN_005336d0(void);
int FUN_0052e310(void);
int FUN_00544d20(void);
int FUN_00549640(void);
int FUN_005578a0(void);
int FUN_005515a0(void);
int FUN_0057c270(void);
int FUN_00561ee0(void);
int FUN_00472380(void);
void FUN_0052d8e0(void);
// MATCH: 0x00493640 FUN_00493640
int FUN_00493640(void)
{
  if (FUN_004e5d30() == 0 ||
      FUN_00543e50() == 0 ||
      FUN_0053eaa0() == 0 ||
      FUN_0053d0b0() == 0 ||
      FUN_00538600() == 0 ||
      FUN_00534a80() == 0 ||
      FUN_00534920() == 0 ||
      FUN_00546530() == 0 ||
      FUN_005336d0() == 0 ||
      FUN_0052e310() == 0 ||
      FUN_00544d20() == 0 ||
      FUN_00549640() == 0 ||
      FUN_005578a0() == 0 ||
      FUN_005515a0() == 0) {
    return 0;
  }
  FUN_0052d8e0();
  if (FUN_0057c270() == 0 || FUN_00561ee0() == 0) {
    return 0;
  }
  return FUN_00472380() != 0;
}
