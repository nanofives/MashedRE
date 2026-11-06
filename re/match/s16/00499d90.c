// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00499d90 (h6): builds two vertices per 0x20-byte input record into DAT_007e2580
   (v0 = record pos + packed ARGB, v1 = pos + dir with the alpha byte cleared), then sets render state and draws
   them through the device vtable (+0x164 with 0x42, +0x14c with 2/n/buffer/0x10). DAT_007e2580 element count
   [UNCERTAIN: array length not read; 0x400 used]. 
   Best 43.9% (regs 43.9%, ops 63.6%), 385/249 ins. Prologue, the remainder loop (byte-identical, including the
   reload of v0.color before the & 0xffffff, which needs the v++ / v[-1] form) and the whole tail after the loop
   (render state, SetFVF cache, DrawPrimitiveUP-shaped call) are identical. Residual: the variable-trip record loop.
   Original unrolls it x4 (`mov edi,3 / cmp ebp,4`, step 0x80); 9176 unrolls the same body x8 (`mov edi,7 /
   cmp ebp,8`). Tried: index vs v[0]/v[1] vs v++ twice, separate Vtx *w, record pointer, global indexing,
   color via pointer, <<= / |= spelling, volatile read (kills unroll), hand-unrolled x4 + remainder (36.5%).
   Same finding as S01 0x00401690. Class (a): compiler-version candidate (unroll factor of a variable-trip loop). */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
typedef struct Vtx_ { float x; float y; float z; unsigned int color; } Vtx;
typedef struct Rec_ { float px; float py; float pz; float dx; float dy; float dz; int pad18;
  unsigned char r; unsigned char g; unsigned char b; unsigned char a; } Rec;
typedef struct Dev_ Dev;
typedef long (__stdcall *DevSetFVF)(Dev *self, unsigned int fvf);
typedef long (__stdcall *DevDrawUP)(Dev *self, int type, unsigned int count, const void *data, unsigned int stride);
struct Dev_ { void **vtbl; };
typedef int (*RwFn2)(int a, int b);
MGLOBALA(Vtx, DAT_007e2580, 0x400, 0x007e2580);
MGLOBALA(float, DAT_00773928, 16, 0x00773928);
MGLOBAL(int, DAT_006181c8, 0x006181c8);
MGLOBAL(int, DAT_006181cc, 0x006181cc);
MGLOBAL(Dev *, DAT_007d4110, 0x007d4110);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_004cb330(int a, void *m);
int FUN_004d5480(int state, int value);
int FUN_004cb130(int a, int b, int c, int d);
int FUN_004d53b0(void);
// MATCH: 0x00499d90 FUN_00499d90
int FUN_00499d90(Rec *param_1, int param_2)
{
  Vtx *v;
  int i;

  v = DAT_007e2580;
  for (i = 0; i < param_2; i++) {
    v->x = param_1[i].px;
    v->y = param_1[i].py;
    v->z = param_1[i].pz;
    v->color = param_1[i].a << 8;
    v->color = (v->color | param_1[i].r) << 8;
    v->color = (v->color | param_1[i].g) << 8;
    v->color = v->color | param_1[i].b;
    v++;
    v->x = param_1[i].dx + param_1[i].px;
    v->y = param_1[i].dy + param_1[i].py;
    v->z = param_1[i].dz + param_1[i].pz;
    v->color = v[-1].color & 0xffffff;
    v++;
  }
  FUN_004cb330(0x100, DAT_00773928);
  FUN_004d5480(0x89, 0);
  FUN_004d5480(0x16, 1);
  if (DAT_006181c8 != 0x42) {
    DAT_006181c8 = 0x42;
    DAT_006181cc = -1;
    ((DevSetFVF)DAT_007d4110->vtbl[0x164 / 4])(DAT_007d4110, 0x42);
  }
  (*(RwFn2 *)(DAT_007d3ff8 + 0x20))(0xc, 1);
  FUN_004cb130(0, 0, 0, 0);
  FUN_004d53b0();
  ((DevDrawUP)DAT_007d4110->vtbl[0x14c / 4])(DAT_007d4110, 2, param_2, DAT_007e2580, 0x10);
  (*(RwFn2 *)(DAT_007d3ff8 + 0x20))(0xc, 0);
  return 1;
}
