// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR01 2026-11-08: 94.1% (regs 94.1%), was 91.2%. Nested if chain returning r; remaining: the original keeps one ret per failing call where ours jumps to a shared ret (12 return shapes tried, incl. && chains and separate result variables). Class (b). */
// h2 hand-matched. 91.2% (regs 91.2%, ops 89.7%). Residual: original lays out each failure return inline (xor eax,eax / ret after the jcc, the second one reusing eax==0), ours cross-jumps them into one shared tail. Tried return-value-as-variable, compound if, /Ox /Gy /Ob2 /G6 /Oi-. Class (a). Callbacks passed as function pointers (void FUN_xxx(void)).
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

typedef void (*Fn)(void);
void FUN_4e4e60(void);
void FUN_4e4f00(void);
void FUN_4e5e20(void);
void FUN_4e5e70(void);
void FUN_4e6250(void);
void FUN_4e64f0(void);
void FUN_4e7ef0(void);
void FUN_4e7f60(void);
void FUN_4e8540(void);
void FUN_4e8560(void);
void FUN_4ea4c0(void);
void FUN_4ea4d0(void);
void FUN_4f08e0(void);
void FUN_4f08f0(void);
void FUN_4f0990(void);
void FUN_4f09c0(void);
int FUN_004c2d90(int p0, int p1, Fn p2, Fn p3);
int FUN_004e3470(void);
int FUN_004e65c0(void);
int FUN_004f42d0(void);
// MATCH: 0x004e5d30 FUN_004e5d30
int FUN_004e5d30(void)
{
  int r;
  r = FUN_004c2d90(0x4, 0x501, FUN_4e7ef0, FUN_4e7f60);
  r |= FUN_004c2d90(0x30, 0x502, FUN_4f09c0, FUN_4f0990);
  r |= FUN_004c2d90(0x4, 0x503, FUN_4e8540, FUN_4e8560);
  r |= FUN_004c2d90(0x8, 0x504, FUN_4e64f0, FUN_4e6250);
  r |= FUN_004c2d90(0x4, 0x505, FUN_4e4f00, FUN_4e4e60);
  r |= FUN_004c2d90(0x0, 0x50a, FUN_4f08e0, FUN_4f08f0);
  r |= FUN_004c2d90(0xc, 0x507, FUN_4e5e70, FUN_4e5e20);
  r |= FUN_004c2d90(0x0, 0x50b, FUN_4ea4d0, FUN_4ea4c0);
  if (r < 0) {
    return 0;
  }
  r = FUN_004e3470();
  if (r != 0) {
    r = FUN_004e65c0();
    if (r != 0) {
      r = FUN_004f42d0() != 0;
    }
  }
  return r;
}
