// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00454c10 (h3): 100% (regs 100%, whole symbol byte-identical). Raw form 81.8% (Ghidra typed it
   __fastcall(ECX,EDX); ECX is never read, the body zeroes it as a scratch). One argument, the 0x14-byte entry, in
   EDX: a file-static callee whose caller already holds the entry in EDX (pilot finding 1). The file uses a stand-in
   caller shaped like the original's caller at 0x00454db0 (not in pc_funcs.csv, not a MATCH target). The slot
   search over the 4 x 0x18 table at 0x006882f8 is a pointer loop with an int-cast bound and an early return.
   Field names are offsets only. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct V3 { float x; float y; float z; } V3;
typedef struct Clump20 { int f0; int *f4; } Clump20;
typedef struct Mat40 { char pad0[0x30]; V3 f30; } Mat40;
typedef struct E14 { int *f0[2]; Mat40 *f8; int fc; Clump20 *f10; } E14;
typedef struct E18 { V3 f0; int fc; int f10; int f14; } E18;
MGLOBALA(E14, DAT_00688358, 4, 0x00688358);
MGLOBALA(E18, DAT_006882f8, 4, 0x006882f8);
int FUN_004e4800(Clump20 *p0);
int FUN_004e45b0(int p0, Clump20 *p1);
int FUN_004c1480(int *p0, void *p1, int p2);
// MATCH: 0x00454c10 FUN_00454c10
static void FUN_00454c10(E14 *e)
{
  E18 *p;
  e->fc = 2;
  for (p = DAT_006882f8; (int)p < (int)(DAT_006882f8 + 4); p++) {
    if (p->fc == 0) {
      p->f10 = 0;
      p->f14 = 0;
      p->fc = 1;
      p->f0 = e->f8->f30;
      return;
    }
  }
}
typedef struct Obj0ac { char pad0[0x30]; V3 f30; char pad3c[0x70]; E14 *fac; } Obj0ac;
void FUN_00465ca0(int p0, V3 *p1);
/* stand-in caller (not a MATCH target): same shape as the original's caller at 0x00454db0 */
void standin_00454db0(Obj0ac *o, int k)
{
  V3 v;
  E14 *e;
  e = o->fac;
  if (k == 2 && e->fc == 1) {
    FUN_00454c10(e);
    v = o->f30;
    FUN_00465ca0(0x18, &v);
  }
}
