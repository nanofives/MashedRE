// FLAGS: /O2 /GS /Ob1
/* 0x00411f30 FUN_00411f30 (216 B): 100.0% exact, regs 100.0%.
   Raw g2s was 86.2% (GS cookie read as unaff_retaddr, byte locals, pointer loop). Hand form: RGBA colour
   struct {0xff,0,0,0xff}, char path[1024] (the /GS cookie is the compiler's), callee arities from the pushes,
   i-indexed loop over 6 x 0x2c records starting at 0x0063bb38 (fields +0 and +4). Residual: none. */
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
typedef struct RGBA { uchar r, g, b, a; } RGBA;
typedef struct Rec2c { undefined4 tex; int f4; int pad[9]; } Rec2c;
MGLOBAL(undefined4, DAT_0063bc40, 0x0063bc40);
MGLOBALA(Rec2c, DAT_0063bb38, 6, 0x0063bb38);

void FUN_004b6520(char *p0, int p1);
undefined4 thunk_FUN_004b68e0(void);
undefined4 thunk_FUN_004b68f0(const char *p0);
undefined4 FUN_004b3b70(const char *p0, char *p1, undefined4 p2);
undefined4 FUN_004781b0(const char *p0, int p1);
undefined4 FUN_004b3e40(char *p0, int p1, int p2, int p3);
void FUN_004b5320(undefined4 p0, int p1, int p2);
void FUN_004b5580(undefined4 p0, RGBA *p1);
undefined4 FUN_004b3f90(undefined4 p0);

// MATCH: 0x00411f30 FUN_00411f30
void FUN_00411f30(void)
{
  int i;
  undefined4 r;
  RGBA col;
  char path[1024];

  col.r = 0xff;
  col.g = 0;
  col.b = 0;
  col.a = 0xff;
  FUN_004b6520(path, 0x400);
  thunk_FUN_004b68e0();
  FUN_004b3b70("lazer.dff", path, thunk_FUN_004b68f0("lazer.dff"));
  DAT_0063bc40 = FUN_004781b0("LaserTower.dff", 0);
  for (i = 0; i < 6; i++) {
    r = FUN_004b3e40(path, 0x400, 0, 0);
    FUN_004b5320(r, 0x40, 1);
    FUN_004b5580(r, &col);
    DAT_0063bb38[i].tex = FUN_004b3f90(r);
    DAT_0063bb38[i].f4 = 0;
  }
}
