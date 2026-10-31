// FLAGS: /O2 /GS /Ob1
/* 0x0040c5b0 FUN_0040c5b0 (209 B): 100.0% exact, regs 100.0%.
   Raw g2s was 35.9%. The callee FUN_0040be50 (1850 B, not in this slice) takes the record index in EAX:
   the static-function convention (pilot finding 1), so this file holds a STAND-IN static FUN_0040be50 defined
   first (its body is not the real one; it only has to make a call so EAX/ECX/EDX die across it, which is what
   spills cur to the stack in the original). Also needed: kind >= 1 && kind <= 0xb (cmp 1/jb, cmp 0xb/ja),
   s = DAT_007ea5e4[idx] - 0xff00 and the colour loaded before the compares, n = 0x100 before done = 0,
   seen[4] = {0,0,0,0} initializer (puts the xor eax,eax first). Residual: none. */
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
typedef struct Rec20 { int f0; int f4; int f8; int pad[5]; } Rec20;
MGLOBAL(int, DAT_007e9de0, 0x007e9de0);
MGLOBAL(int, DAT_007f1030, 0x007f1030);
MGLOBALA(uint, DAT_007e9de4, 256, 0x007e9de4);
MGLOBALA(int, DAT_007ea1e4, 256, 0x007ea1e4);
MGLOBALA(int, DAT_007ea5e4, 256, 0x007ea5e4);
MGLOBALA(Rec20, DAT_007ea9e4, 256, 0x007ea9e4);
void FUN_0047d150(int p0);

/* stand-in for the real FUN_0040be50 (1850 B, takes the record index in EAX); not matched */
static void FUN_0040be50(int i)
{
  FUN_0047d150(DAT_007ea5e4[i]);
}

// MATCH: 0x0040c5b0 FUN_0040c5b0
void FUN_0040c5b0(void)
{
  int cur;
  int idx;
  int n;
  int done;
  uint kind;
  int *slot;
  int col;
  int s;
  int seen[4] = {0, 0, 0, 0};

  idx = DAT_007e9de0 - 1;
  if (idx < 0) {
    idx = 0xff;
  }
  cur = DAT_007f1030;
  n = 0x100;
  done = 0;
  for (; n > 0; n--) {
    if (done != 0) {
      return;
    }
    if (DAT_007ea1e4[idx] == cur) {
      kind = DAT_007e9de4[idx];
      if (kind >= 1 && kind <= 0xb) {
        if (kind == 6) {
          col = DAT_007ea9e4[idx].f0;
          s = DAT_007ea5e4[idx] - 0xff00;
          if (col == (int)0xff0000ff || col == (int)0xffff0080) {
            slot = &seen[s];
            if (*slot == 0) {
              FUN_0040be50(idx);
              *slot = 1;
            }
          }
        }
        else {
          FUN_0040be50(idx);
        }
      }
    }
    else {
      done = 1;
    }
    idx--;
    if (idx < 0) {
      idx = 0xff;
    }
  }
}
