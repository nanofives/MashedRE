// FLAGS: /O2 /GS /Ob1
/* 0x0040b540: 100% (regs 100%). g2s did not compile (C2059). Hand form: 4 x 16-byte table at 0x007f1a14
   (id at +0, same Ent shape as pilot a1_00432080), int[4] local, bubble sort. The one step that moved 96.2% -> 100%:
   swap local_10 as t = [j+1]; [j+1] = [j]; [j] = t, which anchors the shared induction base at &local_10[0]
   (the [j]-first spelling anchors it at &local_10[1], a 4-byte offset on 3 instructions). Permuter found nothing
   from 96.2%. No residual. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
typedef struct { int id; int a, b, c; } Ent;
MGLOBAL(int, DAT_007f0fcc, 0x007f0fcc);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBALA(Ent, DAT_007f1a14, 4, 0x007f1a14);
MGLOBALA(int, DAT_008a94e0, 4, 0x008a94e0);

int FUN_00417740(int param_1);

// MATCH: 0x0040b540 FUN_0040b540
void FUN_0040b540(int *param_1)
{
  int local_10[4];
  int i;
  int j;
  int n;
  int t;

  for (i = 0; i < 4; i++) {
    local_10[i] = DAT_008a94e0[i];
    if (DAT_007f1a14[i].id == -1) {
      local_10[i] = -100;
    }
    param_1[i] = i;
  }
  for (n = 3; n > 0; n--) {
    for (j = 0; j < n; j++) {
      if (local_10[j] < local_10[j + 1]) {
        t = local_10[j + 1];
        local_10[j + 1] = local_10[j];
        local_10[j] = t;
        t = param_1[j];
        param_1[j] = param_1[j + 1];
        param_1[j + 1] = t;
      }
    }
  }
  if ((DAT_007f0fd0 == 4) || (DAT_007f0fd0 == 7)) {
    for (i = 0; i < 4; i++) {
      param_1[i] = FUN_00417740(i);
    }
  }
  if (DAT_007f0fd0 == 9) {
    if (DAT_007f0fcc != 0) {
      param_1[0] = 0;
      param_1[1] = 1;
      return;
    }
    param_1[0] = 1;
    param_1[1] = 0;
  }
}
