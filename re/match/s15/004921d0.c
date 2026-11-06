// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h6 0x004921d0: 100% (regs 100%). Hand: 24-byte local as int[6] (char[24] drew a /GS cookie), callee arities from the single add esp,0x38, FUN_004c2f00 returns short. Residual: none. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0077195c, 0x0077195c);
MGLOBAL(int, DAT_00771960, 0x00771960);
MGLOBALA(char, s_RWTEXDICTIONARY_005cf6a4, 16, 0x005cf6a4);
MGLOBALA(char, s_LoadIcon_005cf698, 9, 0x005cf698);
short FUN_004c2f00(void);
int FUN_004c2ed0(void *p0, int p1);
int FUN_00498bc0(void);
int FUN_00498bd0(void);
void FUN_00467110(int p0, int p1);
void FUN_0042b890(int p0);
void FUN_0042b8a0(int p0);
int FUN_0042d560(void);
void FUN_0042f660(void);
int FUN_004997b0(int p0, const char *p1, int *p2, int *p3);
int FUN_004b3eb0(int p0, int p1, int p2, int p3);
int FUN_004c5c00(int p0, const char *p1);
// MATCH: 0x004921d0 FUN_004921d0
void FUN_004921d0(void)
{
  int w;
  int h;
  int local_20;
  int local_1c;
  int local_18[6];

  FUN_004c2ed0(local_18, FUN_004c2f00());
  w = FUN_00498bc0();
  h = FUN_00498bd0();
  FUN_00467110(w, h);
  FUN_0042b890(w);
  FUN_0042b8a0(h);
  FUN_0042d560();
  FUN_0042f660();
  local_20 = 0;
  local_1c = 0;
  FUN_004997b0(0x194, s_RWTEXDICTIONARY_005cf6a4, &local_1c, &local_20);
  DAT_0077195c = FUN_004b3eb0(local_1c, local_20, 0, 0);
  if (DAT_0077195c != 0) {
    DAT_00771960 = FUN_004c5c00(DAT_0077195c, s_LoadIcon_005cf698);
  }
}
