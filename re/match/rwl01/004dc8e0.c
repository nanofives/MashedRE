// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 100% (regs 100%), class -. arrays DAT_007d70cc/dc/ec[4] with unsigned i<4 (gives ecx+const loop), COM Release __stdcall, head pointer reloaded after hit */
/* MATCH-RWL01 0x004dc8e0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef int code();
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(int *, DAT_007d70c0, 0x007d70c0);
MGLOBAL(undefined4, DAT_007d70c8, 0x007d70c8);
MGLOBAL(uint, DAT_00911fbc, 0x00911fbc);
MGLOBALA(int, DAT_007d70cc, 4, 0x007d70cc);
MGLOBALA(int, DAT_007d70dc, 4, 0x007d70dc);
MGLOBALA(int, DAT_007d70ec, 4, 0x007d70ec);
// MATCH: 0x004dc8e0 FUN_004dc8e0
void FUN_004dc8e0(void)
{
  int *p;
  int *q;
  int *nx;
  int *com;
  uint i;
  int h;

  DAT_007d70c8 = 0;
  p = DAT_007d70c0;
  for (i = 0; i < 4; i++) {
    DAT_007d70cc[i] = 0;
    DAT_007d70dc[i] = 0;
    h = DAT_007d70ec[i];
    if (h != 0) {
      for (q = p; q != 0; q = (int *)q[4]) {
        if (q[2] == h) {
          q[1] = 0;
          q[3] = 0;
          p = DAT_007d70c0;
          break;
        }
      }
      DAT_007d70ec[i] = 0;
    }
  }
  if ((DAT_00911fbc & 0x10000) != 0) {
    while (p != 0) {
      com = (int *)p[2];
      nx = (int *)p[4];
      if (com != 0) {
        (*(int (__stdcall *)(int *))*(int *)(*com + 8))(com);
        p[2] = 0;
        p = (int *)p[3];
        if (p != 0) {
          *p = 0;
        }
      }
      p = nx;
    }
  }
}
