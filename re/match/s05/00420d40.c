// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00420d40 by hand (h2): 100% (regs 100%). Raw g2s 72.7%. Four 0x24-byte entries at 0x0063e4b8
   (the original's loop pointer 0x0063e4c4 is entry+0xc): FUN_004b6520(entry, 8) then four dwords +0x08..+0x14 zeroed.
   Moved it: a local entry pointer e = &DAT_0063e4b8[i] (indexing the global directly anchors the induction pointer
   at +0x08 or puts the +0x08 store second, 90-95%). No residual. */
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
typedef struct {
  int u0[2];      /* +0x00 cleared by FUN_004b6520(p, 8) */
  int a;          /* +0x08 */
  int b;          /* +0x0c */
  int c;          /* +0x10 */
  int d;          /* +0x14 */
  int u18[3];     /* +0x18 */
} Ent24;
MGLOBALA(Ent24, DAT_0063e4b8, 4, 0x0063e4b8);
void FUN_004b6520(void *p, int n);
// MATCH: 0x00420d40 FUN_00420d40
void FUN_00420d40(void)
{
  int i;
  Ent24 *e;
  for (i = 0; i < 4; i++) {
    e = &DAT_0063e4b8[i];
    FUN_004b6520(e, 8);
    e->a = 0;
    e->b = 0;
    e->c = 0;
    e->d = 0;
  }
}
