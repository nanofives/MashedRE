// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x004248b0: 100% (regs 100%), relocheck OK. By hand (h8). The raw converter's file scored 100% but
   relocheck was BAD: with separate MGLOBALs per field the masked compare cannot tell which load comes first, and
   every statement order tried put 0x00899560 before 0x008995b4. Rewritten as a 4-entry array of 0x138-byte records
   (0x008994c0 source, 0x008999a0 destination, 0x008994c0 + 4*0x138 = 0x008999a0); the compiler emits each pair of
   adjacent statements swapped, so the statement order below (a0,f4,a4,f8,a8,f0,fc,000) gives the original load order
   f4,a0,f8,a4,f0,a8,000,fc. No residual. */
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
typedef struct S004248b0 {
  int f_000;
  char pad_004[0x9c];
  undefined4 f_0a0;
  undefined4 f_0a4;
  undefined4 f_0a8;
  char pad_0ac[0x44];
  undefined4 f_0f0;
  undefined4 f_0f4;
  undefined4 f_0f8;
  undefined4 f_0fc;
  char pad_100[0x38];
} S004248b0;
MGLOBALA(S004248b0, DAT_008994c0, 4, 0x008994c0);
MGLOBALA(S004248b0, DAT_008999a0, 4, 0x008999a0);
// MATCH: 0x004248b0 FUN_004248b0
void FUN_004248b0(void)
{
  int i;
  for (i = 0; i < 4; i++) {
    DAT_008999a0[i].f_0a0 = DAT_008994c0[i].f_0a0;
    DAT_008999a0[i].f_0f4 = DAT_008994c0[i].f_0f4;
    DAT_008999a0[i].f_0a4 = DAT_008994c0[i].f_0a4;
    DAT_008999a0[i].f_0f8 = DAT_008994c0[i].f_0f8;
    DAT_008999a0[i].f_0a8 = DAT_008994c0[i].f_0a8;
    DAT_008999a0[i].f_0f0 = DAT_008994c0[i].f_0f0;
    DAT_008999a0[i].f_0fc = DAT_008994c0[i].f_0fc;
    DAT_008999a0[i].f_000 = DAT_008994c0[i].f_000 + 1;
  }
}
