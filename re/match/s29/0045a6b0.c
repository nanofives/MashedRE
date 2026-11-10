// FLAGS: /O2 /GS /Ob1
/* MATCH-S29 (POWERUPINIT-B) 0x0045a6b0: the type-table init in slot 4 of 0x005f9998 (+0x14 at 0x005f9aac).
   "FlameThrower.dff" cloned into four 0x68-byte records at 0x0068bb60, walked from the last one down. Per
   record: clone at +0x20; the atom of part code 1 (0x004b5190) at +0x24 and its frame matrix (0x004c0ed0 on
   atom +0x4, 0x40 bytes) at +0x28; 0x0045a3a0(record); a particle pool 0x004892c0(tex, 0x1e, 6, 0x37) at +0x10,
   then 0x004893a0(pool, 5, 2). Texture "exp_cloud2" in 0x0068d150. */
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
typedef struct Mat40_0045a6b0 { int m[16]; } Mat40_0045a6b0;
typedef struct Atom_0045a6b0 { int f0; int frame; } Atom_0045a6b0;
typedef struct Rec_0045a6b0 {
  int f0[4];
  int pool;      /* +0x10 */
  int f14[3];
  int clump;     /* +0x20 */
  Atom_0045a6b0 *atom; /* +0x24 */
  Mat40_0045a6b0 mat;  /* +0x28 */
} Rec_0045a6b0;
MGLOBALA(Rec_0045a6b0, DAT_0068bb60, 4, 0x0068bb60);
MGLOBAL(int, DAT_0068d150, 0x0068d150);
int FUN_0040bb30(const char *name);
int FUN_0042a5d0(const char *name, int a, int b);
int FUN_004e6ab0(int clump);
int FUN_004b3fc0(int clump, Atom_0045a6b0 **out);
int FUN_004b5190(Atom_0045a6b0 *atom, int a, int b);
Mat40_0045a6b0 *FUN_004c0ed0(int frame);
void FUN_0045a3a0(Rec_0045a6b0 *r);
int FUN_004892c0(int tex, int a, int b, int c);
void FUN_004893a0(int pool, int a, int b);
void FUN_004e6e00(int clump);

// MATCH: 0x0045a6b0 FUN_0045a6b0
void FUN_0045a6b0(void)
{
  int clump;
  Atom_0045a6b0 *atoms[10];
  Rec_0045a6b0 *r;
  int n;
  int i;

  clump = FUN_0042a5d0("FlameThrower.dff", 0, 0);
  DAT_0068d150 = FUN_0040bb30("exp_cloud2");
  i = 4;
  do {
    i--;
    r = &DAT_0068bb60[i];
    r->clump = FUN_004e6ab0(clump);
    n = FUN_004b3fc0(r->clump, atoms);
    while (n != 0) {
      n--;
      if (FUN_004b5190(atoms[n], 0, 0) == 1) {
        r->atom = atoms[n];
        r->mat = *FUN_004c0ed0(atoms[n]->frame);
      }
    }
    FUN_0045a3a0(r);
    r->pool = FUN_004892c0(DAT_0068d150, 0x1e, 6, 0x37);
    FUN_004893a0(r->pool, 5, 2);
  } while (i != 0);
  FUN_004e6e00(clump);
}
