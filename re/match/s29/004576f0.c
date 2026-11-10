// FLAGS: /O2 /GS /Ob1
/* MATCH-S29 (POWERUPINIT-B) 0x004576f0: the type-table init in slot 7 of 0x005f9998 (+0x14 at 0x005f9b6c).
   Two particle batches (0x0068a2c0, 0x0068a210) through 0x004770c0 / 0x00476cb0, then four clones of
   "oilslicker.dff" into the 16-byte records at 0x0068a250 (+0xc), each through 0x0040cda0 (0x3ecccccd = 0.4f)
   and the static 0x00456d00 (record in EAX, copied from re/match/s09/00456d00.c where it is C4). */
/* ---- MATCH-S29 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct Rec00456d00 {
  float m_0;
  int m_4;
  int m_8;
  int m_c;
} Rec00456d00;
MGLOBALA(Rec00456d00, DAT_0068a250, 4, 0x0068a250);
MGLOBAL(char, DAT_0068a2c0, 0x0068a2c0);
MGLOBAL(char, DAT_0068a210, 0x0068a210);
int FUN_0040bb30(const char *name);
void FUN_004770c0(void *batch, int a, int b, int tex);
void FUN_00476cb0(void *batch, int a, int b);
int FUN_0042a5d0(const char *name, int a, int b);
int FUN_004e6ab0(int clump);
void FUN_0040cda0(int clump, float f, int a);
void FUN_00474d60(int p0, int p1);
void FUN_004e6e00(int clump);

/* 0x00456d00 FUN_00456d00: matched in re/match/s09/00456d00.c; here so cl passes the record in EAX */
static void FUN_00456d00(Rec00456d00 *r)
{
  r->m_0 = 1.0f;
  r->m_8 = 0;
  r->m_4 = 0;
  FUN_00474d60(r->m_c, 0);
}

// MATCH: 0x004576f0 FUN_004576f0
void FUN_004576f0(void)
{
  int t0, t1, clump;
  int i;

  t0 = FUN_0040bb30("oil");
  t1 = FUN_0040bb30("oilShine");
  FUN_004770c0(&DAT_0068a2c0, 0x80c, 0x40, t0);
  FUN_00476cb0(&DAT_0068a2c0, 5, 6);
  FUN_004770c0(&DAT_0068a210, 0x4c, 0x40, t1);
  FUN_00476cb0(&DAT_0068a210, 5, 2);
  clump = FUN_0042a5d0("oilslicker.dff", 0, 0);
  for (i = 0; i < 4; i++) {
    DAT_0068a250[i].m_c = FUN_004e6ab0(clump);
    FUN_0040cda0(DAT_0068a250[i].m_c, 0.4f, 0);
    FUN_00456d00(&DAT_0068a250[i]);
    FUN_00474d60(DAT_0068a250[i].m_c, 0);
  }
  FUN_004e6e00(clump);
}
