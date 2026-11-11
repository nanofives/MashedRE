// FLAGS: /O2 /Ob1 /G5
/* RWC-DRIVER 2026-11-11: 68.5% -> 100%. Moved by: 0x14-byte record struct at DAT_007d4130, realloc arm first (!= 0), switch with 0x1a..0x22 listed as their own 'DAT_007d4128 = 0' group (gives the byte index table + 2-entry jump table), index loops (i < 3 over 0x005d8b80), search loop with break then 'if (j >= n) append else max-update'. */
/* h3 hand-match: best 68.5% (regs 74.1%), class b. Fixed realloc arity (3 args, flags hoisted) and COM calls to __stdcall: 64.6 -> 68.5. Residual: switch 0x15..0x23 compiles as a compare chain, original is byte index table + 2-entry jump table (tried explicit case labels 0x1a..0x22 into default, no change); malloc/realloc arm layout and the post-loop realloc tail also differ. */
/* MATCH-RWL01 0x004c90a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#ifndef __cplusplus
#define true 1
#endif
typedef struct Rec14 { int a; int b; uint c; int d; int e; } Rec14;
MGLOBAL(Rec14 *, DAT_007d4130, 0x007d4130);
MGLOBAL(int, DAT_00911f80, 0x00911f80);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int *, DAT_007d4108, 0x007d4108);
MGLOBAL(undefined4, DAT_007d410c, 0x007d410c);
MGLOBAL(int, DAT_007d4128, 0x007d4128);
MGLOBALA(undefined4, DAT_005d8b80, 3, 0x005d8b80);
// MATCH: 0x004c90a0 FUN_004c90a0
void FUN_004c90a0(void)
{
  int i;
  int j;
  int k;
  int cnt;
  Rec14 *cur;

  if (DAT_007d4130 != 0) {
    DAT_007d4130 = (Rec14 *)(**(code **)(DAT_007d3ff8 + 0x110))(DAT_007d4130,(DAT_00911f80 * 5 + 5) * 4,0x1040411);
  }
  else {
    DAT_007d4130 = (Rec14 *)(**(code **)(DAT_007d3ff8 + 0x108))((DAT_00911f80 * 5 + 5) * 4,0x1040411);
  }
  (*(int (__stdcall *)(int *,undefined4,Rec14 *))*(int *)(*DAT_007d4108 + 0x20))(DAT_007d4108,DAT_007d410c,DAT_007d4130);
  switch (DAT_007d4130->d) {
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x18:
  case 0x19:
  case 0x23:
    DAT_007d4130->e = 0;
    DAT_007d4128 = 1;
    break;
  case 0x1a:
  case 0x1b:
  case 0x1c:
  case 0x1d:
  case 0x1e:
  case 0x1f:
  case 0x20:
  case 0x21:
  case 0x22:
    DAT_007d4128 = 0;
    break;
  default:
    DAT_007d4128 = 0;
  }
  for (i = 0; i < 3; i++) {
    cnt = (*(int (__stdcall *)(int *,undefined4,undefined4))*(int *)(*DAT_007d4108 + 0x18))(DAT_007d4108,DAT_007d410c,DAT_005d8b80[i]);
    for (k = 0; k < cnt; k++) {
      (*(int (__stdcall *)(int *,undefined4,undefined4,int,Rec14 *))*(int *)(*DAT_007d4108 + 0x1c))
                (DAT_007d4108,DAT_007d410c,DAT_005d8b80[i],k,&DAT_007d4130[DAT_007d4128]);
      for (j = 1; j < DAT_007d4128; j++) {
        cur = &DAT_007d4130[DAT_007d4128];
        if (cur->a == DAT_007d4130[j].a && cur->b == DAT_007d4130[j].b && cur->d == DAT_007d4130[j].d) break;
      }
      if (j >= DAT_007d4128) {
        DAT_007d4130[DAT_007d4128].e = 1;
        DAT_007d4128++;
      }
      else {
        if (DAT_007d4130[j].c < DAT_007d4130[DAT_007d4128].c) {
          DAT_007d4130[j].c = DAT_007d4130[DAT_007d4128].c;
        }
      }
    }
  }
  if (DAT_007d4128 < DAT_00911f80 + 1) {
    DAT_007d4130 = (Rec14 *)(**(code **)(DAT_007d3ff8 + 0x110))(DAT_007d4130,DAT_007d4128 * 0x14,0x1040411);
  }
}
