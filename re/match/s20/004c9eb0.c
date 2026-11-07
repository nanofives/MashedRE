// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004c9eb0 (h1): 94.8% (regs 94.8%). Residual: the prologue places `xor ebx, ebx`
   and `push ebx` (original: push ebx between the param test and its branch, xor ebx after push esi/edi). Tried: best = 0
   inside the if (91.4%, push ebx moves into the branch), early return, final compare outside the if (73.3%),
   both compare orders, flag sweep (game flags 68.9%), permuter 230 compiles. Class (b). Stores param_1 in DAT_006181c4; when nonzero, for each of the 3 formats at
   DAT_005d8b80 calls the D3D9 object (DAT_007d4108) vtable +0x18 (count) and walks indices count-1..0 through
   vtable +0x1c into a 16-byte mode record; field +8 equal to DAT_006181c4 ends the walk with that value, otherwise
   the largest field +8 below it is kept. Writes the result back when it differs. FLAGS /G5 (RW core). */
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
typedef struct Mode16 { unsigned int w; unsigned int h; unsigned int rate; int fmt; } Mode16;
typedef unsigned int (__stdcall *D3DCountFn)(void *self, int adapter, int fmt);
typedef int (__stdcall *D3DEnumFn)(void *self, int adapter, int fmt, unsigned int i, Mode16 *m);
MGLOBAL(unsigned int, DAT_006181c4, 0x006181c4);
MGLOBAL(int **, DAT_007d4108, 0x007d4108);
MGLOBAL(int, DAT_007d410c, 0x007d410c);
MGLOBALA(int, DAT_005d8b80, 3, 0x005d8b80);

// MATCH: 0x004c9eb0 FUN_004c9eb0
void FUN_004c9eb0(unsigned int param_1)
{
  unsigned int best = 0;
  unsigned int n;
  int i;
  Mode16 mode;
  DAT_006181c4 = param_1;
  if (param_1 != 0) {
    for (i = 0; i < 3; i++) {
      n = ((D3DCountFn)(*DAT_007d4108)[0x18 / 4])(DAT_007d4108, DAT_007d410c, DAT_005d8b80[i]);
      while (n != 0) {
        n--;
        ((D3DEnumFn)(*DAT_007d4108)[0x1c / 4])(DAT_007d4108, DAT_007d410c, DAT_005d8b80[i], n, &mode);
        if (mode.rate == DAT_006181c4) {
          best = DAT_006181c4;
          break;
        }
        if (mode.rate < DAT_006181c4 && best < mode.rate) {
          best = mode.rate;
        }
      }
    }
    if (DAT_006181c4 != best) {
      DAT_006181c4 = best;
    }
  }
}
