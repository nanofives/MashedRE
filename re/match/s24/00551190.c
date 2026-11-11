// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00551190 (h2): 90.7% (regs 90.7%), 74/75 instructions. Builds an RtFS-style handler: RwMalloc
   (RwGlobals+0x108) 0x5c bytes, twelve method pointers (0x00550c30..0x005514e0), RwCalloc (RwGlobals+0x114) of
   n * 0x60 file entries each pointing back at the handler at +0x38, then FUN_00551330; on failure frees both
   through RwGlobals+0x10c. Walking a local entry pointer (not h->files) took 78.7% to 90.7%. Residual: the
   original pushes esi before the FUN_005504d0 early return (push ebx; push esi; ... pop esi; pop ebx); ours
   shrink-wraps push esi past it. Tried: h = 0 before the early return, permuter 195 compiles (no gain).
   Class (a): callee-saved push placement around an early return (flags_brief residual class).
   Callees whose arity was not read are declared with () (address taken only). */
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
typedef void *(*rwmalloc_t)(int size, int hint);
typedef void *(*rwcalloc_t)(int n, int size, int hint);
typedef void (*rwfree_t)(void *p);
typedef struct {
  char pad0[0x14];
  void *fn14;   /* +0x14 */
  void *fn18;   /* +0x18 */
  char pad1[0xc];
  void *fn28;   /* +0x28 */
  void *fn2c;   /* +0x2c */
  void *fn30;   /* +0x30 */
  void *fn34;   /* +0x34 */
  void *fn38;   /* +0x38 */
  void *fn3c;   /* +0x3c */
  void *fn40;   /* +0x40 */
  void *fn44;   /* +0x44 */
  void *fn48;   /* +0x48 */
  void *fn4c;   /* +0x4c */
  char pad2[8];
  char *files;  /* +0x58: n entries of 0x60 bytes */
} FSHandler;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_00550c30();
int FUN_00550c50();
int FUN_00550d80();
int FUN_00550da0();
int FUN_00550f20();
int FUN_005512b0(int p0);
__int64 FUN_00551050(int p0, long p1, int p2);
int FUN_00551090();
int FUN_00551180(int p0);
void FUN_00551290(int p0);
int FUN_005512d0();
int FUN_005514e0(int *p0);
int FUN_005504d0(int p0);
int FUN_00551330(FSHandler *p0, int p1, int p2, int p3);
// MATCH: 0x00551190 FUN_00551190
FSHandler *FUN_00551190(int param_1, int param_2, int param_3)
{
  FSHandler *h;
  char *f;
  int i;

  if (FUN_005504d0(param_3) != 0) {
    return 0;
  }
  h = (FSHandler *)(*(rwmalloc_t *)(DAT_007d3ff8 + 0x108))(0x5c, 0x401be);
  if (h != 0) {
    h->fn14 = (void *)FUN_00550c30;
    h->fn18 = (void *)FUN_00551290;
    h->fn28 = (void *)FUN_00550c50;
    h->fn2c = (void *)FUN_00550d80;
    h->fn30 = (void *)FUN_00550da0;
    h->fn34 = (void *)FUN_00550f20;
    h->fn38 = (void *)FUN_00551050;
    h->fn3c = (void *)FUN_00551090;
    h->fn40 = (void *)FUN_00551180;
    h->fn44 = (void *)FUN_005514e0;
    h->fn48 = (void *)FUN_005512b0;
    h->fn4c = (void *)FUN_005512d0;
    f = (char *)(*(rwcalloc_t *)(DAT_007d3ff8 + 0x114))(param_1, 0x60, 0x401be);
    h->files = f;
    for (i = 0; i < param_1; i++) {
      *(FSHandler **)(f + 0x38) = h;
      f += 0x60;
    }
    if (FUN_00551330(h, param_1, param_3, param_2) != 1) {
      (*(rwfree_t *)(DAT_007d3ff8 + 0x10c))(h->files);
      (*(rwfree_t *)(DAT_007d3ff8 + 0x10c))(h);
      return 0;
    }
  }
  return h;
}
