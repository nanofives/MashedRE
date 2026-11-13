// FLAGS: /O2 /Ob1 /G5 /Oy-
/* MATCH-S20 0x004cb4c0 (h2, h5): 90.2% (regs 90.2%) with FLAGS /O2 /Ob1 /G5 /Oy- (83.7% without /Oy-). Raw 59.2%.
   D3D SetTransform(0x100) world-matrix cache: Mat16 struct globals (copy = rep movsd struct assignment), int compare
   loop against the cached matrix, int return hr >= 0.
   Round 2 (h5): the original has a push ebp / mov ebp, esp frame with no locals: /Oy- (frame pointer kept)
   reproduces it exactly (83.7 -> 90.2), so this function's object was built without frame-pointer omission.
   Residual (b): the original keeps a 16-step down-counter in edx and compares cache[i] (loaded) against the
   new matrix in memory; ours compares the pointer against end. Tried with /Oy-: both compare orders (89.1),
   pointer do-while / while(n--) / byte-offset forms (77.6-83.5), 16 - i index (88.0). Earlier (h2): float matrix
   type, permuter. relocheck not run (below 100%).
   MATCH-RWT-S1 (2026-11-13), still 90.2% (this source kept): a down-counter loop
   `const int *p = new, *q = cache; int n = 16; do { if (*q != *p) goto set; p++; q++; } while (--n);` emits the
   original's exact loop (eax = new, ecx = cache - new, edx = 16, `mov esi,[ecx+eax]` / `cmp esi,[eax]` / `dec edx`)
   under /G6 /Oy- (loop identical, but the /G6 scheduler moves the prologue and store order: 87.0%). Under /G5 /Oy-
   the same source splits the compare into `mov ebx,[eax]` / `cmp esi,ebx` and pushes ebx (83.5%, 97/92 ins).
   No source form tried keeps the folded compare under /G5: 18 loop shapes, 5 structural rewrites (no cache local,
   flag variable, break form, Mat16 pointer, cache read after the stores), volatile p or q, short/char counter,
   goto loops, and flags /Oa /Ow /Os /Gr /G3 /G4 /Og, rw9044 toolchain; permuter 1295 compiles on the counter form.
   Open lever: what keeps c2 from taking ebx for the split in the original. Class (b). */
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
typedef struct { int m[16]; } Mat16;
typedef long (__stdcall *setxf_fn)(int *, int, Mat16 *);
MGLOBAL(Mat16 *, DAT_007d4558, 0x007d4558);
MGLOBAL(Mat16, DAT_00618258, 0x00618258);
MGLOBAL(int, DAT_007d4154, 0x007d4154);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(Mat16, DAT_005d8b90, 0x005d8b90);
// MATCH: 0x004cb4c0 FUN_004cb4c0
int FUN_004cb4c0(int *param_1)
{
  int i;
  long r;
  if (param_1 != 0 && (param_1[3] & 0x20000) == 0) {
    int *cache = DAT_007d4558->m;
    DAT_00618258.m[0] = param_1[0];
    DAT_00618258.m[1] = param_1[1];
    DAT_00618258.m[2] = param_1[2];
    DAT_00618258.m[4] = param_1[4];
    DAT_00618258.m[5] = param_1[5];
    DAT_00618258.m[6] = param_1[6];
    DAT_00618258.m[8] = param_1[8];
    DAT_00618258.m[9] = param_1[9];
    DAT_00618258.m[10] = param_1[10];
    DAT_00618258.m[12] = param_1[12];
    DAT_00618258.m[13] = param_1[13];
    DAT_00618258.m[14] = param_1[14];
    if (DAT_007d4154 == 0) {
      for (i = 0; i < 16; i++) {
        if (DAT_00618258.m[i] != cache[i]) goto set;
      }
      return 1;
    }
set:
    *(Mat16 *)cache = DAT_00618258;
    r = (*(setxf_fn *)(*DAT_007d4110 + 0xb0))(DAT_007d4110, 0x100, &DAT_00618258);
    DAT_007d4154 = 0;
    return r >= 0;
  }
  if (DAT_007d4154 == 0) {
    *DAT_007d4558 = DAT_005d8b90;
    r = (*(setxf_fn *)(*DAT_007d4110 + 0xb0))(DAT_007d4110, 0x100, &DAT_005d8b90);
    DAT_007d4154 = 1;
    return r >= 0;
  }
  return 1;
}
