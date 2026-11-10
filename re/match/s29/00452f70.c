// FLAGS: /O2 /GS /Ob1
/* MATCH-S29 (POWERUPINIT-B) 0x00452f70: the type-table init in slot 8 of 0x005f9998 (+0x14 at 0x005f9bac).
   Four 0x1c-byte launcher records from 0x00684e3c: clone of "mortar.dff" at +0x10, +0x4 = 0, +0xc = 0xbf800000
   (-1.0f), 0x00474d60(clone, 0), and the atom of part code 2 (0x004b5190) at +0x14; codes 0..1 get
   0x004b5240(atom, 1), code 2 gets 0x004b5240(atom, 0). Then 32 0x110-byte round records from 0x00684eac: clone
   of "MortarRound.dff" at +0x44, +0x0/+0x40/+0x4c/+0x50 = 0, particle pool 0x004892c0("smoke" texture, 0xf, 2,
   0x817) at +0x54, 0x004893a0(pool, 5, 6), 0x00474d60(clone, 0). Both source clumps are destroyed (0x004e6e00).
   Record bases: the launcher fields are read at 0x00684e40..0x00684e50 and the round fields at
   0x00684eac..0x00684ec0 + i*0x110; the bases chosen here make the two arrays adjacent (0x00684e3c + 4*0x1c =
   0x00684eac). The bases do not change the code. */
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
typedef struct Lnch_00452f70 { int f0; int f4; int f8; float fc; int clump; void *atom; int f18; } Lnch_00452f70;
typedef struct Round_00452f70 { int f0; int f4[15]; int f40; int clump; int f48; int f4c; int f50; int pool; int f58[46]; } Round_00452f70;
MGLOBALA(Lnch_00452f70, DAT_00684e3c, 4, 0x00684e3c);
MGLOBALA(Round_00452f70, DAT_00684eac, 32, 0x00684eac);
int FUN_0040bb30(const char *name);
int FUN_0042a5d0(const char *name, int a, int b);
int FUN_004e6ab0(int clump);
void FUN_00474d60(int clump, int a);
int FUN_004b3fc0(int clump, void **out);
int FUN_004b5190(void *atom, int a, int b);
void FUN_004b5240(void *atom, int a);
int FUN_004892c0(int tex, int a, int b, int c);
void FUN_004893a0(int pool, int a, int b);
void FUN_004e6e00(int clump);

// MATCH: 0x00452f70 FUN_00452f70
void FUN_00452f70(void)
{
  void *atoms[3];
  int tex, launcher, round;
  void *atom;
  int code;
  int i, j;
  Lnch_00452f70 *l;

  tex = FUN_0040bb30("smoke");
  launcher = FUN_0042a5d0("mortar.dff", 0, 0);
  round = FUN_0042a5d0("MortarRound.dff", 0, 0);
  for (i = 0; i < 4; i++) {
    l = &DAT_00684e3c[i];
    l->clump = FUN_004e6ab0(launcher);
    l->f4 = 0;
    l->fc = -1.0f;
    FUN_00474d60(l->clump, 0);
    FUN_004b3fc0(l->clump, atoms);
    for (j = 0; j < 3; j++) {
      atom = atoms[j];
      code = FUN_004b5190(atom, 0, 0);
      switch (code) {
      case 0:
      case 1:
        FUN_004b5240(atom, 1);
        break;
      case 2:
        FUN_004b5240(atom, 0);
        l->atom = atom;
        break;
      }
    }
  }
  FUN_004e6e00(launcher);
  for (i = 0; i < 32; i++) {
    DAT_00684eac[i].clump = FUN_004e6ab0(round);
    DAT_00684eac[i].f0 = 0;
    DAT_00684eac[i].f40 = 0;
    DAT_00684eac[i].f4c = 0;
    DAT_00684eac[i].f50 = 0;
    DAT_00684eac[i].pool = FUN_004892c0(tex, 0xf, 2, 0x817);
    FUN_004893a0(DAT_00684eac[i].pool, 5, 6);
    FUN_00474d60(DAT_00684eac[i].clump, 0);
  }
  FUN_004e6e00(round);
}
