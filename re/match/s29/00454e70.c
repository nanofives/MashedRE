// FLAGS: /O2 /GS /Ob1
/* MATCH-S29 (POWERUPINIT-B) 0x00454e70: the type-table init in slot 6 of 0x005f9998 (+0x14 at 0x005f9b2c).
   Zero-fills the four 0x14-byte entries at 0x00688358 (0x50 B), loads "flashunit.dff" into 0x006883a8, and per
   entry stores a clone at +0x10 and files its two atoms by part code (0x004b5190) at +0x0/+0x4: code 0 gets
   0x004b5240(atom, 1), code 1 gets 0x004b5240(atom, 0). Then particle batch 0x0088fec0 ("lensflare-fs8",
   0x807, 4) and 0x00476c10(batch, {0, 0, 0.5f, 1.0f}). Ends with a tail jump to 0x00454e40. */
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
typedef struct V4_00454e70 { float a, b, c, d; } V4_00454e70;
typedef struct E14_00454e70 { void *atom[2]; int f8; int fc; int clump; } E14_00454e70;
MGLOBALA(E14_00454e70, DAT_00688358, 4, 0x00688358);
MGLOBAL(int, DAT_006883a8, 0x006883a8);
MGLOBAL(char, DAT_0088fec0, 0x0088fec0);
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
void FUN_00476c10(void *batch, V4_00454e70 *v);
void FUN_00454e40(void);

// MATCH: 0x00454e70 FUN_00454e70
void FUN_00454e70(void)
{
  void *atoms[2];
  V4_00454e70 col = {0.0f, 0.0f, 0.5f, 1.0f};
  E14_00454e70 *e;
  void *atom;
  int code;
  int i;

  FUN_004b6520(DAT_00688358, 0x50);
  FUN_004c5c80(0);
  DAT_006883a8 = FUN_0042a5d0("flashunit.dff", 0, 0);
  for (e = DAT_00688358; (int)e < (int)(DAT_00688358 + 4); e++) {
    e->clump = FUN_004e6ab0(DAT_006883a8);
    FUN_004b3fc0(e->clump, atoms);
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
      e->atom[code] = atom;
    }
  }
  FUN_004770c0(&DAT_0088fec0, 0x807, 4, FUN_0040bb30("lensflare-fs8"));
  FUN_00476cb0(&DAT_0088fec0, 5, 2);
  FUN_00476c10(&DAT_0088fec0, &col);
  FUN_00454e40();
}
