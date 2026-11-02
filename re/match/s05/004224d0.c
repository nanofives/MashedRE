// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x004224d0 (158 B): 100% exact (regs 100, ops 100) by hand + match_permute. Raw g2s 32.4%.
   Record init of the 0xf40-stride array at 0x006403e8: FUN_004b6520(rec, 0xf40) (cdecl, 2 args) then field
   stores. A struct pointer with typed fields gave 89.2% in Ghidra's store order (the two 0 stores merged early
   into eax). The permuter moved f28/f34 ahead of the two 0.5f stores: 100%. Field names are offsets only;
   0xf30/0xf3c are not written here. Array count 4 is a declaration size only. Residual: none. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct { int a, b, c, d; } V4_4d0;
typedef struct {
  char head[0xf00];
  V4_4d0 v;          /* 0xf00 */
  float f10, f14, f18, f1c, f20, f24, f28;
  int f2c;
  int f30;
  float f34;
  int f38;
  int f3c;
} Rec_4d0;   /* stride 0xf40 */
MGLOBALA(Rec_4d0, DAT_006403e8, 4, 0x006403e8);
MGLOBAL(float, DAT_005f6154, 0x005f6154);
void FUN_004b6520(void *p, int n);
// MATCH: 0x004224d0 FUN_004224d0
void FUN_004224d0(int param_1, V4_4d0 *param_2, int param_3)
{
  Rec_4d0 *r = &DAT_006403e8[param_1];
  FUN_004b6520(r, 0xf40);
  r->f2c = param_1;
  r->v = *param_2;
  r->f28 = DAT_005f6154;
  r->f34 = 0.0f;
  r->f1c = 0.5f;
  r->f24 = 0.5f;
  r->f10 = -1.8f;
  r->f14 = 1.0f;
  r->f18 = -0.85f;
  r->f20 = 0.0f;
  r->f38 = param_3;
}
