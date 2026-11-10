// FLAGS: /O2 /GS /Ob1
/* MATCH-S29 (POWERUPINIT-B) 0x004583a0: the type-table init in slot 3 of 0x005f9998 (+0x14 at 0x005f9a6c).
   Zero-fills the four 0x18-byte dropper records at 0x0068b0d0 (0x60 B) and the 32 0x6c-byte mine records at
   0x0068a310 (0xd80 B). "MineDropper.dff" (0x0068b134) is cloned into dropper +0x14; "Mine.dff" (0x0068b130) is
   cloned into mine +0x58, whose two atoms are filed by part code (0x004b5190) at +0x50/+0x54: code 0 gets
   0x004b5240(atom, 1), code 1 gets 0x004b5240(atom, 0). Then 0x00458360(), and particle batch 0x0068b090
   ("lensflare-fs8", 0, 0x20) with 0x00476c10(batch, 0x005f9820). */
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
typedef struct V4_004583a0 { float a, b, c, d; } V4_004583a0;
typedef struct Drop_004583a0 { int f0[5]; int clump; } Drop_004583a0;
typedef struct Mine_004583a0 { int f0[20]; void *atom[2]; int clump; int f5c[4]; } Mine_004583a0;
MGLOBALA(Drop_004583a0, DAT_0068b0d0, 4, 0x0068b0d0);
MGLOBALA(Mine_004583a0, DAT_0068a310, 32, 0x0068a310);
MGLOBAL(int, DAT_0068b134, 0x0068b134);
MGLOBAL(int, DAT_0068b130, 0x0068b130);
MGLOBAL(char, DAT_0068b090, 0x0068b090);
MGLOBAL(V4_004583a0, DAT_005f9820, 0x005f9820);
void FUN_004b6520(void *p, int n);
void FUN_004c5c80(int a);
int FUN_0042a5d0(const char *name, int a, int b);
int FUN_004e6ab0(int clump);
int FUN_004b3fc0(int clump, void **out);
int FUN_004b5190(void *atom, int a, int b);
void FUN_004b5240(void *atom, int a);
int FUN_0040bb30(const char *name);
void FUN_004770c0(void *batch, int a, int b, int tex);
void FUN_00476cb0(void *batch, int a, int b);
void FUN_00476c10(void *batch, V4_004583a0 *v);
void FUN_00458360(void);

// MATCH: 0x004583a0 FUN_004583a0
void FUN_004583a0(void)
{
  void *atoms[2];
  Mine_004583a0 *m;
  void *atom;
  int code;
  int i;

  FUN_004b6520(DAT_0068b0d0, 0x60);
  FUN_004b6520(DAT_0068a310, 0xd80);
  FUN_004c5c80(0);
  DAT_0068b134 = FUN_0042a5d0("MineDropper.dff", 0, 0);
  for (i = 0; i < 4; i++) {
    DAT_0068b0d0[i].clump = FUN_004e6ab0(DAT_0068b134);
  }
  DAT_0068b130 = FUN_0042a5d0("Mine.dff", 0, 0);
  for (m = DAT_0068a310; (int)m < (int)(DAT_0068a310 + 32); m++) {
    m->clump = FUN_004e6ab0(DAT_0068b130);
    FUN_004b3fc0(m->clump, atoms);
    for (i = 0; i < 2; i++) {
      atom = atoms[i];
      code = FUN_004b5190(atom, 0, 0);
      switch (code) {
      case 0:
        FUN_004b5240(atom, 1);
        break;
      case 1:
        FUN_004b5240(atom, 0);
        break;
      }
      m->atom[code] = atom;
    }
  }
  FUN_00458360();
  FUN_004770c0(&DAT_0068b090, 0, 0x20, FUN_0040bb30("lensflare-fs8"));
  FUN_00476cb0(&DAT_0068b090, 5, 2);
  FUN_00476c10(&DAT_0068b090, &DAT_005f9820);
}
