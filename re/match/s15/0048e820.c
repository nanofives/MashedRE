// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h2 0x0048e820: 85.5% (regs 100%, ops 100%), 124/124 ins. Hand rewrite (raw 16.9%): 17 emitters, per-emitter param records (0x10 B at 0x00722140), 256 particles of 0x40 B per emitter at 0x007228b0 walked with j = 256; while (j--) (74.6% -> 85.5%), switch cases in layout order 7,9,10,8,6,1,13,14,15,16 with the stores in the order the tail merge needs. Residual: only the callee-saved register pair is swapped (ours i=esi, hoisted 1.0f=edi; original i=edi, 1.0f=esi). Tried: do/while vs while(i--) vs for, --i, unsigned i, declaration order, int-literal 1, 14/15/16 sharing a goto tail, separate x/y/z stores, empty default, permuter 423 variants (no gain). Class (b): register allocation that has not moved with any source change tried. */
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
typedef struct {
  float x, y, z;
  unsigned char pad0c[0x24 - 0x0c];
  float f24;
  int pad28;
  float f2c;
  int pad30;
  float f34;
  unsigned char pad38[0x40 - 0x38];
} Part40;
typedef struct {
  unsigned char a;
  unsigned char pad1[3];
  float f4;
  float f8;
  unsigned char r, g, b;
  unsigned char padf;
} EmitParm;
MGLOBALA(EmitParm, DAT_00722140, 17, 0x00722140);
MGLOBALA(int, DAT_00722250, 17 * 8, 0x00722250);
MGLOBALA(Part40, DAT_007228b0, 17 * 256, 0x007228b0);
void FUN_0048cee0(void);
void FUN_004b6480(void *p0, unsigned int p1);
// MATCH: 0x0048e820 FUN_0048e820
void FUN_0048e820(void)
{
  int i;
  int j;
  Part40 *pt;

  i = 17;
  do {
    i--;
    FUN_004b6480(&DAT_00722250[i * 8], 0x100);
    j = 256;
    while (j--) {
      pt = &DAT_007228b0[i * 256 + j];
      pt->x = pt->y = pt->z = 0.0f;
      pt->f24 = 0.0f;
      pt->f2c = 0.0f;
      switch (i) {
      case 7:
        pt->f34 = 0.98f;
        DAT_00722140[i].f8 = 0.03f;
        DAT_00722140[i].f4 = 2.0f;
        DAT_00722140[i].a = 0x80;
        DAT_00722140[i].r = 0x92;
        DAT_00722140[i].g = 0x74;
        DAT_00722140[i].b = 0x48;
        break;
      case 9:
        pt->f34 = 0.98f;
        DAT_00722140[i].f8 = 0.03f;
        DAT_00722140[i].a = 0x80;
        DAT_00722140[i].r = 0x92;
        DAT_00722140[i].g = 0x74;
        DAT_00722140[i].b = 0x48;
        DAT_00722140[i].f4 = 1.0f;
        break;
      case 10:
        pt->f34 = 0.999f;
        DAT_00722140[i].f8 = 0.06f;
        DAT_00722140[i].a = 0x20;
        DAT_00722140[i].g = 0xcb;
        DAT_00722140[i].b = 0x7e;
        DAT_00722140[i].r = 0xff;
        DAT_00722140[i].f4 = 1.0f;
        break;
      case 8:
        pt->f34 = 0.98f;
        DAT_00722140[i].f8 = 0.02f;
        DAT_00722140[i].f4 = 2.0f;
        DAT_00722140[i].a = 0xa0;
        DAT_00722140[i].r = 0x57;
        DAT_00722140[i].g = 0x45;
        DAT_00722140[i].b = 0x2b;
        break;
      case 6:
        pt->f34 = 0.8f;
        DAT_00722140[i].f8 = 0.01f;
        DAT_00722140[i].a = 0x40;
        DAT_00722140[i].g = 0xff;
        DAT_00722140[i].b = 0xff;
        DAT_00722140[i].r = 0xff;
        DAT_00722140[i].f4 = 1.0f;
        break;
      case 1:
        pt->f34 = 0.95f;
        DAT_00722140[i].f8 = 0.1f;
        DAT_00722140[i].a = 0x19;
        DAT_00722140[i].r = 0x5a;
        DAT_00722140[i].g = 0x5a;
        DAT_00722140[i].b = 0x5a;
        DAT_00722140[i].f4 = 1.0f;
        break;
      case 13:
        pt->f34 = 0.95f;
        DAT_00722140[i].f8 = 0.1f;
        DAT_00722140[i].f4 = 2.5f;
        DAT_00722140[i].a = 0x40;
        DAT_00722140[i].r = 0x5a;
        DAT_00722140[i].g = 0x5a;
        DAT_00722140[i].b = 0x5a;
        break;
      case 14:
        DAT_00722140[i].a = 0x80;
        pt->f34 = 0.97f;
        DAT_00722140[i].f8 = 0.05f;
        DAT_00722140[i].g = 0;
        DAT_00722140[i].b = 0;
        DAT_00722140[i].r = 0xff;
        DAT_00722140[i].f4 = 1.0f;
        break;
      case 15:
        DAT_00722140[i].a = 0xdc;
        pt->f34 = 0.97f;
        DAT_00722140[i].f8 = 0.05f;
        DAT_00722140[i].g = 0;
        DAT_00722140[i].b = 0;
        DAT_00722140[i].r = 0xff;
        DAT_00722140[i].f4 = 1.0f;
        break;
      case 16:
        DAT_00722140[i].a = 0xff;
        pt->f34 = 0.97f;
        DAT_00722140[i].f8 = 0.05f;
        DAT_00722140[i].g = 0;
        DAT_00722140[i].b = 0;
        DAT_00722140[i].r = 0xff;
        DAT_00722140[i].f4 = 1.0f;
        break;
      }
    }
    FUN_0048cee0();
  } while (i != 0);
}
