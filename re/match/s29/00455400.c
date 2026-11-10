// FLAGS: /O2 /GS /Ob1
/* MATCH-S29 (POWERUPINIT-B) 0x00455400: the type-table init in slot 2 of 0x005f9998 (+0x14 at 0x005f9a2c).
   Five launcher records (stride 0x2c, fields read at 0x006885d4..0x006885f4 + i*0x2c) and five missile records
   (stride 0x6c, fields at 0x006883d8..0x00688418 + i*0x6c), walked i = 4..0. Launcher: clone of
   "MissileLauncher.dff" at +0x14, the atom of part code 1 (0x004b5190) at +0x1c, +0x0/+0x18/+0x20 = 0,
   +0x10 = 0x3f000000 (0.5f), +0x8 = i. Missile: clone of "missile.dff" at +0x54, particle emitter
   0x00489940(10, "smoke" texture, 0x817, 1, &{0xa0, 0xa0, 0xa0, 0x40}, 0.5f, 2.0f) at +0x60, the atom of part
   code 1 at +0x5c with 0x004c13e0(atom +0x4, {3.0f, 3.0f, 9.0f}, 1), and +0x20/+0x48/+0x58 = 0. Record bases are
   a choice that makes the arrays adjacent (0x006883b8 + 5*0x6c = 0x006885d4); they do not change the code. */
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
typedef struct V3_00455400 { float x, y, z; } V3_00455400;
typedef struct Rgba_00455400 { unsigned char r, g, b, a; } Rgba_00455400;
typedef struct Atom_00455400 { int f0; int frame; } Atom_00455400;
typedef struct Lnch_00455400 {
  int f0; int f4; int idx; int fc; float f10; int clump; int f18; Atom_00455400 *atom; int f20; int f24[2];
} Lnch_00455400;
typedef struct Msl_00455400 {
  int f0[8]; int f20; int f24[9]; int f48; int f4c[2]; int clump; int f58; Atom_00455400 *atom; int pp; int f64[2];
} Msl_00455400;
MGLOBALA(Lnch_00455400, DAT_006885d4, 5, 0x006885d4);
MGLOBALA(Msl_00455400, DAT_006883b8, 5, 0x006883b8);
int FUN_0040bb30(const char *name);
int FUN_0042a5d0(const char *name, int a, int b);
void FUN_004c5c80(int a);
int FUN_004e6ab0(int clump);
int FUN_004b3fc0(int clump, Atom_00455400 **out);
int FUN_004b5190(Atom_00455400 *atom, int a, int b);
int FUN_00489940(int a, int tex, int b, int c, Rgba_00455400 *col, float f0, float f1);
void FUN_004c13e0(int frame, V3_00455400 *v, int a);
void FUN_004e6e00(int clump);

// MATCH: 0x00455400 FUN_00455400
void FUN_00455400(void)
{
  Rgba_00455400 col;
  int i;
  int launcher, missile, tex;
  Atom_00455400 *atoms[10];
  int n;

  i = 5;
  tex = FUN_0040bb30("smoke");
  col.r = 0xa0;
  col.g = 0xa0;
  col.b = 0xa0;
  col.a = 0x40;
  launcher = FUN_0042a5d0("MissileLauncher.dff", 0, 0);
  missile = FUN_0042a5d0("missile.dff", 0, 0);
  FUN_004c5c80(0);
  do {
    i--;
    DAT_006885d4[i].clump = FUN_004e6ab0(launcher);
    n = FUN_004b3fc0(DAT_006885d4[i].clump, atoms);
    while (n != 0) {
      n--;
      if (FUN_004b5190(atoms[n], 0, 0) == 1) {
        DAT_006885d4[i].atom = atoms[n];
      }
    }
    DAT_006885d4[i].f0 = 0;
    DAT_006885d4[i].f10 = 0.5f;
    DAT_006885d4[i].idx = i;
    DAT_006885d4[i].f18 = 0;
    DAT_006885d4[i].f20 = 0;
    DAT_006883b8[i].clump = FUN_004e6ab0(missile);
    DAT_006883b8[i].pp = FUN_00489940(10, tex, 0x817, 1, &col, 0.5f, 2.0f);
    n = FUN_004b3fc0(DAT_006883b8[i].clump, atoms);
    while (n != 0) {
      n--;
      if (FUN_004b5190(atoms[n], 0, 0) == 1) {
        V3_00455400 sc = {3.0f, 3.0f, 9.0f};
        DAT_006883b8[i].atom = atoms[n];
        FUN_004c13e0(atoms[n]->frame, &sc, 1);
      }
    }
    DAT_006883b8[i].f48 = 0;
    DAT_006883b8[i].f58 = 0;
    DAT_006883b8[i].f20 = 0;
  } while (i != 0);
  FUN_004e6e00(launcher);
  FUN_004e6e00(missile);
}
