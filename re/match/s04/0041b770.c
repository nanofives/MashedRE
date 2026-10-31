// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041b770 (39 B): 100.0% exact (regs 100, ops 100), by hand. Raw converter: 27.3%.
   The record arrives in ECX: `static`, defined before its only caller FUN_0041c010 (call site 0x0041c062); the
   compiler picked ECX here, not EAX. The three dword copies are one 12-byte struct assignment
   (record +0x150 = table[record +0x164], table = two 12-byte entries at 0x005f334c).
   This file also holds the caller FUN_0041c010 (100%): a 24-byte local struct {0.45, 0.33, 1.0, -0.45, 0.33, 1.0}
   assigned to 0x005f334c (rep movsd, ecx 6), then the 2-entry loop over 0x0063cab8, then DAT_0063cda0 = 0.
   Residual: none. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
#ifdef __cplusplus
#define MATCH_STATIC
#define MATCH_ECX __fastcall
#else
#define MATCH_STATIC static
#define MATCH_ECX
#endif
typedef struct { float x, y, z; } V3_0041b770;
typedef struct { V3_0041b770 v[2]; } V3Pair_0041b770;
typedef struct { char pad[0x150]; V3_0041b770 col; char pad2[0x164 - 0x15c]; int mode; char pad3[0x16c - 0x168]; } Emit0041b770;   /* stride 0x16c */
MGLOBALA(Emit0041b770, DAT_0063cab8, 2, 0x0063cab8);
MGLOBAL(V3Pair_0041b770, DAT_005f334c, 0x005f334c);
MGLOBAL(int, DAT_0063cda0, 0x0063cda0);

// MATCH: 0x0041b770 FUN_0041b770
MATCH_STATIC void MATCH_ECX FUN_0041b770(Emit0041b770 *e)
{
  e->col = DAT_005f334c.v[e->mode];
}

// MATCH: 0x0041c010 FUN_0041c010
void FUN_0041c010(void)
{
  int i;
  V3Pair_0041b770 t;
  t.v[0].x = 0.45f;
  t.v[0].y = 0.33f;
  t.v[0].z = 1.0f;
  t.v[1].x = -0.45f;
  t.v[1].y = 0.33f;
  t.v[1].z = 1.0f;
  DAT_005f334c = t;
  for (i = 0; i < 2; i++)
    FUN_0041b770(&DAT_0063cab8[i]);
  DAT_0063cda0 = 0;
}
