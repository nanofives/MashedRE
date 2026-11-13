// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d7100 (h3): best 21.4% (regs 71.4%, ops 96.4%). Raw 11.7%. Form: a render-state cache of {value, dirty}
   pairs at 0x007d57f8 (state 0x1b at 0x007d58d0, state 0xf at 0x007d5870) with a dirty queue at 0x007d5168 /
   count 0x007d6c14, written as two macros (set; set-if-changed). Same instruction sequence as the original;
   residual: the original holds the constant 1 in eax and the loaded globals in ecx, ours swaps them throughout.
   Tried branch order, early returns, a normalised flag local, permuter 240 s (93 compiles, 0 gains).
   Class (b): register allocation that moves only with a source form not found.
   RWC-FIRST-FRAME-IM2D (2026-11-12): 100%, relocheck OK (27/0). The function returns int 1 on every path: mov eax, 1
   at 0x004d710e, eax not written again before the ret at 0x004d71ea (the caller 0x004d7480 case 0xc returns it as
   RenderStateSet's result). The "constant 1 in eax" residual above was this return value. The void form left the
   address 0x007d6b14 in eax on the RW core (verify/rwcim2d_20261113). */
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
typedef struct Rs7100 { int value; int dirty; } Rs7100;
MGLOBAL(int, DAT_007d6b10, 0x007d6b10);
MGLOBAL(int, DAT_007d6b14, 0x007d6b14);
MGLOBAL(int, DAT_007d6bfc, 0x007d6bfc);
MGLOBAL(int, DAT_007d6c14, 0x007d6c14);
MGLOBALA(Rs7100, DAT_007d57f8, 210, 0x007d57f8);
MGLOBALA(int, DAT_007d5168, 210, 0x007d5168);
#define RS7100_SET(s, v) { DAT_007d57f8[s].value = (v); if (!DAT_007d57f8[s].dirty) { DAT_007d57f8[s].dirty = 1; DAT_007d5168[DAT_007d6c14++] = (s); } }
#define RS7100_CHG(s, v) { if (DAT_007d57f8[s].value != (v)) { DAT_007d57f8[s].value = (v); if (!DAT_007d57f8[s].dirty) { DAT_007d57f8[s].dirty = 1; DAT_007d5168[DAT_007d6c14++] = (s); } } }
// MATCH: 0x004d7100 FUN_004d7100
int FUN_004d7100(int param_1)
{
  if (param_1 != 0) {
    if (DAT_007d6b10 == 0) {
      DAT_007d6b10 = 1;
      if (DAT_007d6b14 == 0) {
        RS7100_SET(0x1b, 1);
        RS7100_CHG(0xf, DAT_007d6bfc);
      }
    }
  } else {
    if (DAT_007d6b10 != 0) {
      DAT_007d6b10 = 0;
      if (DAT_007d6b14 == 0) {
        RS7100_SET(0x1b, 0);
        RS7100_CHG(0xf, 0);
      }
    }
  }
  return 1;
}
