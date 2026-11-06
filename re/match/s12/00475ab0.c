// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 h8 0x00475ab0: best 32.4% (regs 46.6%), 148/115 instructions, natural form below. Raw converter 29.4%.
   Adds one 0x50-byte record to group param_1 (same record arrays as 0x00475c10: elements DAT_00691604[g], count
   DAT_00691610[g], live count DAT_00691614[g]). When the group is full, the record with the largest +0x44 (start value
   DAT_005d757c) is retired (+0x48 = 0, live count - 1). Then the first record with +0x48 == 0 gets the 64-byte block
   *param_2, colour *param_3 (DAT_0061324c when param_3 is 0), +0x44 = 0, +0x48 = 1, +0x4c = alpha byte, +0x4e = 0.
   Residual: the original unrolls the max-search loop x4 (mov edi,3 / cmp esi,4, step 0x140); 9176 unrolls the same
   loop x8 (mov esi,7 / cmp ebx,8). Everything after shifts registers and stack slots, so the score is low although
   the rest of the shape matches (prologue, remainder loop, retire, fill). Same residual as S01 0x00401690, S16
   0x00499d90, 0x004427c0, 0x00482140. Tried: index vs pointer loop, unsigned i, while, swapped assignments, `>`/`!<=`
   compares, double max, element via the global, file-static DAT_00691614 (33.1%), pointer-walk fill loop (24.3%),
   and a hand-written x4 body + remainder loop (36.2%, 116/115, but 9176 turns it into a counted dec-loop; kept in
   D:/MashedWT/MATCH-S12_data/scratch/h8/alt_00475ab0_manual_x4.c, not used here because it is not the source shape).
   Class (a): unroll factor of a variable-trip loop (compiler version). */
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
typedef struct {
  undefined4 m[16];
  unsigned char col[4];
  float age;
  int active;
  short s4c;
  short s4e;
} Elem;
MGLOBAL(Elem **, DAT_00691604, 0x00691604);
MGLOBAL(int *, DAT_00691610, 0x00691610);
MGLOBAL(int *, DAT_00691614, 0x00691614);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined4, DAT_0061324c, 0x0061324c);
typedef struct { undefined4 a[16]; } M64;
// MATCH: 0x00475ab0 FUN_00475ab0
void FUN_00475ab0(int param_1, M64 *param_2, unsigned char *param_3)
{
  Elem *e;
  Elem *best;
  int n;
  int i;
  float mx;

  if (param_3 == 0) param_3 = (unsigned char *)&DAT_0061324c;
  e = DAT_00691604[param_1];
  n = DAT_00691610[param_1];
  if (DAT_00691614[param_1] == n) {
    best = 0;
    mx = DAT_005d757c;
    for (i = 0; i < n; i++) {
      if (mx < e[i].age) {
        best = &e[i];
        mx = e[i].age;
      }
    }
    best->active = 0;
    DAT_00691614[param_1]--;
  }
  for (i = 0; i < n; i++) {
    if (e[i].active == 0) {
      *(M64 *)e[i].m = *param_2;
      *(undefined4 *)e[i].col = *(undefined4 *)param_3;
      e[i].age = 0;
      e[i].active = 1;
      e[i].s4c = param_3[3];
      e[i].s4e = 0;
      DAT_00691614[param_1]++;
      return;
    }
  }
}
