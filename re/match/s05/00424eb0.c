// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00424eb0 (1039 B): best 98.6% (regs 100%, ops 100%), 347/347 instructions. By hand (h8), raw converter
   65.2%. The file also holds a static copy of 0x00422fd0 with an extra unused first parameter (the loop-2 count).
   That is what makes the caller load the count before `push esi; call 0x00422fd0` (the original loads it into EAX).
   This copy is NOT the real callee shape: every other caller of 0x00422fd0 pushes one argument and 0x00422fd0 reads it
   from the stack, so the real function is a plain 1-arg cdecl (re/match/s05/00422fd0.c, 100%), so the copy carries no MATCH line. Here the compiler
   passes the index in ESI and the count on the stack (callee 83.3%), so the caller differs only by register names:
   `mov edx,[esp+0x18]; push edx` for `mov eax,[esp+0x18]; push esi`, and the sete register at the 0x0040eee0 call.
   With a plain extern 1-arg FUN_00422fd0 the count is dead and the caller drops to 79.8% (frame 0x34). FUN_004a2c48 is
   the CRT __ftol (the (int) cast). Residual (b): the source form that leaves the dead count load in EAX was not found
   (tried both param orders, void/int return, sete spelling).
   PROV-DIVERGED-18 (2026-11-11): named FUN_00422fd0, the stand-in was a WRONG BODY when linked against the original:
   provlink (and the object build) bind that name to the original 0x00422fd0, which reads its one argument from
   [esp+8] (0x00422fd1), and this call site pushes the count (the original pushes the index, 0x00425279 push esi).
   Replay DIVERGED@1108. Two fixes were replayed (verify/provinsitu_20261111/RESULT_PROV-DIVERGED-18.md):
   (1) the real 1-arg extern: 79.8%, frame 0x34, DIVERGED@151 (a callee reads the caller's frame: 0x0046c7d0 has no
   equiv summary and the frame slots move by 4); (2) this form: the stand-in renamed s_StandIn_00422fd0 so it is
   linked as this file's own static helper and runs on the index, frame 0x38 as the original: 98.6%,
   IDENTICAL-FOR-2724. The stand-in is a private copy of 0x00422fd0 (83.3%), not a binding of the original. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct V3_00424eb0 {
  float x, y, z;
} V3_00424eb0;
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBALA(char, DAT_00803324, 4, 0x00803324);
int FUN_0040e340(void);
int FUN_0040e350(void);
int FUN_0040e370(int p0);
int FUN_0046c7b0(int p0);
void FUN_0046c570(int p0);
void FUN_0046c7d0(int p0, int p1);
void FUN_0046cbb0(int p0, int *p1, int *p2);
void FUN_0046cbe0(int p0, int p1, int p2);
int FUN_00443080(void);
int FUN_0040dc90(void);
void FUN_00421690(int p0);
void FUN_00486f90(void);
void FUN_0046baa0(int p0);
float FUN_00408a50(int p0);
void FUN_00409290(int *p0);
void FUN_00408b00(int p0, int p1, int p2, V3_00424eb0 *p3, float *p4);
void FUN_004704c0(int p0, V3_00424eb0 p1, float p2, void *p3);
void FUN_0045ba00(int p0, int p1);
void FUN_0040b290(int p0, int p1);
void FUN_0040eee0(int p0, int p1);
int FUN_0040e470(int p0);
void FUN_004215c0(int p0, int p1, int p2);
int FUN_0046c5c0(int p0);
int FUN_0046c790(int p0, int p1);
void thunk_FUN_00419760(int p0, int p1);
/* stand-in for 0x00422fd0 (no MATCH line: not the real callee shape, see header) */
static int s_StandIn_00422fd0(int n, int param_1)
{
  if (param_1 < 0x10) {
    FUN_0046c5c0(param_1);
    FUN_0046c790(param_1, 1);
    FUN_004215c0(param_1, 0x42480000, 0);
    FUN_004215c0(param_1, 0x42480000, 1);
    FUN_0045ba00(param_1, 2);
    if (DAT_007f0fd0 != 7) {
      if (FUN_0040e470(param_1) == 1) {
        thunk_FUN_00419760(param_1, 1);
      }
    }
  }
  return 0;
}
// MATCH: 0x00424eb0 FUN_00424eb0
int FUN_00424eb0(int dt)
{
  int i;
  int n;
  int nact;
  int ndone;
  int last;
  int h;
  float ang;
  V3_00424eb0 pos;
  int state[4];
  int timer[4];

  FUN_0040e340();
  if (FUN_0040e350() == 7) {
    for (i = 0; i < 4; i++) {
      if (FUN_0040e370(i) && FUN_0046c7b0(i) == 1) {
        FUN_0046c570(i);
      }
    }
  }
  n = 0;
  for (i = 0; i < 4; i++) {
    if (FUN_0040e370(i) && FUN_0046c7b0(i) == 1) {
      n++;
      FUN_0046c7d0(i, dt);
      FUN_0046cbb0(i, &state[i], &timer[i]);
    }
  }
  nact = 0;
  ndone = 0;
  for (i = 0; i < 4; i++) {
    if (FUN_0040e370(i) && FUN_0046c7b0(i) == 1 && state[i] != 0) {
      nact++;
      timer[i] += dt;
      if ((state[i] == 1 || state[i] == 2) && timer[i] >= 4000) {
        state[i] = 0xff;
      }
      FUN_0046cbe0(i, state[i], timer[i]);
      if (state[i] == 0xff) {
        ndone++;
        if (FUN_00443080() && DAT_007f0fd0 != 10 && FUN_0040dc90() == i) {
          timer[i] = 0;
          state[i] = 0;
          FUN_0046cbe0(i, 0, 0);
          FUN_00421690(i);
          FUN_00486f90();
          FUN_0046baa0(i);
          h = (int)FUN_00408a50(i);
          FUN_00409290(&h);
          FUN_00408b00(h, 0, 1, &pos, &ang);
          FUN_004704c0(i, pos, ang, DAT_00803324);
          ndone--;
        }
        if (DAT_007f0fd0 == 8) {
          if (i == 0) continue;
          timer[i] = 0;
          state[i] = 0;
          FUN_0046cbe0(i, 0, 0);
          FUN_00421690(i);
          FUN_00486f90();
          FUN_0046baa0(i);
          h = (int)FUN_00408a50(i);
          FUN_00409290(&h);
          FUN_00408b00(h, i, 4, &pos, &ang);
          FUN_004704c0(i, pos, ang, DAT_00803324);
          ndone--;
        }
        if (DAT_007f0fd0 == 9 || DAT_007f0fd0 == 7) {
          timer[i] = 0;
          state[i] = 0;
          FUN_0046cbe0(i, 0, 0);
          FUN_00421690(i);
          FUN_00486f90();
          FUN_0045ba00(i, 1);
          FUN_0046baa0(i);
          h = (int)FUN_00408a50(i);
          FUN_00409290(&h);
          FUN_00408b00(h, 0, 1, &pos, &ang);
          FUN_004704c0(i, pos, ang, DAT_00803324);
          ndone--;
        }
        if (DAT_007f0fd0 == 4) {
          timer[i] = 0;
          state[i] = 0;
          FUN_0046cbe0(i, 0, 0);
          FUN_00421690(i);
          FUN_00486f90();
          FUN_0045ba00(i, 1);
          FUN_0046baa0(i);
          h = (int)FUN_00408a50(i);
          FUN_00409290(&h);
          FUN_00408b00(h, i, 4, &pos, &ang);
          FUN_004704c0(i, pos, ang, DAT_00803324);
          ndone--;
        }
      }
    }
  }
  if (nact != 0 && nact == ndone) {
    last = 0;
    for (i = 0; i < 4; i++) {
      if (FUN_0040e370(i) && FUN_0046c7b0(i) == 1 && state[i] == 0xff) {
        s_StandIn_00422fd0(n, i);
        last = i + 1;
        if (nact > 1) {
          FUN_0040b290(i, 0);
        }
      }
    }
    if (last != 0) {
      FUN_0040eee0(last - 1, nact == 1);
    }
  }
  return 1;
}
