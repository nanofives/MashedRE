// FLAGS: /O2 /GS /Ob1
/* MATCH-S29 (POWERUPINIT-B) 0x0045b830: the type-table init in slot 5 of 0x005f9998 (+0x14 at 0x005f9aec).
   Zero-fills the four 0x24-byte slots at 0x0068d158 (0x004b6520, 0x90 B), loads "shotgun.dff" into 0x0068d1e8,
   and per slot stores a clone at +0x1c and files its three atoms by part code (0x004b5190) at +0x0..+0x8: code 0
   gets 0x004b5240(atom, 1); codes 1..2 get 0x004b5240(atom, 0), 0x004c13e0(atom +0x4, {4.0f, 4.0f, 4.0f} as a block-scope initializer, 1) and
   0x004b52f0(atom, 0x40, 1). Ends with a tail jump to 0x0045b810. */
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
typedef struct V3_0045b830 { float x, y, z; } V3_0045b830;
typedef struct Atom_0045b830 { int f0; int frame; } Atom_0045b830;
typedef struct Slot45b360 {
    Atom_0045b830 *a[3];
    int m_c;
    int m_10;
    int m_14;
    int m_18;
    int m_frame;
    int m_20;
} Slot45b360;
MGLOBALA(Slot45b360, DAT_0068d158, 4, 0x0068d158);
MGLOBAL(int, DAT_0068d1e8, 0x0068d1e8);
void FUN_004b6520(void *p, int n);
void FUN_004c5c80(int a);
int FUN_0042a5d0(const char *name, int a, int b);
int FUN_004e6ab0(int clump);
int FUN_004b3fc0(int clump, Atom_0045b830 **out);
int FUN_004b5190(Atom_0045b830 *atom, int a, int b);
void FUN_004b5240(Atom_0045b830 *atom, int a);
void FUN_004c13e0(int frame, V3_0045b830 *v, int a);
void FUN_004b52f0(Atom_0045b830 *atom, int a, int b);
void FUN_0045b810(void);

// MATCH: 0x0045b830 FUN_0045b830
void FUN_0045b830(void)
{

  Atom_0045b830 *atoms[3];
  Slot45b360 *s;
  Atom_0045b830 *atom;
  int code;
  int i;

  FUN_004b6520(DAT_0068d158, 0x90);
  FUN_004c5c80(0);
  DAT_0068d1e8 = FUN_0042a5d0("shotgun.dff", 0, 0);
  for (s = DAT_0068d158; (int)s < (int)(DAT_0068d158 + 4); s++) {
    s->m_frame = FUN_004e6ab0(DAT_0068d1e8);
    FUN_004b3fc0(s->m_frame, atoms);
    for (i = 0; i < 3; i++) {
      atom = atoms[i];
      code = FUN_004b5190(atom, 0, 0);
      switch (code) {
      case 0:
        FUN_004b5240(atom, 1);
        break;
      case 1:
      case 2:
        FUN_004b5240(atom, 0);
        {
          V3_0045b830 sc = {4.0f, 4.0f, 4.0f};
          FUN_004c13e0(atom->frame, &sc, 1);
        }
        FUN_004b52f0(atom, 0x40, 1);
        break;
      }
      s->a[code] = atom;
    }
  }
  FUN_0045b810();
}
