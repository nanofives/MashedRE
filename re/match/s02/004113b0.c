// FLAGS: /O2 /GS /Ob1
/* 0x004113b0 FUN_004113b0 (260 B): 100.0% exact, regs 100.0%.
   Raw g2s was 37.2% (GS cookie read as unaff_retaddr, memset expanded). Hand form: {key,val} table at
   0x005f29d0 ended by key -1, intrinsic memset, RwGlobals slots +0x108 (alloc) and +0xc4 (sprintf) as
   function pointers, the size check as its own if around the error print + for(;;) (the goto-ok form is
   78.6%: block order). Residual: none. */
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
typedef struct Pair { int key; int val; } Pair;
typedef void *(*RwMallocFn)(uint size, int hint);
typedef int (*RwSprintfFn)(char *buf, const char *fmt, ...);
MGLOBAL(uint, DAT_008a94ac, 0x008a94ac);
MGLOBALA(Pair, DAT_005f29d0, 1, 0x005f29d0);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(void *, DAT_008a94a8, 0x008a94a8);

void *memset(void *p, int c, unsigned int n);
#pragma intrinsic(memset)
int FID_conflict__wprintf(const char *fmt, ...);
int FUN_00482900(int p0, int p1);
void FUN_004987b0(const char *fmt, ...);

// MATCH: 0x004113b0 FUN_004113b0
int FUN_004113b0(void)
{
  Pair *p;
  char name[64];

  DAT_008a94ac = 0;
  if (DAT_005f29d0[0].key != -1) {
  for (p = DAT_005f29d0; p->key != -1; p++) {
    DAT_008a94ac += FUN_00482900(p->val, 10);
  }
  }
  if (DAT_008a94ac != 0x24a3c) {
  FUN_004987b0("GHOST Size ERROR %d != %d\n", DAT_008a94ac, 0x24a3c);
  for (;;) {
  }
  }
  FID_conflict__wprintf("Replay buffer will be %d bytes in size\n", 0x24a3c);
  DAT_008a94a8 = (*(RwMallocFn *)(DAT_007d3ff8 + 0x108))(DAT_008a94ac, 0);
  if (DAT_008a94a8 != 0) {
    memset(DAT_008a94a8, 0, DAT_008a94ac);
    (*(RwSprintfFn *)(DAT_007d3ff8 + 0xc4))(name, "%sAll%d.rep", "c:\\toast\\", 0x10b);
  }
  return 1;
}
